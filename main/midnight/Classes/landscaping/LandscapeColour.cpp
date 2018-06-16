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

static Color4B TimeOfDayColours[32][4];


Color4B LandscapeColour::Adjust( Color4B source, Color4F tint )
{
    return Color4B( source.r * tint.r, source.g * tint.g, source.b * tint.b, source.a );
}

void LandscapeColour::SetMovementColour(mxtime_t start,mxtime_t end)
{
    startTint[0] =  CreateTimeBrightness(start);
    auto adjust = Color4F( startTint[0] );
 
#if defined(_DAY_NIGHT_SHADER_)
    startTint[1] =  Adjust(TimeOfDayColours[start][1],adjust);
    startTint[2] =  Adjust(TimeOfDayColours[start][2],adjust);
    startTint[3] =  Adjust(TimeOfDayColours[start][3],adjust);
#endif

    // get time of day for target colour
    endTint[0] = CreateTimeBrightness(end);
    adjust = Color4F( endTint[0] );
                           
#if defined(_DAY_NIGHT_SHADER_)
    endTint[1] =  Adjust(TimeOfDayColours[end][1],adjust);
    endTint[2] =  Adjust(TimeOfDayColours[end][2],adjust);
    endTint[3] =  Adjust(TimeOfDayColours[end][3],adjust);
#endif
}

void LandscapeColour::SetLookColour(mxtime_t time)
{
    startTint[0]  = CreateTimeBrightness(time);
    endTint[0] = startTint[0];
    
    auto adjust = Color4F( startTint[0] );
    
    
#if defined(_DAY_NIGHT_SHADER_)
    startTint[1] =  Adjust(TimeOfDayColours[time][1],adjust);
    startTint[2] =  Adjust(TimeOfDayColours[time][2],adjust);
    startTint[3] =  Adjust(TimeOfDayColours[time][3],adjust);
    endTint[1]=startTint[1];
    endTint[2]=startTint[2];
    endTint[3]=startTint[3];
#endif
    
}


/*
 * Function name    : panel_look::SetTimeOfDay
 *
 * Return type        : void
 *
 * Arguments        : int time
 *
 * Description        :
 *
 */

void LandscapeColour::SetTimeOfDay ( mxtime_t time )
{
    timeofday = time ;
    startTint[0] = CreateTimeBrightness(timeofday);
#if defined (_DAY_NIGHT_SHADER_)
    startTint[1] =  TimeOfDayColours[time][1];
    startTint[2] =  TimeOfDayColours[time][2];
    startTint[3] =  TimeOfDayColours[time][3];
#endif
    
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

Color4B ConvertFromInt( int colour )
{
    return Color4B(_RED(colour),_GREEN(colour),_BLUE(colour),_ALPHA(colour));
}

Color4B LandscapeColour::CreateTimeBrightness ( mxtime_t time )
{
    Color4B colour = TimeOfDayColours[time][0];
    
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

Color4B LandscapeColour::CalcCurrentMovementTint ( u32 index )
{
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

void LandscapeColour::OnXmlInit ( chilli::xml::node* node )
{
    chilli::lib::xml::node* e = NULL ;
    
    if ( node == NULL )
        return;
    
    if ( (e = node->Find("day")) ) {
        
        FOREACHELEMENT(e,d) {
            if ( chilli::lib::c_stricmp(d->Value(),"time") == 0 ) {
                int id = d->ReadInt("id");
                TimeOfDayColours[id][0] = ConvertFromInt(d->ReadColour("shade",0)) ;
            }
        }
    }
    
#if defined(_DAY_NIGHT_SHADER_)

    if ( (e = node->Find("shade")) ) {
        FOREACHELEMENT(e,d) {
            if ( chilli::lib::c_stricmp(d->Value(),"time") == 0 ) {
                int id = d->ReadInt("id");
                TimeOfDayColours[id][2] = ConvertFromInt(d->ReadColour("fill",0)) ;
                TimeOfDayColours[id][1] = ConvertFromInt(d->ReadColour("outline",0)) ;
                TimeOfDayColours[id][3] = ConvertFromInt(d->ReadColour("tint",0xffffffff)) ;
            }
        }
    }
#endif

}

