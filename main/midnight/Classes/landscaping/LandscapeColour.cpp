//
//  LandscapeColour.cpp
//  midnight
//
//  Created by Chris Wild on 14/06/2018.
//

#include "LandscapeColour.h"
#include "../ui/uihelper.h"
#include "../system/configmanager.h"
#include "LandscapeView.h"

USING_NS_CC;



static Color3B TimeOfDayColours[MAX_TIME][MAX_SHADES];
static const int DefaultWhite = 0xffffffff;

inline Color3B ConvertFromInt( int colour )
{
    return Color3B(_RED(colour),_GREEN(colour),_BLUE(colour));
}

inline Color4B GetTint( mxtime_t time, TINT shade )
{
    return Color4B(TimeOfDayColours[time][(int)shade]);
}



void LandscapeColour::updateNode( Node* node )
{
    auto tint1 = Color4F(CalcCurrentMovementTint(TINT::TerrainOutline));
    auto tint2 = Color4F(CalcCurrentMovementTint(TINT::TerrainFill));
    node->setGLProgramState( options->programState->clone() );
    node->getGLProgramState()->setUniformFloat("p_alpha", 1.0f);                    // alpha
    node->getGLProgramState()->setUniformVec4("p_left", Vec4(tint1.r,tint1.g,tint1.b,tint1.a));      // outline
    node->getGLProgramState()->setUniformVec4("p_right", Vec4(tint2.r,tint2.g,tint2.b,tint2.a));

}

Color4B LandscapeColour::Adjust( Color4B source, Color4F tint )
{
    return Color4B( source.r * tint.r, source.g * tint.g, source.b * tint.b, source.a );
}

#define SET_TINT(x,y,z) \
    x[(int)z] = Adjust(GetTint(y,z),adjust)

void LandscapeColour::SetMovementColour(mxtime_t start,mxtime_t end)
{
    startTint[0] =  CreateTimeBrightness(start);
    auto adjust = Color4F( startTint[0] );
 
    SET_TINT(startTint, start, TINT::TerrainOutline);
    SET_TINT(startTint, start, TINT::TerrainFill);
    
    // get time of day for target colour
    endTint[0] = CreateTimeBrightness(end);
    adjust = Color4F( endTint[0] );

    SET_TINT(endTint, end, TINT::TerrainOutline);
    SET_TINT(endTint, end, TINT::TerrainFill);
}

void LandscapeColour::SetLookColour(mxtime_t time)
{
    timeofday = time ;
    
    startTint[0]  = CreateTimeBrightness(time);
    
    auto adjust = Color4F( startTint[0] );
    
    SET_TINT(startTint, time, TINT::TerrainOutline);
    SET_TINT(startTint, time, TINT::TerrainFill);
    SET_TINT(startTint, time, TINT::Person);
    SET_TINT(startTint, time, TINT::Tunnel);
    
    endTint[0]=startTint[0];
    endTint[1]=startTint[1];
    endTint[2]=startTint[2];
    endTint[3]=startTint[3];
    endTint[4]=startTint[4];
}

/*
 * Function name    : panel_look::CreateTimeBrightness
 *
 * Return type        :
 *
 * Arguments        :
 *
 * Description        :
 *
 */

Color4B LandscapeColour::CreateTimeBrightness ( mxtime_t time )
{
    Color4B colour = GetTint(time, TINT::Normal);
    
    double red,green,blue;
    int    adjust=0;
    //
    //    variant        days;
    //    mxi->GetProperty( "sv_days", days);
    //
    //    if ( TimeOfDay[timeofday].moon_light ) {
    //        int moonno = ((s32)days%28)/2;
    //        adjust = moonlight[moonno] * 2;
    //    }
    //
    red = (colour.r*2)-1-adjust;
    green = (colour.g*2)-1-adjust;
    blue = (colour.b*2)-1-adjust;
    
    return Color4B(red,green,blue,255);
}

Color4B LandscapeColour::CalcCurrentMovementTint ( TINT shade )
{
    int index = (int)shade;
    
    if ( !options->isMoving )
        return startTint[index];
    
    double  r1,g1,b1;
    double  r2,g2,b2;
    
    double  r3,g3,b3;
    
    r1=startTint[index].r;
    g1=startTint[index].g;
    b1=startTint[index].b;
    
    
    r2=endTint[index].r;
    g2=endTint[index].g;
    b2=endTint[index].b;
    
    r3 = r2-r1;
    g3 = g2-g1;
    b3 = b2-b1;

    r1 += r3*options->movementAmount;
    g1 += g3*options->movementAmount;
    b1 += b3*options->movementAmount;
    
    return Color4B(r1,g1,b1,255);
}

Color3B LandscapeColour::GetPersonColour()
{
#if defined(_DDR_)
    if (options->isInTunnel)
        return Color3B(GetTint(timeofday, TINT::Tunnel));
#endif
    return Color3B(GetTint(timeofday, TINT::Person));
}

void LandscapeColour::OnXmlInit ( chilli::xml::node* node )
{
    chilli::lib::xml::node* e = NULL ;
 
#define SET_COLOUR(x,y) \
    TimeOfDayColours[id][(int)x] = ConvertFromInt(d->ReadColour(y,DefaultWhite))
    
    if ( node == NULL )
        return;
    
    if ( (e = node->Find("day")) ) {
        FOREACHELEMENT(e,d) {
            if ( chilli::lib::c_stricmp(d->Value(),"time") == 0 ) {
                int id = d->ReadInt("id");
                SET_COLOUR(TINT::Normal,"shade") ;
            }
        }
    }
    
    if ( (e = node->Find("people")) ) {
        FOREACHELEMENT(e,d) {
            if ( chilli::lib::c_stricmp(d->Value(),"time") == 0 ) {
                int id = d->ReadInt("id");
                SET_COLOUR(TINT::Person,"normal") ;
                SET_COLOUR(TINT::Tunnel,"tunnel") ;
            }
        }
    }
    
    if ( (e = node->Find("terrain")) ) {
        FOREACHELEMENT(e,d) {
            if ( chilli::lib::c_stricmp(d->Value(),"time") == 0 ) {
                int id = d->ReadInt("id");
                SET_COLOUR(TINT::TerrainFill,"fill") ; //2
                SET_COLOUR(TINT::TerrainOutline,"outline") ; //1
            }
        }
    }

}

