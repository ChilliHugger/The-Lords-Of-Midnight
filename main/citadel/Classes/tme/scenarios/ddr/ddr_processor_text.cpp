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

namespace tme {

//namespace processors {

/*
 * Function name	: text::text
 * 
 * Return type		: 
 * 
 * Arguments		: 
 * 
 * Description	    : 
 * 
 */

ddr_text::ddr_text()
{
}


/*
 * Function name	: text::~text
 * 
 * Return type		: 
 * 
 * Arguments		: 
 * 
 * Description	    : 
 * 
 */

ddr_text::~ddr_text()
{
}
    
    /*
     * Function name	: text::HowMuchOfText
     *
     * Return type		: LPSTR
     *
     * Arguments		: int number
     *                  : LPSTR text1
     *                  : LPSTR text2
     *
     * Description	    :
     *
     */
    
    LPSTR ddr_text::HowMuchOfText( u32 number, LPSTR text1, LPSTR text2 )
    {
        CStrBuf*	buffer = new CStrBuf ;
        
        if ( number>= NUMELE(adverb_token) )
            number=NUMELE(adverb_token)-1;
        if ( adverb_token[number] ) {
            buffer->strcpy ( adverb_token[ number ] );
            buffer->strcat ( (LPCSTR)" " );
        }
        buffer->strcat ( text1 );
        
        return buffer->GetAt();
    }

    LPSTR ddr_text::DescribeTime ( u32 time )
    {
        return DescribeNumber((time+3)/4);
    }
    
    //LPSTR ddr_text::DescribeEnergy ( u32 energy )
    //{
    //    LPSTR txtEnergy=HowMuchOfText ( energy/sv_energy_scale, energy_token[0], energy_token[1] );
    //    return txtEnergy;
    //}
    
    LPSTR ddr_text::DescribeCharacterTime( const mxcharacter* character )
    {
        CStrBuf*	buffer = new CStrBuf ;
        int time = character->Time();
        
        if ( time == sv_time_night )
            buffer->strcpy ( SystemString(SS_ITISNIGHT) );
        else if ( time == sv_time_dawn )
            buffer->strcpy ( SystemString(SS_ITISDAWN) );
        else {
            buffer->strcpy ( CookedSystemString(SS_HOURSOFDAY,character) );
        }
        return PurgeCache(buffer);
    }
    
    
    LPSTR ddr_text::DescribeNumberPart ( int number, ZERO_MODE zeromode )
    {
        CStrBuf*	buffer = new CStrBuf ;
        
        
        BOOL needscomma=true;
        BOOL needsand=false;
        
        int units = number%20;
        int hundreds = number/20;
        
        if ( units == 0 )
            needscomma=false;
        
        int scores = hundreds%5;
        hundreds/=5;

        if ( hundreds ) {
            needsand=true;
            buffer->strcat ( number_token[hundreds-1] );
            buffer->strcat ( (LPSTR)" " );
            buffer->strcat ( number_token[27] );
        }else
            needscomma=false;
        
        if ( scores ) {
            if ( needscomma ) {
                buffer->strcat ( (LPSTR)", " );
            } else {
                if ( needsand )
                    buffer->strcat ( (LPSTR)" and " );
            }
            buffer->strcat ( number_token[scores-1] );
            buffer->strcat ( (LPSTR)" " );
            buffer->strcat ( number_token[29] );
            needsand=true;
        }
    
        if ( units  ) {
            if ( needsand )
                buffer->strcat ( (LPSTR)" and " );
            buffer->strcat ( number_token[units-1] );
            needsand=true;
        }
        
        if ( needsand == 0 ) {
            // no
            buffer->strcat ( zero_token[zeromode] );
        }
        
        return buffer->GetAt() ;
    }

    

    /*
     * Function name	: text::DescribeStronghold
     *
     * Return type		: LPSTR
     *
     * Arguments		: const mxstronghold* stronghold
     *
     * Description	    :
     *
     */
    /*
     *  The fortress of Carorthand holds ten hundred, two score and ten fey riders.
     *  They are slightly tired.
     *  The fortress is owned by x who is loyal to they fey.
     */
    
    //73	SS_STRONGHOLD1	The %s is occupied by armies loyal to %s.§
    //74	SS_STRONGHOLD2	The %s is owned by %s who is loyal to.§
    //75	SS_STRONGHOLD3	%s holds %s %s %s.§
    
    LPSTR ddr_text::DescribeStronghold(const mxstronghold* stronghold)
    {
        CStrBuf*	buffer = new CStrBuf ;
        mxterrain*	tinfo;
        
        mxloc& mapsqr = mx->gamemap->GetAt ( stronghold->Location() );
        
        buffer->sprintf (
                         CookedSystemString(SS_STRONGHOLD3),
                         DescribeLocation(stronghold->Location()),
                         DescribeNumber( stronghold->Total() ),
                         (LPCSTR)mx->RaceById(stronghold->OccupyingRace())->SoldiersName(),
                         (LPCSTR)mx->UnitById(stronghold->Type())->Name(),
                         DescribeEnergy(stronghold->Energy())
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
            
            sprintf ( buffer->End(), CookedSystemString(message,character), (LPCSTR)tinfo->Name()  );
        }
		
        return PurgeCache(buffer);
    }
    
    /*
     * Function name	: text::DescribeCharacterBattle
     *
     * Return type		: LPSTR
     *
     * Arguments		: const mxcharacter* character
     *
     * Description	    :
     *
     */
    /* In battle[ against Carorthand the fey], tarithel the fey won [no|a] victory.
     * Tarithel slew [Carorthand they fey|no one].
     * Tarithel lost x [riders|warriors].
     * Luxor slew [Carorthand the fey|no one].
     * Taritel's [riders|warriors] slew x of the foe.
     */
    LPSTR ddr_text::DescribeCharacterBattle ( const mxcharacter* character )
    {
        CStrBuf*	buffer = new CStrBuf ;
        
        int temp=0;
        temp += character->warriors.Lost() ? 1 : 0 ;
        temp += character->riders.Lost() ? 2 : 0 ;
        
        //systemstrings[SS_BATTLE1]="In battle against %s, {char:name} ";
        //systemstrings[SS_BATTLE_NONE]="In battle, {char:name} ";

        buffer->strcpy("");
        
        // in battle
        ddr_character* ddr_char = (ddr_character*)character;
        if ( ddr_char->fighting_against != NULL ) {
            buffer->sprintf( CookedSystemString(SS_BATTLE1,character),  ddr_char->fighting_against->Longname().GetAt() );
        }else{
            buffer->strcpy ( CookedSystemString(SS_BATTLE_NONE,character) );
        }

        //systemstrings[SS_BATTLE_VICTORY_FREE]="won a victory. ";
        //systemstrings[SS_BATTLE_VICTORY_ENEMY]="was defeated. ";
        //systemstrings[SS_BATTLE_VICTORY_NONE]="won no victory. ";
        if ( !character->IsInBattle() || character->HasWonBattle() ) {
            if ( character->HasWonBattle() )
                buffer->strcat ( CookedSystemString(SS_BATTLE_VICTORY_FREE,character) );
            else
                buffer->strcat ( CookedSystemString(SS_BATTLE_VICTORY_ENEMY,character) );
        } else
            buffer->strcat ( CookedSystemString(SS_BATTLE_VICTORY_NONE,character) );

    

        switch ( temp ) {
            case 0:
                // rorthron the wise has fought no battle.
                //buffer->strcpy ( CookedSystemString(SS_BATTLE_NONE,character) );
                break;
            case 1:
                sprintf ( buffer->End(), CookedSystemString(SS_BATTLE_WARRIORS,character),
                         DescribeNumber(character->warriors.Lost()) );
                break;
            case 2:
                sprintf ( buffer->End(), CookedSystemString(SS_BATTLE_RIDERS,character),
                         DescribeNumber(character->riders.Lost()) );
                break;
            case 3:
                sprintf ( buffer->End(), CookedSystemString(SS_BATTLE_WARRIORS_RIDERS,character),
                         DescribeNumber(character->warriors.Lost(),ZERO_NO),
                         DescribeNumber(character->riders.Lost(),ZERO_NO) );
                break;
        }


        //systemstrings[SS_BATTLE_CHARACTER_SLEW] = "{char:name} slew %s. ";
        if ( ddr_char->Flags().Is(cf_killed_foe) && ddr_char->fighting_against!=NULL ) {
            sprintf ( buffer->End(),
                     CookedSystemString(SS_BATTLE_CHARACTER_SLEW,character),
                     ddr_char->fighting_against->Longname().GetAt() );
        } else {
            sprintf ( buffer->End(),
                     CookedSystemString(SS_BATTLE_CHARACTER_SLEW,character),
                     "no one" );
        }
 
        //systemstrings[SS_BATTLE_WARRIORS_SLEW]="{case:first}{char:gender:hisher} warriors slew %s of the foe. ";
        //systemstrings[SS_BATTLE_RIDERS_SLEW]="{case:first}{char:gender:hisher} riders slew %s of the foe. ";

        if ( character->warriors.Killed() )
            sprintf ( buffer->End(),
                     CookedSystemString(SS_BATTLE_WARRIORS_SLEW,character),
                     DescribeNumber(character->warriors.Killed(),ZERO_NONE) );
        
        if ( character->riders.Killed() )
            sprintf ( buffer->End(),
                     CookedSystemString(SS_BATTLE_RIDERS_SLEW,character), 		
                     DescribeNumber(character->riders.Killed(),ZERO_NONE) );
        return PurgeCache(buffer);
    }

    LPSTR ddr_text::DescribeCharacterDeath2 ( const mxcharacter* character )
    {
        const ddr_character* ddr_c = static_cast<const ddr_character*>(character);
        
        if ( character->killedbyobject )
            return CookedSystemString(SS_KILLED_OBJECT,character);
        
        if ( ddr_c->fighting_against )
            return CookedSystemString(SS_KILLED_BY,character);
        
        return CookedSystemString(SS_KILLED_IN_BATTLE,character);
        
    }
    
    
    LPSTR ddr_text::DescribeVictory ( u32 victory, s32 value )
    {
        CStrBuf*	buffer = new CStrBuf ;
        
        // describe victory
        LPSTR text = victory_token[value];
        text = mx->text->HowMuchOfText(victory, text, text);
        
        buffer->sprintf(mx->text->CookedSystemString(SS_VICTORY4),text);
        
        return PurgeCache(buffer);
    }
    
    LPSTR ddr_text::DescribeCharacterLocation( const mxcharacter* character )
    {
        CStrBuf*	buffer = new CStrBuf ;
        
        mxloc& here = mx->gamemap->GetAt ( character->Location() );
        mxgridref loc = mx->scenario->FindLookingTowards(character->Location(),character->Looking());
        mxloc& there = mx->gamemap->GetAt ( loc );
        
        if ( character->IsInTunnel() ) {
            buffer->strcpy ( CookedSystemString(SS_TUNNEL,character) );
        }else{
            
            if ( there.IsMisty() ) {
				buffer->strcpy ( CookedSystemString(SS_MESSAGE_MIST,character) );
            } else {
                if ( (here.area!=there.area) ||
                    (here.IsInDomain()!=there.IsInDomain()) ||
                    (here.terrain!=there.terrain) )
                {
                    buffer->strcpy ( CookedSystemString(SS_MESSAGE6,character) );
                    //buffer->strcpy(" ");
                }else{
                    buffer->strcpy ( CookedSystemString(SS_MESSAGE5,character) );
                    //buffer->strcpy(" ");
                }
            }
        }
        
        return PurgeCache(buffer);
    }
    
    void ddr_text::Serialize ( archive& ar )
    {
        mxtext::Serialize(ar);
        if ( ar.IsStoring() )
            return;
        
        //string test = SystemString(SS_MESSAGE1);
        //systemstrings[SS_MESSAGE1] = "{char:text:time}. {char:text:sees}Here {char:loc:text}, {char:longname} has found nothing. {char:text:obj}. {char:text:group} {char:text:inbattle}";
        
        //  systemstrings[SS_TUNNEL]="{char:longname} stands in a tunnel looking {char:dir:name}. {char:text:time}.";
        //  systemstrings[SS_SEEK] = "{char:text:time}. {char:text:sees}Here {char:loc:text}, {char:longname} has found {obj:text}. {char:text:obj}. {char:text:group}";
        //  systemstrings[SS_FIGHT] = "{char:text:time}. {char:text:sees}Here {char:loc:text}, {char:longname} has found {obj:text} and slain them. {char:text:obj}. {char:text:group}";
        
        //systemstrings[SS_SEEK_MSG1]=", \"{char:longname} stands {char:loc:terrain:prep} {char:loc:name}.\" ";
        systemstrings[SS_SEEK_MSG3]=", \"The %s, can be found in the hands of {char:longname} {char:loc:text}.\" ";
        //systemstrings[SS_SEEK_MSG2]=", \"The %s, can be found %s.\" ";
        
        //systemstrings[SS_CARRYING]="{char:name} carries the {char:obj:fullname}";
        //  systemstrings[SS_ARMY_WARRIORS]="{str:48} %s {char:race:soldiers} warriors. {str:49}";
        //  systemstrings[SS_ARMY_RIDERS]="{str:48} %s {char:race:soldiers} riders. {str:50}";
        //  systemstrings[SS_ARMY_CHARACTER_WARRIORS]="They are %s. ";
        //  systemstrings[SS_ARMY_CHARACTER_RIDERS]="They are %s. ";
        //  systemstrings[SS_GUIDANCE1] = "{char:text:time}. {char:text:sees}Here {char:loc:text}, {char:longname} has found {obj:text}";
        //  systemstrings[SS_GUIDANCE2] = " {char:text:obj}. {char:text:group}";
        //  systemstrings[SS_BATTLE_VICTORY_NONE] = "{char:longname} prepares to do battle... ";
        //  systemstrings[SS_DEFEAT1] = " The evil Shareth has won victory!";
        //  systemstrings[SS_DEFEAT2] = " {char:longname} is dead, {char:text:death2}\n{str:58}";
        //  systemstrings[SS_KILLED_OBJECT] = " slain by {case:lower}{char:battle:obj:name}. ";
        //  systemstrings[SS_KILLED_BY] = " killed in battle by {char:battle:fighting:longname}. ";
        //  systemstrings[SS_KILLED_IN_BATTLE] = " killed in battle. ";
        
        //<string Id="27" Code="SS_MESSAGE2">{char:loc:terrain:prep} {char:loc:name} stands {char:longname}.§{char:text:army}</string>

        //systemstrings[SS_MESSAGE2]="{char:loc:text} stands {char:longname}. {char:text:army}";
        
        //systemstrings[SS_CHARACTER_DEAD] = "\n  {char:longname} is dead, {char:text:death2}";
        //systemstrings[SS_KILLED_OBJECT] = "{char:longname} is dead, slain by {case:lower}{char:battle:obj:name}. ";

        
        // SS_VICTORY2
        //systemstrings[SS_VICTORY2] = "After {special:days} {plural:day:days} the warriors of Midnight return to the frozen gates. ";
        //systemstrings[SS_CHARACTER_HERE] = "{char:name} is here. ";
        //systemstrings[SS_SHARETH_DEAD] = "{char:name} is dead. ";
        //systemstrings[SS_TOKENS_VICTORY] = "none|worthy|noble|overwhelming";
        //systemstrings[SS_VICTORY4] = "\n\nThe Victory of the Moonprince is %s. ";
        //systemstrings[SS_VICTORY5] = "\n\nThe Watchwords of Midnight are revealed!\n\n'The brave and the free fear no foe.\nForth, mighty hosts of Midnight!\nUnto death or victory we go!'";
        
        FillArrayFromSystemString( victory_token, SS_TOKENS_VICTORY );

        //systemstrings[SS_BATTLE_CHARACTER_SLEW] = "{char:name} slew %s. ";
        //systemstrings[SS_BATTLE_WARRIORS_SLEW]="{case:first}{char:gender:hisher} warriors slew %s of the foe. ";
        //systemstrings[SS_BATTLE_RIDERS_SLEW]="{case:first}{char:gender:hisher} riders slew %s of the foe. ";
        //systemstrings[SS_BATTLE1]="In battle against %s, {char:name} ";
        //systemstrings[SS_BATTLE_NONE]="In battle, {char:name} ";
        //systemstrings[SS_BATTLE_VICTORY_FREE]="won a victory. ";
        //systemstrings[SS_BATTLE_VICTORY_ENEMY]="was defeated. ";
        //systemstrings[SS_BATTLE_VICTORY_NONE]="won no victory. ";

        //systemstrings[SS_BATTLE_WARRIORS]="{char:name} lost %s warriors. ";
        //systemstrings[SS_BATTLE_RIDERS]="{char:name} lost %s riders. ";
        //systemstrings[SS_BATTLE_WARRIORS_RIDERS]="{char:name} lost %s warriors and %s riders. ";

        // luxor the moonprince places the crown of carudrium on his head, the power contained within brings all lords loyal to him.
        // Tarithel the fey casts the Spell of Thigor, She is magically transported to be with Morkin.
        // rorthron throws the runes of finorn, all lords loyal to the moon prince are refreshed and envigorated
        // rorthron uses the %s, he now feels refreshed and envigorated.

        //systemstrings[SS_OBJECT_USE_1]="{char:longname} places the {obj:text} on {char:gender:hisher} head, the power contained within brings all lords loyal to the Moonprince to {char:gender:himher}.";
        //systemstrings[SS_OBJECT_USE_2]="{char:longname} casts the {obj:text} and {char:gender:heshe} is magically transported to be with Morkin.";
        //systemstrings[SS_OBJECT_USE_3]="{char:longname} throws the {obj:text}, all lords loyal to the Moonprince are refreshed and envigorated";
        //systemstrings[SS_OBJECT_USE_4]="{char:longname} uses the {obj:text} and {char:gender:heshe} now feels refreshed and envigorated.";

        //systemstrings[SS_MESSAGE_MIST]="  {char:longname} stands {char:loc:terrain:prep} {char:loc:name} looking {char:dir:name} into the mist. ";
    
        
        //mxobject* guidance = mx->ObjectById(OB_GUIDANCE);
        //string text = guidance->description;
        //systemstrings[SS_STRONGHOLD1]="The %s is owned by {char:longname}.";
        //string text = SystemString(SS_STRONGHOLD1);
        //text = SystemString(SS_STRONGHOLD2);
        //text = SystemString(SS_STRONGHOLD3);
        
    }
    

    
    
} // namespace:tme

#endif // _DDR_
