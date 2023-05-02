//
//  LandscapeColour.cpp
//  midnight
//
//  Created by Chris Wild on 14/06/2018.
//

#include "LandscapeColour.h"
#include "../ui/uihelper.h"
#include "../system/settingsmanager.h"
#include "LandscapeView.h"

#include "../system/moonring.h"
#include "../system/shadermanager.h"

USING_NS_CC;


static Color4B TimeOfDayColours[MAX_TIME][MAX_SHADES];
static const int DefaultWhite = 0xffffffff;

inline Color4B ConvertFromInt( int colour )
{
    return Color4B(_RED(colour),_GREEN(colour),_BLUE(colour), _ALPHA(colour));
}

inline Color4B GetTint( mxtime_t time, TINT shade )
{
    return Color4B(TimeOfDayColours[time][(int)shade]);
}

LandscapeColour::LandscapeColour(LandscapeOptions* options) :
    mr(options->mr),
    options(options)
{
}

LandscapeColour::~LandscapeColour()
{
}

void LandscapeColour::updateTerrainNode( Node* node )
{
    auto tint1 = Color4F(CalcCurrentMovementTint(TINT::TerrainOutline));
    auto tint2 = Color4F(CalcCurrentMovementTint(TINT::TerrainFill));
    auto shader = mr->shader;
    
    shader->AttachShader(node, options->terrainTimeShader);
    shader->UpdateTerrainTimeShader(node, alpha_normal, tint1, tint2);
}

void LandscapeColour::updateCharacterNode( Node* node )
{
    auto fade = CalcCurrentMovementFade(TINT::Person);
    auto shader = mr->shader;
    
    shader->AttachShader(node, options->characterTimeShader);

#if defined(_LOM_)
    auto colour = Vec4(0,(5.0f/255.0f),(78.0f/255.0f),alpha_normal);
    #if defined(_TUNNELS_)
        if (options->isInTunnel)
            colour = Vec4((64.0f/255.0f),(64.0f/255.0f),0,alpha_normal);
    #endif
#endif

#if defined(_DDR_)
    auto colour = options->isInTunnel
        ? Vec4((80.0f/255.0f),0,0,alpha_normal)
        : Vec4(0,0,0,alpha_normal);
#endif

    
    shader->UpdateCharacterTimeShader(node, alpha_normal, fade, colour);
}



Color4B LandscapeColour::Adjust( Color4B source, Color4F tint )
{
    return Color4B( source.r * tint.r, source.g * tint.g, source.b * tint.b, source.a );
}

#define SET_TINT(x,y,z) \
    x[(int)z] = Adjust(GetTint(y,z),adjust)

void LandscapeColour::SetMovementColour(mxtime_t start,mxtime_t end)
{
    // TINT::Normal
    startTint[0] =  CreateTimeBrightness(start);
    auto adjust = Color4F( startTint[0] );
 
    SET_TINT(startTint, start, TINT::TerrainOutline);
    SET_TINT(startTint, start, TINT::TerrainFill);
    SET_TINT(startTint, start, TINT::Person);
    SET_TINT(startTint, start, TINT::Tunnel);
    
    
    // TINT::Normal
    // get time of day for target colour
    endTint[0] = CreateTimeBrightness(end);
    adjust = Color4F( endTint[0] );

    SET_TINT(endTint, end, TINT::TerrainOutline);
    SET_TINT(endTint, end, TINT::TerrainFill);
    SET_TINT(endTint, end, TINT::Person);
    SET_TINT(endTint, end, TINT::Tunnel);
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

f32 LandscapeColour::CalcCurrentMovementFade ( TINT shade )
{
    int index = (int)shade;
    if ( !options->isMoving )
    {
#if defined(_TUNNELS_)
        if (options->isInTunnel)
            return FROM_ALPHA(GetTint(timeofday, TINT::Tunnel).a);
#endif
        return FROM_ALPHA(startTint[index].a);
    }
    
    f32 a1 = FROM_ALPHA(startTint[index].a);
    f32 a2 = FROM_ALPHA(endTint[index].a);
    f32 a3 = a2-a1;
    a1 += a3*options->movementAmount;
    
    return a1;
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

void LandscapeColour::OnXmlInit ( XmlNode* node )
{
    XmlNode* e = nullptr ;
 
#define SET_COLOUR(x,y) \
    TimeOfDayColours[id][(int)x] = ConvertFromInt(xml::ReadColour(d,y,DefaultWhite))
    
    if ( node == NULL )
        return;
    
    if ( (e = xml::Find(node,"day")) ) {
        FOREACHELEMENT(e,d) {
            if ( chilli::lib::c_stricmp(d->Value(),"time") == 0 ) {
                int id = xml::ReadInt(d,"id");
                SET_COLOUR(TINT::Normal,"shade") ;
            }
        }
    }
    
    if ( (e = xml::Find(node,"people")) ) {
        FOREACHELEMENT(e,d) {
            if ( chilli::lib::c_stricmp(d->Value(),"time") == 0 ) {
                int id = xml::ReadInt(d,"id");
                SET_COLOUR(TINT::Person,"normal") ;
                SET_COLOUR(TINT::Tunnel,"tunnel") ;
            }
        }
    }
    
    if ( (e = xml::Find(node,"terrain")) ) {
        FOREACHELEMENT(e,d) {
            if ( chilli::lib::c_stricmp(d->Value(),"time") == 0 ) {
                int id = xml::ReadInt(d,"id");
                SET_COLOUR(TINT::TerrainFill,"fill") ; //2
                SET_COLOUR(TINT::TerrainOutline,"outline") ; //1
            }
        }
    }

}

