/*
 * FILE:    processor_text.cpp
 * 
 * PROJECT: MidnightEngine
 * 
 * CREATED: 
 * 
 * AUTHOR:  Chris Wild
 * 
 * Copyright 2011 Chilli Hugger. All rights reserved.
 * 
 * PURPOSE: 
 * 
 * 
 */

#include "../baseinc/tme_internal.h"

#if defined(_DDR_)
#include "../scenarios/ddr/scenario_ddr_internal.h"
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <string.h>
#include <algorithm>

using namespace chilli::lib::StringExtensions;


namespace tme {

//namespace processors {




/*
 * Function name    : text::text
 * 
 * Return type        : 
 * 
 * Arguments        : 
 * 
 * Description        : 
 * 
 */

mxtext::mxtext() :
    m_case(CASE_NONE),
    last_number(0)
{
}


/*
 * Function name    : text::~text
 * 
 * Return type        : 
 * 
 * Arguments        : 
 * 
 * Description        : 
 * 
 */

mxtext::~mxtext()
{
}

/*
 * Function name    : text::FillArrayFromSystemString
 * 
 * Return type        : void
 * 
 * Arguments        : LPSTR array[]
 *                  : int id
 * 
 * Description        : 
 * 
 */

c_string mxtext::FillArrayFromSystemString( u32 id )
{
c_string values;
    std::string value = SystemString(id);
    StringExtensions::split(value, '|', values);
    std::string oldString = "-";
    std::string newString = "";
    std::replace (values.begin(), values.end(), oldString, newString);
    return values;
}


/*
 * Function name    : text::Serialize
 * 
 * Return type        : void
 * 
 * Arguments        : archive& ar
 * 
 * Description        : 
 * 
 */

void mxtext::Serialize ( archive& ar )
{
u32 ii;
int id;

    if ( ar.IsStoring() ) {
        ar << m_cSystemStrings;
        for ( ii=0; ii<m_cSystemStrings; ii++ ) {
            ar << ii;
            ar << systemcodes[ii];
            ar << systemstrings[ii];
        }
    }else{
        
        std::string empty = "";
        systemstrings.Clear();
        
        ar >> m_cSystemStrings;
    
        systemcodes.resize(m_cSystemStrings, empty);
        systemstrings.resize(m_cSystemStrings, empty);
        
        for ( ii=0; ii<m_cSystemStrings; ii++ ) {
            ar >> id;
            ar >> systemcodes[id];
            ar >> systemstrings[id];
        }

        adverb_token = FillArrayFromSystemString( SS_ADVERBS );
        number_token = FillArrayFromSystemString( SS_NUMBERS );
        fear_token = FillArrayFromSystemString( SS_FEARTOKENS );
        courage_token = FillArrayFromSystemString( SS_COURAGETOKENS );
        energy_token = FillArrayFromSystemString( SS_ENERGYTOKENS );
        zero_token = FillArrayFromSystemString( SS_ZEROTOKENS );
        despondent_token = FillArrayFromSystemString( SS_DESPONDENTTOKENS );
        reckless_token = FillArrayFromSystemString( SS_RECKLESSTOKENS );
        traits_token = FillArrayFromSystemString( SS_TRAITS );
        plural_tokens = FillArrayFromSystemString( SS_PLURALTOKENS );

    }
}


mxid mxtext::StringByName ( const std::string& name ) const
{
    for ( u32 ii=0; ii<m_cSystemStrings; ii++ ) {
        if (name == systemcodes[ii]) {
            return MAKE_ID(IDT_STRING,(ii+1));
        }
    }

    return IDT_NONE;
}

LPCSTR mxtext::SystemStringById ( mxid id )
{
    if ( ID_TYPE(id) != IDT_STRING )
        return emptyString.c_str() ;
    return SystemString(GET_ID(id)-1);
}


/*
 * Function name    : text::SystemString
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : int id
 * 
 * Description        : 
 * 
 */

LPCSTR mxtext::SystemString ( u32 id )
{
    //_MXASSERTE ( id>=0 && id<m_cSystemStrings );
    if ( /*id<0 ||*/ id>=m_cSystemStrings ) return emptyString.c_str() ;
    return systemstrings[id].c_str();
}


/*
 * Function name    : text::CookedSystemString
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : int id
 *                  : const mxcharacter* character
 * 
 * Description        : 
 * 
 */

std::string mxtext::CookedSystemString ( u32 id, const mxcharacter* character )
{
    std::string text = SystemString(id);
    auto result =  CookText(text,character) ;
    return result;
}



/*
 * Function name    : text::SpecialStrings
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : int id
 *                  : const mxcharacter* character
 * 
 * Description        : 
 * 
 */

std::string mxtext::SpecialStrings ( LPCSTR token, const mxcharacter* character )
{
std::string text;

    if (c_stricmp(token,"days") == 0 ) {
        if ( sv_days > 999 ) {
            text = "countless";
            last_number=sv_days;
        }else{
            text = DescribeNumber(sv_days);
        }
        return text;
    }
    return "{NONE}" ;
}

/*
 * Function name    : text::HowMuchOfText
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : int number
 *                  : LPSTR text1
 *                  : LPSTR text2
 * 
 * Description        : 
 * 
 */

std::string mxtext::HowMuchOfText( u32 number, const c_string& tokens )
{
std::string buffer;

    if ( number >= adverb_token.Count() )
        number=0;
    
    auto token = number < 5 ? 1 : 0;

    if ( !adverb_token.at(number).empty())
        buffer += adverb_token.at(number) ;

    return buffer + tokens.at(token);
}


/*
 * Function name    : text::DescribeNumber
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : int number
 * 
 * Description        : 
 * 
 */

std::string mxtext::DescribeNumber ( int number, ZERO_MODE zerotype )
{
    last_number=number;
    return DescribeNumberPart(number,zerotype);
}


/*
 * Function name    : text::DescribeNumberPart
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : int number
 * 
 * Description        : 
 * 
 */

std::string mxtext::DescribeNumberPart ( int number, ZERO_MODE zeromode )
{
bool    useand;
std::string buffer;

    useand = false ;
    int thousands = number / 1000 ;
    number %= 1000 ;
    int hundreds = number / 100 ;
    number %= 100 ;
    int tens = number / 10 ;
    number %= 10 ;

    if ( thousands ) {
        buffer += DescribeNumberPart ( thousands,zeromode ) + " " + number_token[28];
        useand = true ;
    }

    if ( hundreds ) {
        if ( useand ) buffer += " ";
        buffer += DescribeNumberPart ( hundreds,zeromode ) + " " + number_token[27];
        useand = TRUE ;
    }

    if ( tens>1 ) {
        if ( useand ) buffer += " and " ;
        buffer += number_token[19+tens-2] ;
        useand = false ;
        if ( number == 0 ) return buffer ;
        buffer += " " ;
    }else if ( tens == 1 ) 
        number+=10;

    if ( useand ) {
        if ( number == 0 ) return buffer;
        buffer += " and " ;
    }
    if ( number == 0 )
        buffer += zero_token[zeromode+1];
        else
        buffer += number_token[number-1];
    return buffer ;
}

#if defined(_DDR_)
/*
 *
 *
 */

std::string& mxtext::DescribeTime ( u32 time )
{
    return DescribeNumber((time+1)/2);
}
#endif
    
std::string mxtext::DescribeEnergy ( u32 energy )
{
    auto txtEnergy = HowMuchOfText ( energy/sv_energy_scale, energy_token );
    if ( energy<=sv_energy_cannot_continue) {
        return Format ( SystemString(SS_CHARACTER_CANNOT_CONTINUE), txtEnergy.c_str());
    }
    
    return txtEnergy;
}

std::string mxtext::DescribeDespondent ( u32 despondent )
{
    return HowMuchOfText ( despondent/sv_despondent_scale, despondent_token );
}

std::string mxtext::DescribeReckless ( u32 reckless )
{
    return HowMuchOfText ( reckless/sv_reckless_scale, reckless_token );
}

std::string mxtext::DescribeFear ( u32 fear )
{
    return HowMuchOfText ( 7-(fear/sv_fear_scale), fear_token );
}

std::string mxtext::DescribeCourage ( u32 courage )
{
    return HowMuchOfText ( courage/sv_courage_scale, courage_token );
}


/*
 * Function name    : text::DescribePlural
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : LPSTR single
 *                  : LPSTR plural
 *                  : int number
 * 
 * Description        : 
 * 
 */

std::string mxtext::DescribePlural( u32 token, int number )
{
    if ( ABS(number) > 1 ) 
        token++;
    return plural_tokens[token];
}

/*
 * Function name    : text::DescribeCharacterRecruitMen
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : const mxcharacter* character
 *                  : const mxstronghold* stronghold
 *                  : int qty
 * 
 * Description        : 
 * 
 */

std::string mxtext::DescribeCharacterRecruitMen ( const mxcharacter* character, const mxstronghold* stronghold, u32 qty )
{
    mxunitinfo* uinfo = mx->UnitById(stronghold->Type());
    auto buffer = Format (
        SystemString(SS_RECRUITMEN),
        qty,
        uinfo->Name().c_str(),
        stronghold->Total(),
        uinfo->Name().c_str()
        );

    return CookText(buffer,character);
}

#if defined(_DDR_)
std::string mxtext::DescribeObjectLocation(mxobject* object)
{
    mxcharacter* c = mx->scenario->WhoHasObject(object) ;
    
    // object not carried
    if ( c == nullptr ) {
        return Format ( SystemString(SS_SEEK_MSG2),
                         DescribeObjectWithPower(object).c_str(),
                         DescribeLocationWithPrep(object->Location(),nullptr).c_str()
                         );
    }else{
        auto buffer = Format (
                         SystemString(SS_SEEK_MSG3),
                         DescribeObjectWithPower(object).c_str()
                         );
        return CookText(buffer,c);
    }
}
#endif
    
/*
 * Function name    : text::DescribeCharacterPostMen
 *
 * Return type        : LPSTR
 *
 * Arguments        : const mxcharacter* character
 *                  : const mxstronghold* stronghold
 *                  : int qty
 * 
 * Description        : 
 * 
 */

std::string mxtext::DescribeCharacterPostMen ( const mxcharacter* character, const mxstronghold* stronghold, u32 qty )
{
    mxunitinfo* uinfo = mx->UnitById(stronghold->Type());
    auto buffer = Format (
        SystemString(SS_POSTMEN),
        qty,
        uinfo->Name().c_str(),
        stronghold->Total(),
        uinfo->Name().c_str()
        );

    return CookText(buffer,character);
}


/*
 * Function name    : text::DescribeCharacterTime
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : const mxcharacter* character
 * 
 * Description        : 
 * 
 */

std::string mxtext::DescribeCharacterTime( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";
    
    int time = character->Time();

    if ( time == sv_time_night )
        return SystemString(SS_ITISNIGHT);
    else if ( time == sv_time_dawn )
       return SystemString(SS_ITISDAWN);
    else {
        if ( time&1 )
            return CookedSystemString(SS_LESSTHAN, character);
        else
            return CookedSystemString(SS_HOURSOFDAY,character);
    }
}


/*
 * Function name    : text::DescribeCharacterEnergy
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : const mxcharacter* character
 * 
 * Description        : 
 * 
 */

std::string mxtext::DescribeCharacterEnergy ( const mxcharacter* character )
{
    return CookedSystemString(SS_CHARACTER_ENERGY,character);
}

/*
 * Function name    : text::DescribeCharacterCourage
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : const mxcharacter* character
 * 
 * Description        : 
 * 
 */

std::string mxtext::DescribeCharacterCourage ( const mxcharacter* character )
{
    return CookedSystemString(SS_CHARACTER_COURAGE,character);
}

/*
 * Function name    : text::DescribeCharacterFear
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : const mxcharacter* character
 * 
 * Description        : 
 * 
 */

std::string mxtext::DescribeCharacterFear ( const mxcharacter* character )
{
    return CookedSystemString(SS_CHARACTER_FEAR,character);
}


/*
 * Function name    : text::DescribeCharacterObject
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : const mxcharacter* character
 * 
 * Description        : 
 * 
 */

std::string mxtext::DescribeCharacterObject ( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";

    if ( character->carrying == nullptr )
        return CookedSystemString(SS_CARRYNOTHING,character);
    return CookedSystemString(SS_CARRYING,character);
}

std::string mxtext::DescribeObject ( const mxobject* object )
{
    std::string description = oinfo->description;
    const mxobject* temp = oinfo;
    oinfo = object;
    auto text = CookText(description,nullptr);
    oinfo = temp ;
    return text;
}
    
#if defined(_DDR_)
std::string mxtext::DescribeObjectWithPower ( const mxobject* object )
{
    std::string description = "{obj:text}, whose power is in {case:lower}{obj:power}";
    const mxobject* temp = oinfo;
    oinfo = object;
    auto buffer = CookText(description, nullptr);
    oinfo = temp ;
    return buffer;
}
#endif

    /*
 * Function name    : text::DescribeCharacterDeath
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : const mxcharacter* character
 * 
 * Description        : 
 * 
 */
/*        Wolves slew him.
 *        He was killed in battle.
 */

std::string mxtext::DescribeCharacterDeath ( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";
        
    if ( character->killedbyobject != nullptr )
        return CookedSystemString(SS_KILLED_OBJECT,character);
    return Format (
        CookedSystemString(SS_KILLED_BATTLE,character).c_str(),
        DescribeCharacterBattle(character).c_str()
        );
}

#if defined(_DDR_)
std::string mxtext::DescribeCharacterDeath2 ( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";
        
    if ( character->killedbyobject != nullptr )
        return CookedSystemString(SS_KILLED_OBJECT,character);
    return CookedSystemString(SS_KILLED_BY,character);
}
#endif


/*
 * Function name    : text::DescribeCharacterBattle
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : const mxcharacter* character
 * 
 * Description        : 
 * 
 */
/*        In the Battle of Ashimar, Shadows lost 110 warriors and 10 riders.
 *        Shadows alone slew 35 of the enemy. 
 *        His warriors slew 290.
 *        His riders slew 100.
 *        The battle continues.
 *        Victory went to the free!
 */
std::string mxtext::DescribeCharacterBattle ( const mxcharacter* character )
{
std::string buffer;
    
    RETURN_IF_NULL(character) "";
        
    int temp=0;
    temp += character->warriors.Lost() ? 1 : 0 ;
    temp += character->riders.Lost() ? 2 : 0 ;
    
    switch ( temp ) {
        case 0:
            buffer = CookedSystemString(SS_BATTLE_NONE,character);
            break;
        case 1:
            buffer = Format (
                CookedSystemString(SS_BATTLE_WARRIORS,character).c_str(),
                DescribeNumber(character->warriors.Lost()).c_str() );
            break;
        case 2:
            buffer = Format (
                CookedSystemString(SS_BATTLE_RIDERS,character).c_str(),
                DescribeNumber(character->riders.Lost()).c_str() );
            break;
        case 3:
            buffer = Format (
                CookedSystemString(SS_BATTLE_WARRIORS_RIDERS,character).c_str(),
                DescribeNumber(character->warriors.Lost(),ZERO_NO).c_str(),
                DescribeNumber(character->riders.Lost(),ZERO_NO).c_str());
            break;
    }

    buffer += Format (
        CookedSystemString(SS_BATTLE_CHARACTER_SLEW,character).c_str(),
        DescribeNumber(character->battleslew,ZERO_NONE).c_str() );

    if ( character->warriors.Killed() ) {
        buffer += Format (
            CookedSystemString(SS_BATTLE_WARRIORS_SLEW,character).c_str(),
            DescribeNumber(character->warriors.Killed(),ZERO_NONE).c_str() );
    }

    if ( character->riders.Killed() ) {
        buffer += Format (
            CookedSystemString(SS_BATTLE_RIDERS_SLEW,character).c_str(),
            DescribeNumber(character->riders.Killed(),ZERO_NONE).c_str() );
    }

    if ( !character->IsInBattle() ) {
        if ( character->HasWonBattle() )
            buffer += CookedSystemString(SS_BATTLE_VICTORY_FREE,character);
        else
            buffer += CookedSystemString(SS_BATTLE_VICTORY_ENEMY,character);
    } else
        buffer += CookedSystemString(SS_BATTLE_VICTORY_NONE,character);

    return buffer;
}


/*
 * Function name    : text::DescribeCharacterArmy
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : const mxcharacter* character
 * 
 * Description        : 
 * 
 */
/*        Luxor the Moonprince commands no warriors and no riders.
 *        His warriors are quite tired. His riders are quite tired.
 */


std::string mxtext::DescribeCharacterArmy ( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";
        
    int temp=0;
    temp += character->warriors.Total() ? 1 : 0 ;
    temp += character->riders.Total() ? 2 : 0 ;


    switch ( temp ) {
        case 0:
            return CookedSystemString(SS_ARMY_NONE,character);
        case 1:
            return Format (
                CookedSystemString(SS_ARMY_WARRIORS,character).c_str(),
                DescribeNumber(character->warriors.Total()).c_str(),
                DescribeEnergy( character->warriors.Energy()).c_str()
                );
        case 2:
            return Format (
                CookedSystemString(SS_ARMY_RIDERS,character).c_str(),
                DescribeNumber(character->riders.Total()).c_str(),
                DescribeEnergy ( character->riders.Energy()).c_str()
                );
        case 3:
            return Format (
                CookedSystemString(SS_ARMY_WARRIORS_RIDERS,character).c_str(),
                DescribeNumber(character->warriors.Total()).c_str(),
                DescribeNumber(character->riders.Total()).c_str(),
                DescribeEnergy ( character->warriors.Energy()).c_str(),
                DescribeEnergy ( character->riders.Energy()).c_str()
                );
        default:
            return "";
    }
}




/*
 * Function name    : text::DescribeCharacterTraits
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : const mxcharacter* character
 * 
 * Description        : 
 * 
 */

std::string mxtext::DescribeCharacterTraits ( const mxcharacter* character )
{
std::string buffer;
    
    RETURN_IF_NULL(character) "";
    
    u32 f = character->traits ;
    bool first = true;

    for ( int ii=0; ii<32; ii++ ) {
        int i = f&1;

        if ( !traits_token.at(ii).empty() ) {

            if ( i ) {
                if ( !first ) 
                    buffer += ", ";

                buffer += traits_token.at(ii) ;
                first = false ;
            }

        }

        f>>=1;

    }

    return buffer;
}

std::string mxtext::DescribeCharacterFoe ( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";
    
    return character->foe
        ? CookedSystemString(SS_FOE,character)
        : "";
}

std::string mxtext::DescribeCharacterLiege ( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";
    
    return character->liege
        ? CookedSystemString(SS_LIEGE,character)
        : "";
}

#if defined(_DDR_)
    
std::string mxtext::DescribeCharacterInBattle ( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";
    
    return character->IsPreparingForBattle()
        ? CookedSystemString(SS_BATTLE_PREPARES_BATTLE,character)
        : "";
}
    
std::string mxtext::DescribeCharacterLoyalty ( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";
    
    return character->loyalty
        ? CookedSystemString(SS_LOYAL_TO,character)
        : "";
}
    
#endif
    
std::string mxtext::DescribeCharacterGroup ( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";
    
    if ( character->following )
        return CookedSystemString(SS_GROUP_FOLLOWER,character);
    else if ( character->followers )
        return CookedSystemString(SS_GROUP_LEADER,character);
    return "";
}

 
    
/*
 * Function name    : text::DescribeCharacterLocation
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : const mxcharacter* character
 * 
 * Description        : 
 * 
 */
    
std::string mxtext::DescribeCharacterLocation( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";
    
    mxloc& here = mx->gamemap->GetAt ( character->Location() );
    mxgridref loc = mx->scenario->FindLookingTowards(character->Location(),character->Looking());
    mxloc& there = mx->gamemap->GetAt ( loc );
    
    if ( (here.area!=there.area) ||
        (here.IsInDomain()!=there.IsInDomain()) ||
        (here.terrain!=there.terrain) )
        {
            return CookedSystemString(SS_MESSAGE6,character);
        }else{
            return CookedSystemString(SS_MESSAGE5,character);
        }
    
    return "";
}

#if defined(_DDR_)
//
// Luxor sees [the object of type][ and ]an underground entrance.
//
// {char:name} sees
// and
// and underground entrance
// .
std::string mxtext::DescribeCharacterSees ( const mxcharacter* character )
{
std::string buffer;

    RETURN_IF_NULL(character) "";
 
    auto scenario = static_cast<ddr_x*>(mx->scenario);
    
    mxobject* object = scenario->FindObjectAtLocation(character->Location());
    bool entrance = mx->gamemap->HasTunnelEntrance(character->Location());

    if ( object== nullptr && !entrance )
        return "";
    
    byffer = character->Shortname() + " sees ";
    if ( object ) {
        buffer += "the " + DescribeObjectWithPower(object) ;
    }
    if ( entrance ) {
        if ( object )
            buffer += "and ";
        buffer += "an underground entrance";
    }
    buffer += ". ";
    
    return buffer;
}

std::string mxtext::DescribeLocationWithPrep ( mxgridref loc, const mxcharacter* character )
{
    mxgridref oldLoc = this->loc;
    
    this->loc = loc ;
    
    if ( character && character->IsInTunnel() ) {
        return "in the tunnel";
    }
    
    auto buffer = CookText("{loc:terrain:prep} ") + DescribeLocation(loc);
    
    this->loc = oldLoc ;
    
    return buffer;
}
#endif
    
    
    
/*
 * Function name    : text::DescribeLocation
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : gridref loc
 * 
 * Description        : 
 * 
 */

std::string mxtext::DescribeLocation(mxgridref loc )
{
mxloc&    mapsqr = mx->gamemap->GetAt(loc);
    this->loc = loc ;
    return mapsqr.IsInDomain()
        ? CookedSystemString(SS_LOCATION_DOMAIN)
        : CookedSystemString(SS_LOCATION_NOTDOMAIN);
}



/*
 * Function name    : text::DescribeTerrainPlural
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : int terrain
 * 
 * Description        : 
 * 
 */

std::string mxtext::DescribeTerrainPlural(mxterrain_t terrain)
{
    tinfo = mx->TerrainById( terrain );
    return tinfo->IsPlural()
        ? CookedSystemString(SS_TERRAIN_PLURAL).c_str()
        : tinfo->Name().c_str();

}


/*
 * Function name    : text::DescribeTerrainSingularPlural
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : int terrain
 * 
 * Description        : 
 * 
 */

std::string mxtext::DescribeTerrainSingularPlural(mxterrain_t terrain)
{
    tinfo = mx->TerrainById( terrain );
    return tinfo->IsPlural()
        ? CookedSystemString(SS_TERRAIN_PLURAL)
        : CookedSystemString(SS_TERRAIN_SINGLE);
}


/*
 * Function name    : text::DescribeArea
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : int area
 * 
 * Description        : 
 * 
 */

std::string mxtext::DescribeArea(u32 area)
{
    ainfo = mx->AreaById( area );
    if ( !ainfo->prefix.empty() )
        return CookedSystemString(SS_AREA_PREFIXED);
    return ainfo->Name();
}

/*
 * Function name    : text::DescribeStronghold
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : const mxstronghold* stronghold
 * 
 * Description        : 
 * 
 */
/*
 *  LOM:
 *  The stronghold holds x warriors of the free.
 *    They are slightly tired.
 *    The strongholds is owned by x.
 *
 *  DDR:
 *  The fortress of Carorthand holds then hundred, two score and ten fey riders.
 *  They are slightly tired.
 *  The fortress is owned by x who is loyal to they fey.
 */

std::string mxtext::DescribeStronghold(const mxstronghold* stronghold)
{
mxterrain*    tinfo;

    mxloc& mapsqr = mx->gamemap->GetAt ( stronghold->Location() );

    auto buffer = Format (
        CookedSystemString(SS_STRONGHOLD3).c_str(),
        DescribeLocation(stronghold->Location()).c_str(),
        DescribeNumber( stronghold->Total()).c_str(),
        mx->UnitById(stronghold->Type())->Name().c_str(),
        mx->RaceById(stronghold->OccupyingRace())->SoldiersName().c_str() );

    if ( stronghold->Owner() ) {
        tinfo = mx->TerrainById( mapsqr.terrain );

        int message = SS_STRONGHOLD2 ;
        mxcharacter* character=stronghold->Owner() ;

        if ( stronghold->Occupier() && stronghold->Occupier() != stronghold->Owner() ) { 
            message = SS_STRONGHOLD1 ;
            character=stronghold->Occupier();
        }

        buffer += Format (
            CookedSystemString(message).c_str(),
            tinfo->Name().c_str(),
            character->Longname().c_str()
            );
    }
        
    return buffer;
}


std::string mxtext::DescribeArmyTotal(const mxarmytotal* army)
{
    return Format (
        CookedSystemString(SS_WARRIORS_RIDERS,army->character).c_str(),
        DescribeNumber(army->warriors).c_str(),
        DescribeNumber(army->riders).c_str()
        );
}



/*
 * Function name    : text::CookText
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : LPSTR input
 *                  : const mxcharacter* character
 * 
 * Description        : 
 * 
 */

     
    LPSTR strcat_upper ( LPSTR dst, LPCSTR src )
    {
        if ( src == NULL ) return dst;
        LPSTR cp = dst;
        while( *cp ) cp++;
        while( (*cp++ = toupper((*src++)) ) ) ;
        return( dst );
        
    }
    LPSTR strcat_lower ( LPSTR dst, LPCSTR src )
    {
        if ( src == NULL ) return dst;
        LPSTR cp = dst;
        while( *cp ) cp++;
        while( (*cp++ = tolower((*src++))) ) ;
        return( dst );
        
    }
    
std::string mxtext::CookText ( std::string& input, const mxcharacter* character )
{
char        buffer[1024] ;
char        token[64];
LPSTR       is;
LPSTR       os;
char        l;

    is = (LPSTR)input.c_str();
    os = buffer;
    *os='\0';

    
    if ( !input.empty() ) {
    for(;;) {
        l = *is++;
        // start of special token
        if ( l == '{' ) {
            int t=0;
            while(*is!='}') {token[t++]=*is++;};
            token[t]='\0';
            is++;
            *os='\0';

            std::string result = DecodeToken(token,character) ;
            
            if (!result.empty()) {
            
                if ( m_case == CASE_NONE ) {
                    c_strcat ( os, result.c_str()  );
                } else if ( m_case == CASE_FIRST ) {
                    os[0] = toupper(result.at(0));
                    os[1] = '\0';
                    strcat_lower ( os+1, result.substr(1).c_str()  );
                } else if ( m_case == CASE_UPPER ) {
                    strcat_upper ( os, result.c_str()  );
                } else if ( m_case == CASE_LOWER ) {
                    strcat_lower ( os, result.c_str()  );
                }
                
                os+= c_strlen(os);
                m_case=CASE_NONE;
            //}else{
            //
            }
            continue;
        }    
        *os++ = l;
        // end of input
        if ( l == '\0' ) {
            break;
        }
    }
    }

    return buffer;
}

/*
 * Function name    : text::DecodeToken
 * 
 * Return type        : LPSTR
 * 
 * Arguments        : LPSTR token
 *                  : const mxcharacter* character
 * 
 * Description        : 
 * 
 */
//enum TOKENS {
//    TOKEN_NONE=0,
//    TOKEN_FORMAT=1,
//    TOKEN_AREA,
//    TOKEN_GENDER,
//    TOKEN_DIRECTION,
//    TOKEN_CHARACTER,
//    TOKEN_LOCATION,
//    TOKEN_BATTLE,
//    TOKEN_OBJECT,
//    TOKEN_RACE,
//    TOKEN_STRING,
//    TOKEN_TERRAIN,
//    TOKEN_STRONGHOLD,
//    TOKEN_UNIT,
//    TOKEN_SPECIAL,
//};
//
//token_t str_tokens_1[] = {
//    { "format",        TOKEN_FORMAT },
//    { "area",        TOKEN_AREA },
//    { "gender",        TOKEN_GENDER },
//    { "dir",        TOKEN_DIRECTION },
//    { "char",        TOKEN_CHARACTER },
//    { "loc",        TOKEN_LOCATION },
//    { "battle",        TOKEN_BATTLE },
//    { "obj",        TOKEN_OBJECT },
//    { "race",        TOKEN_RACE },
//    { "str",        TOKEN_STRING },
//    { "terrain",    TOKEN_TERRAIN },
//    { "stronghold",    TOKEN_STRONGHOLD },
//    { "unit",        TOKEN_UNIT },
//    { "special",    TOKEN_SPECIAL },
//};


std::string mxtext::DecodeToken ( LPSTR token, const mxcharacter* character )
{
int         is=0;
c_string    tokens;

    LPSTR t = strtok(token,"{:}");
    while ( t ) {
        tokens.Add(t);
        t = strtok(NULL,"{:}");
    }

    //if ( tokens.Count() == 0 )
    //    return NULL ;

    if ( character==NULL )
        character=mx->CurrentChar();

    if ( character ) {
        ginfo = mx->GenderById(character->gender);
        dinfo = mx->DirectionById(character->Looking());
        rinfo = mx->RaceById(character->Race());
    }

    ainfo = mx->AreaById( mx->gamemap->GetAt(loc).area );
    tinfo = mx->TerrainById( mx->gamemap->GetAt(loc).terrain );

    is=0;

#define GET_ARG    \
    tokens.at(is)
    
#define IS_ARG(x)    \
     if ( c_stricmp(x,GET_ARG.c_str()) == 0 )

    while (TRUE) {
        if ( tokens.at(is).empty() )
            return "";

            IS_ARG("case") {
                is++;
                IS_ARG("first")     { m_case=CASE_FIRST; return ""; }
                IS_ARG("upper")     { m_case=CASE_UPPER; return ""; }
                IS_ARG("lower")     { m_case=CASE_LOWER; return ""; }
                IS_ARG("none")      { m_case=CASE_NONE; return ""; }
            }else
            IS_ARG("plural") {
                is++;
                if ( ABS(last_number)==1 ) {
                    return GET_ARG;
                }
                is++;
                return GET_ARG;
            }else
                
            IS_ARG("number") {
                is++;
                return DescribeNumber(atoi(GET_ARG.c_str()));
            }else
 // Character
            IS_ARG("char") {
__char:
                is++;
                RETURN_IF_NULL(character) "";
                
                IS_ARG("name")          return character->Shortname();
                IS_ARG("longname")      return character->Longname();
#if defined(_DDR_)
                IS_ARG("time")          return DescribeTime(character->Time());
#else
                IS_ARG("time")          return DescribeNumber((character->Time()+1)/2);
#endif
                IS_ARG("energy")        return DescribeEnergy(character->energy);
                IS_ARG("despondency")   return DescribeDespondent ( character->despondency );
                IS_ARG("reckless")      return DescribeReckless ( character->reckless );

                IS_ARG("fear")        {  
                                    character->ForcedVariableRefresh();
                                    return DescribeFear ( character->fear );
                                    }
                IS_ARG("courage")    {
                                    character->ForcedVariableRefresh();
                                    return DescribeCourage(character->courage);
                                    }

                IS_ARG("foe")        {
                                    character = character->foe;
                                    goto __char;
                                    }
                IS_ARG("liege")        {
                                    character = character->liege;
                                    goto __char;
                                    }
                IS_ARG("following")    {
                                    character = character->following;
                                    goto __char;
                                    }
#if defined(_DDR_)
                IS_ARG("loyalty")    {
                                    rinfo = mx->RaceById(character->Loyalty());
                                    goto __race;
                                    }
#endif
                IS_ARG("text")        {
                    is++;
                    IS_ARG("liege")     return DescribeCharacterLiege( character );
                    IS_ARG("foe")       return DescribeCharacterFoe( character );
                    IS_ARG("traits")    return DescribeCharacterTraits( character );
                    IS_ARG("time")      return DescribeCharacterTime( character );
                    IS_ARG("energy")    return DescribeCharacterEnergy ( character );
                    IS_ARG("courage")   return DescribeCharacterCourage ( character );
                    IS_ARG("fear")      return DescribeCharacterFear ( character );
                    IS_ARG("obj")       return DescribeCharacterObject ( character );
                    IS_ARG("death")     return DescribeCharacterDeath ( character );
                    IS_ARG("battle")    return DescribeCharacterBattle ( character );
                    IS_ARG("army")      return DescribeCharacterArmy ( character );
                    IS_ARG("loc")       return DescribeCharacterLocation( character );
                    IS_ARG("group")     return DescribeCharacterGroup( character );
#if defined(_DDR_)
                    IS_ARG("death2")    return DescribeCharacterDeath2 ( character );
                    IS_ARG("loyalty")   return DescribeCharacterLoyalty( character );
                    IS_ARG("sees")      return DescribeCharacterSees( character );
                    IS_ARG("inbattle")  return DescribeCharacterInBattle( character );
#endif
                }
            
                IS_ARG("lookat")    {
                                    loc = mx->scenario->FindLookingTowards(character->Location(),character->Looking());
                                    goto __loc;
                                    }
                IS_ARG("battle")    {
                    is++;
                    IS_ARG("loc")    {
                                    loc = character->battleloc;
                                    goto __loc;
                                    }
                    IS_ARG("obj")    {
                                    oinfo = character->killedbyobject; 
                                    goto __obj;
                                    }
#if defined(_DDR_)
                    IS_ARG("fighting")    {
                                    character = (mxcharacter*)((ddr_character*)character)->fighting_against;
                                    goto __char;
                                    }
#endif
                }
                
                IS_ARG("loc")        {
                                    loc = character->Location();
                                    goto __loc;
                                    }
                IS_ARG("gender")    {
                                    ginfo = mx->GenderById(character->gender);
                                    goto __gender;
                                    }
                IS_ARG("dir")        {
                                    dinfo = mx->DirectionById(character->Looking());
                                    goto __dir;
                                    }
                IS_ARG("obj")        {
                                    oinfo = character->carrying; //mx->ObjectById(character->object);
                                    goto __obj;
                                    }
                IS_ARG("race")        {
                                    rinfo = mx->RaceById(character->Race());
                                    goto __race;
                                    }
            }else
// RACE
            IS_ARG("race") {
__race:
                is++;
                IS_ARG("name")          return rinfo->Name();
                IS_ARG("soldiers")      return rinfo->SoldiersName();
            }else
// AREA
            IS_ARG("area") {
__area:
                is++;
                IS_ARG("name")          return ainfo->Name();
                IS_ARG("prefix")        return ainfo->prefix;
                IS_ARG("text")          return DescribeArea(ainfo->Id());
            }else
// GENDER
            IS_ARG("gender") {
__gender:
                is++;
                IS_ARG("name")          return ginfo->Name();
                IS_ARG("heshe")         return ginfo->pronoun1;
                IS_ARG("hisher")        return ginfo->pronoun2;
                IS_ARG("himher")        return ginfo->pronoun3;
            }else
// direction
            IS_ARG("dir") {
__dir:
                is++;
                IS_ARG("name")          return dinfo->Name();
            }else
//OBJECT
            IS_ARG("obj") {
__obj:
                is++;
                IS_ARG("name")          return oinfo->name;
                        // if oinfo->description == NULL
                        // SS_OBJECT_FULL_DESCRIPTION
                IS_ARG("text")      {
                                        std::string description = oinfo->description;
                                        return CookText(description,character);
                                    }
#if defined(_DDR_)
                IS_ARG("fullname")  return DescribeObjectWithPower(oinfo);
                IS_ARG("type")      {
                                        otinfo = mx->ObjectTypeById( static_cast<const ddr_object*>(oinfo)->type );
                                        return otinfo->Name();
                                    }
                IS_ARG("power")     {
                                        opinfo = mx->ObjectPowerById( static_cast<const ddr_object*>(oinfo)->power );
                                        return opinfo->Name();
                                    }
#endif
            }else

// LOCATION
            IS_ARG("loc") {
__loc:
                is++;
                IS_ARG("name")        return DescribeLocation(loc);
#if defined(_DDR_)
                IS_ARG("text")        return DescribeLocationWithPrep(loc,character);
#endif
                IS_ARG("terrain")    {
                                    tinfo = mx->TerrainById( mx->gamemap->GetAt(loc).terrain );
                                    goto __terrain;
                                    }
                IS_ARG("obj")    {
                                    oinfo = mx->ObjectById( mx->gamemap->GetAt(loc).object );
                                    goto __obj;
                                    }
                IS_ARG("area")        {    
                                    ainfo = mx->AreaById( mx->gamemap->GetAt(loc).area );
                                    goto __area;
                                    }
            }else
// TERRAIN
            IS_ARG("terrain") {
__terrain:
                is++;
                IS_ARG("name")      return tinfo->Name();
                IS_ARG("plural")    return DescribeTerrainPlural((mxterrain_t)tinfo->Id());
                IS_ARG("prep")      return tinfo->preposition;
                IS_ARG("text")      {
                                        std::string description = tinfo->description;
                                        return CookText(description,character);
                                    }
                IS_ARG("single")    return DescribeTerrainSingularPlural((mxterrain_t)tinfo->Id());
            }else
// SYSTEM STRING
            IS_ARG("str") {
                return CookedSystemString(StringExtensions::atol(tokens.at(is+1)),character);
            }else
// SPECIAL
            IS_ARG("special") {
                return SpecialStrings(tokens.at(is+1).c_str(),character);
            }
        is++;
    }
}


//}
// namespace processors

}
// namespace tme

//abcd(e)fg(hijk)l(mn)o(pq)rstu(vwx)yz

/*
Text Formatting
{f1}    :    uppercase letter
{f2}    :    uppercase word

Area
{a1}    :    Name
{a2}    :    Prefix
{a3}    :    text

Gender
{g1}    :    Name
{g2}    :    Pronoun1 ( he/she )
{g3}    :    Pronoun2 ( his/her )
{g4}    :    Pronoun3 ( him/her )

Direction
{d1}    :    Name

Character
{c1}    :    name - Shortname
{c2}    :    Longname
{c3}    :    time
{c4}    :    energy
{c5}    :    fear
{c6}    :    courage
text
    {c7}    :    Describe Time
    {c8}    :    Describe Energy
    {c9}    :    Describe Courage
    {c10}    :    Describe Fear
    {c11}    :    Describe Object Carrying
    {c12}    :    Describe Death
    {c13}    :    Describe Battle
    {c14}    :    Describe Army
    {c15}    :    Describe Location/Looking


{cl*}    :    location
{cml*}    :    location looking at
{cbl*}    :    battle location
{cg*}    :    gender
{cd*}    :    direction
{co*}    :    object
{cr*}    :    race
    
Object
{o1}    :    name
{o2}    :    Found Description
{o3}    :    object type
{o4}    :    object power
//{o5}    :    full description

Race
{r1}    :    Name
{r2}    :    Soldiers Name

Strings
{zx}    :    System Strings. where x is a number

Location
{l1}    :    name
{lt*}    :    terrain
{lo*}    :    object
{la*}    :    area


Terrain
{t1}    :    Name
{t2}    :    Plural Safe Name
{t3}    :    Preposition
{t4}    :    description
{t5}    :    single 'a terrain'


Strongholds
{s1}    :    Name
{sc*}    :    owner
{sr*}    :    race
{sl*}    :    location

Units
{u1}    :    Name

{y}        : Special
*/
