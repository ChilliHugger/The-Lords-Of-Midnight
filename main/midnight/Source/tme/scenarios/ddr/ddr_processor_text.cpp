/*
 * FILE:    ddr_processor_text.cpp
 * 
 * PROJECT: MidnightEngine
 * 
 * CREATED: 
 * 
 * AUTHOR:  Chris Wild
 * 
 * Copyright 2013 Chilli Hugger. All rights reserved.
 * 
 * PURPOSE: 
 * 
 * 
 */

#include "../../baseinc/tme_internal.h"
#include "ddr_processor_text.h"
#include "scenario_ddr_internal.h"

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#if defined(_DDR_)

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

ddr_text::ddr_text()
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

ddr_text::~ddr_text()
{
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

std::string ddr_text::HowMuchOfText( u32 number, const c_string& tokens )
{
std::string buffer;
    
    if ( number>= adverb_token.Count() )
        number=adverb_token.Count()-1;
        
    if ( !adverb_token.at(number).empty() ) {
        buffer = adverb_token[ number ] + " ";
    }

    return buffer + tokens[0];
}

std::string ddr_text::DescribeTime ( u32 time )
{
    return DescribeNumber((time+3)/4);
}

std::string ddr_text::DescribeCharacterTime( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";
    
    int time = character->Time();
    
    if ( time == sv_time_night )
        return SystemString(SS_ITISNIGHT) ;
    else if ( time == sv_time_dawn )
        return SystemString(SS_ITISDAWN) ;
    
    return CookedSystemString(SS_HOURSOFDAY,character);
}


std::string ddr_text::DescribeNumberPart ( int number, ZERO_MODE zeromode )
{
std::string buffer;
    
    bool needscomma=true;
    bool needsand=false;
    
    int units = number%20;
    int hundreds = number/20;
    
    if ( units == 0 )
        needscomma=false;
    
    int scores = hundreds%5;
    hundreds/=5;

    if ( hundreds ) {
        needsand=true;
        buffer = number_token[hundreds-1] + " " + number_token[27];

    }else
        needscomma=false;
    
    if ( scores ) {
        if ( needscomma ) {
            buffer += ", ";
        } else {
            if ( needsand )
                buffer += " and ";
        }
        buffer += number_token[scores-1] + " "+ number_token[29];
        needsand=true;
    }

    if ( units  ) {
        if ( needsand )
            buffer += " and ";
        buffer += number_token[units-1];
        needsand=true;
    }
    
    if ( needsand == 0 ) {
        // no
        buffer += zero_token[zeromode+1];
    }
    
    return buffer;
}

std::string ddr_text::DescribeObjectLocation(mxobject* object)
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
 *  The fortress of Carorthand holds ten hundred, two score and ten fey riders.
 *  They are slightly tired.
 *  The fortress is owned by x who is loyal to they fey.
 */

//73    SS_STRONGHOLD1    The %s is occupied by armies loyal to %s.§
//74    SS_STRONGHOLD2    The %s is owned by %s who is loyal to.§
//75    SS_STRONGHOLD3    %s holds %s %s %s.§

std::string ddr_text::DescribeStronghold(const mxstronghold* stronghold)
{
    mxterrain*    tinfo;
    
    auto garrison = static_cast<const ddr_stronghold*>(stronghold);
    
    mxloc& mapsqr = mx->gamemap->GetAt ( stronghold->Location() );
    
    auto buffer = Format (
                     CookedSystemString(SS_STRONGHOLD3).c_str(),
                     DescribeLocation(garrison->Location()).c_str(),
                     DescribeNumber( garrison->TotalTroops() ).c_str(),
                     mx->RaceById(garrison->OccupyingRace())->SoldiersName().c_str(),
                     mx->UnitById(garrison->Type())->Name().c_str(),
                     DescribeEnergy(garrison->Energy()).c_str()
                     );
    
    //
    // army energy
    
    if ( stronghold->Owner() ) {
        tinfo = mx->TerrainById( mapsqr.terrain );
        
        int message = SS_STRONGHOLD2 ;
        mxcharacter* character=stronghold->Owner() ;
        
        if (character->Race()==RA_MOONPRINCE ) {
            message = SS_STRONGHOLD1 ;
        }
        
        buffer += Format(
                    CookedSystemString(message,character).c_str(),
                    tinfo->Name().c_str()
                    );
    }
    
    return buffer;
}

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
/* In battle[ against Carorthand the fey], tarithel the fey won [no|a] victory.
 * Tarithel slew [Carorthand they fey|no one].
 * Tarithel lost x [riders|warriors].
 * Luxor slew [Carorthand the fey|no one].
 * Taritel's [riders|warriors] slew x of the foe.
 */
 
std::string ddr_text::DescribeCharacterBattle ( const mxcharacter* character )
{
std::string buffer;
    
    int temp=0;
    temp += character->warriors.Lost() ? 1 : 0 ;
    temp += character->riders.Lost() ? 2 : 0 ;
    
    //systemstrings[SS_BATTLE1]="In battle against %s, {char:name} ";
    //systemstrings[SS_BATTLE_NONE]="In battle, {char:name} ";
 
    // in battle
    ddr_character* ddr_char = (ddr_character*)character;
    if ( ddr_char->fighting_against != NULL ) {
        buffer += Format(
                    CookedSystemString(SS_BATTLE1,character).c_str(),
                    ddr_char->fighting_against->Longname().c_str()
                    );
    }else{
        buffer += CookedSystemString(SS_BATTLE_NONE,character);
    }

    //systemstrings[SS_BATTLE_VICTORY_FREE]="won a victory. ";
    //systemstrings[SS_BATTLE_VICTORY_ENEMY]="was defeated. ";
    //systemstrings[SS_BATTLE_VICTORY_NONE]="won no victory. ";
    if ( !character->IsInBattle() || character->HasWonBattle() ) {
        if ( character->HasWonBattle() )
            buffer += CookedSystemString(SS_BATTLE_VICTORY_FREE,character);
        else
            buffer += CookedSystemString(SS_BATTLE_VICTORY_ENEMY,character);
    } else
        buffer += CookedSystemString(SS_BATTLE_VICTORY_NONE,character);

    switch ( temp ) {
        case 0:
            // rorthron the wise has fought no battle.
            //buffer->strcpy ( CookedSystemString(SS_BATTLE_NONE,character) );
            break;
        case 1:
            buffer += Format(
                        CookedSystemString(SS_BATTLE_WARRIORS,character).c_str(),
                        DescribeNumber(character->warriors.Lost()).c_str()
                        );
            break;
        case 2:
            buffer += Format(
                        CookedSystemString(SS_BATTLE_RIDERS,character).c_str(),
                        DescribeNumber(character->riders.Lost()).c_str()
                        );
            break;
        case 3:
            buffer += Format(
                        CookedSystemString(SS_BATTLE_WARRIORS_RIDERS,character).c_str(),
                        DescribeNumber(character->warriors.Lost(),ZERO_NO).c_str(),
                        DescribeNumber(character->riders.Lost(),ZERO_NO).c_str()
                        );
            break;
    }


    //systemstrings[SS_BATTLE_CHARACTER_SLEW] = "{char:name} slew %s. ";
    if ( ddr_char->Flags().Is(cf_killed_foe) && ddr_char->fighting_against!=NULL ) {
        buffer += Format(
                    CookedSystemString(SS_BATTLE_CHARACTER_SLEW,character).c_str(),
                    ddr_char->fighting_against->Longname().c_str()
                    );
    } else {
        buffer += Format(
                    CookedSystemString(SS_BATTLE_CHARACTER_SLEW,character).c_str(),
                    "no one"
                    );
    }

    //systemstrings[SS_BATTLE_WARRIORS_SLEW]="{case:first}{char:gender:hisher} warriors slew %s of the foe. ";
    //systemstrings[SS_BATTLE_RIDERS_SLEW]="{case:first}{char:gender:hisher} riders slew %s of the foe. ";

    if ( character->warriors.Killed() ) {
        buffer += Format(
                 CookedSystemString(SS_BATTLE_WARRIORS_SLEW,character).c_str(),
                 DescribeNumber(character->warriors.Killed(),ZERO_NONE).c_str()
                 );
     }
    
    if ( character->riders.Killed() ) {
        buffer += Format(
                 CookedSystemString(SS_BATTLE_RIDERS_SLEW,character).c_str(),
                 DescribeNumber(character->riders.Killed(),ZERO_NONE).c_str()
                 );
     }
                 
    return buffer;
}

// USED: OnCharDeath "CharDeath"
//
std::string ddr_text::DescribeCharacterDeath ( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";
    const ddr_character* ddr_c = static_cast<const ddr_character*>(character);
    
    if ( ddr_c->killedbyobject ) {
        if ( ddr_c->KilledByBattleObject()) {
            // SS_KILLED_BY_BATTLE_OBJECT
            // {char:longname} is dead, slain by the {case:lower}{char:battle:obj:longname} at the hands of {char:battle:fighting:fullname}. {char:text:battle}
            return CookedSystemString(SS_KILLED_BY_BATTLE_OBJECT,character);
        } else {
            // SS_KILLED_OBJECT
            // {char:longname} is dead, slain by {case:lower}{char:battle:obj:name}.
            // Wolves | Dragons | Ice Trolls | Skulkrin
            return CookedSystemString(SS_KILLED_OBJECT,character);
        }
    }
        
    if (ddr_c->fighting_against && ddr_c->killedby == KB_LORD) {
        // SS_KILLED_BY_LORD
        // {char:longname} is dead, slain by {char:battle:fighting:fullname}. {char:text:battle}
        return CookedSystemString(SS_KILLED_BY_LORD, character);
    }
        
    // SS_KILLED_BATTLE
    // {char:longname} is dead, slain by sword. {char:text:battle}
    return CookedSystemString(SS_KILLED_BATTLE,character);
}

// USED:
// SS_CHARACTER_DEAD
// {char:longname} is dead, {char:text:death2}
// SS_DEFEAT2
// {char:longname} is dead, {char:text:death2} The evil Shareth has won victory!

std::string ddr_text::DescribeCharacterDeath2 ( const mxcharacter* character )
{
    const ddr_character* ddr_c = static_cast<const ddr_character*>(character);
    
    // SS_KILLED_BY_OBJECT
    // slain by {case:lower}{char:battle:obj:name}.
    if ( character->killedbyobject && !ddr_c->KilledByBattleObject() )
        return CookedSystemString(SS_KILLED_BY_OBJECT,character);
    
    // SS_KILLED_BY
    // killed in battle by {char:battle:fighting:longname}.
    if ( ddr_c->fighting_against && ddr_c->killedby == KB_LORD )
        return CookedSystemString(SS_KILLED_BY,character);
    
    // SS_KILLED_IN_BATTLE
    // killed in battle.
    return CookedSystemString(SS_KILLED_IN_BATTLE,character);
}


std::string ddr_text::DescribeVictory ( u32 victory, s32 value )
{
c_string tokens;
    
    // describe victory
    auto victoryToken = victory_token[value];
    tokens.push_back(victoryToken);
    tokens.push_back(victoryToken);
 
    return Format(
                    mx->text->CookedSystemString(SS_VICTORY4).c_str(),
                    mx->text->HowMuchOfText(victory, tokens).c_str()
                    );
}

std::string ddr_text::DescribeCharacterLocation( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";
    
    mxloc& here = mx->gamemap->GetAt ( character->Location() );
    mxgridref loc = mx->scenario->FindLookingTowards(character->Location(),character->Looking());
    mxloc& there = mx->gamemap->GetAt ( loc );
    
    if ( there.IsMisty() ) {
        return CookedSystemString(SS_MESSAGE_MIST,character);
    }

    return mxtext::DescribeCharacterLocation(character);
}

std::string ddr_text::DescribeCharacterInBattle ( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";
    const ddr_character* ddr_c = static_cast<const ddr_character*>(character);
    
    return ddr_c->IsPreparingForBattle()
        ? CookedSystemString(SS_BATTLE_PREPARES_BATTLE,character)
        : "";
}
    
std::string ddr_text::DescribeCharacterLoyalty ( const mxcharacter* character )
{
    RETURN_IF_NULL(character) "";
    
    return character->loyalty
        ? CookedSystemString(SS_LOYAL_TO,character)
        : "";
}

//
// Luxor sees [the object of type][ and ]an underground entrance.
//
// {char:name} sees
// and
// and underground entrance
// .
std::string ddr_text::DescribeCharacterSees ( const mxcharacter* character )
{
std::string buffer;

    RETURN_IF_NULL(character) "";
 
    auto scenario = static_cast<ddr_x*>(mx->scenario);
    auto object = scenario->FindObjectAtLocation(character->Location());
    bool entrance = mx->gamemap->HasTunnelEntrance(character->Location());

    if ( object == nullptr && !entrance )
        return "";
    
    auto msg = 0;
    auto type = 0;
    
    if ( object && object->CanPickup() )
        type = 1;
    
    if ( entrance )
        type += 2;
        
    switch (type) {
        case 1:
            // SS_SEES_1;
            // {char:name} sees {char:loc:obj}
            msg = SS_SEES_1;
            break;
        case 2:
            // SS_SEES_2;
            // {char:name} sees an underground entrance.
            msg = SS_SEES_2;
            break;
        case 3:
            // SS_SEES_3;
            // {char:name} sees {char:loc:obj} and an underground entrance.
            msg = SS_SEES_3;
            break;
    }
    
    return CookedSystemString(msg,character);

}

std::string ddr_text::DescribeObjectWithPower ( const mxobject* object )
{
    RETURN_IF_NULL(object) "";
    
    std::string description = "{obj:text}, whose power is in {case:lower}{obj:power}";
    const mxobject* temp = oinfo;
    oinfo = object;
    auto buffer = CookText(description, nullptr);
    oinfo = temp ;
    return buffer;
}

void ddr_text::Serialize ( archive& ar )
{
    mxtext::Serialize(ar);
    if ( ar.IsStoring() )
        return;
        
    plural_tokens = FillArrayFromSystemString( SS_PLURALTOKENS );

        
    systemstrings[SS_KILLED_BY_OBJECT] = "slain by {case:lower}{char:battle:obj:name}. ";
    systemstrings[SS_KILLED_BY_BATTLE_OBJECT] = "{char:longname} is dead, slain by {char:battle:fighting:longname} with the {case:lower}{char:battle:obj:text}. {char:text:battle}";
    systemstrings[SS_KILLED_BATTLE] = "{char:longname} is dead, slain by sword. {char:text:battle}";
    systemstrings[SS_KILLED_BY_LORD] = "{char:longname} is dead, slain by {char:battle:fighting:longname}. {char:text:battle}";
    systemstrings[SS_SEEK_MSG4] = ", \"{char:longname} is dead.\"";

    systemstrings[SS_SEES_1] = "{char:name} sees the {char:loc:obj:fullname}. ";
    systemstrings[SS_SEES_2] = "{char:name} sees an underground entrance. ";
    systemstrings[SS_SEES_3] = "{char:name} sees the {char:loc:obj:fullname} and an underground entrance. ";

    victory_token = FillArrayFromSystemString( SS_TOKENS_VICTORY );

}
    
} // namespace:tme

#endif // _DDR_
