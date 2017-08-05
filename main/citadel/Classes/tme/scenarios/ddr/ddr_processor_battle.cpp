/*
 * FILE:    ddr_processor_battle.cpp
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


#include "../../baseinc/tme_internal.h"
#include "../../baseinc/variables.h"

#include "ddr_processor_battle.h"
#include "scenario_ddr_internal.h"

#if defined(_DDR_)

namespace tme {
    
    ddr_battle::ddr_battle()
    {
    }

    ddr_battle::~ddr_battle()
    {
    }

    
    void ddr_battle::Test(void)
    {
        mxcharacter* ch_torinarg = (mxcharacter*)mx->EntityByName("CH_TORINARG");
        mxcharacter* ch_luxor = (mxcharacter*)mx->EntityByName("CH_LUXOR");
        
        ch_luxor->Location( ch_torinarg->Location() );
        
        mxcharacter* ch = NULL;

        ch = (mxcharacter*)mx->EntityByName("CH_RORTHRON");
        ch->Location(ch_luxor->Location());

        
        ch = (mxcharacter*)mx->EntityByName("CH_THORTHAND");
        ch->Location(ch_luxor->Location());
        ch->Recruited(ch_luxor);
        
        ch = (mxcharacter*)mx->EntityByName("CH_LORANGRIEL");
        ch->Location(ch_luxor->Location());
        ch->Recruited(ch_luxor);
        
        
        ch = (mxcharacter*)mx->EntityByName("CH_IMGORARG");
        ch->Location(ch_luxor->Location());
        ch = (mxcharacter*)mx->EntityByName("CH_GLORMARG");
        ch->Location(ch_luxor->Location());

        Battle(ch_luxor);
        
        ch_luxor->Flags().Reset(cf_inbattle);
        Battle(ch_luxor);
        
        
    }
    
    
    void ddr_battle::War( void )
    {
        // fights
        for ( int ii=0; ii<sv_characters; ii++ ) {
            ddr_character* character = static_cast<ddr_character*>(mx->CharacterById(ii+1));
            Battle(character);
        }
    }
    
    /*
     f1
     e1
     e2
     e3
     f2
     
     f1 vs e1
     e1 vs f2
     e2 vs f2
     e3 vs f1
     f2 vs e1
     
     
     */
    
    
    void ddr_battle::Battle ( mxcharacter* character )
    {
        
        if ( character->IsInBattle() )
            return;
        
        if ( character->IsInTunnel())
            return;
        
        MXTRACE("BATTLE: [%3d] %-16s", (int)character->Id(), character->Symbol().GetAt() );

        //mxlocinfo* info = character->GetLocInfo();
        
        // current garrison
        mxstronghold* stronghold =  mx->scenario->StrongholdFromLocation(character->Location());
        
        // how many enemies
        u32 enemies=0;
        
        if ( stronghold ) {
            if ( stronghold->Loyalty() != character->Loyalty() ) {
                enemies++;
            }
        }
        
        characters_here.Clear();
        
        for ( u32 ii=0; ii<sv_characters; ii++ ) {
            mxcharacter* c = mx->CharacterById(ii+1);
            
            if ( c->IsInTunnel() )
                continue;
            
            if ( (c->Location().x != character->Location().x) || (c->Location().y != character->Location().y) )
                continue;
            
            characters_here.Add(c);
            if ( c->Loyalty() != character->Loyalty() )
                enemies++;
        }
        
        // anything to fight?
        if ( enemies==0 ) {
            return;
        }
        
        for ( u32 ii=0; ii<characters_here.Count(); ii++ )
        {
            mxcharacter* attacker = static_cast<mxcharacter*>(characters_here[ii]);
            MXTRACE("  [%d:%d] %-16s", (int)ii, (int)attacker->Id(), attacker->Symbol().GetAt());
        }
        
        // give all the bad guys first hit
        for ( u32 ii=0; ii<characters_here.Count(); ii++ )
        {
            mxcharacter* attacker = static_cast<mxcharacter*>(characters_here[ii]);
            
            // TODO: is character still alive?
            // still at this location?
            
            //attacker->Flags().Set(cf_inbattle);
            //attacker->battleloc = attacker->Location();
            
            MXTRACE("  skirmish: %-16s", attacker->Symbol().GetAt() );
            BattleVsCharacter(attacker);
        }
        
        
        if ( stronghold ) {
            MXTRACE("Stronghold=%-16s", stronghold->Symbol().GetAt());
            doBattle(stronghold,0);
        }
    }
    
    void ddr_battle::BattleVsCharacter( mxcharacter* attacker )
    {
         s32 success=0;
         
         // check power in battle
         if ( attacker->Carrying()!= NULL && attacker->Carrying()->power == OP_BATTLE )
         {
             success=255;
         }
         else
         {
             success= (((attacker->energy/2)+(attacker->despondency/2))/2)+attacker->reckless/2;
         }
         
        doBattle( attacker, success );
        
    }
    
    // When a lord fights another lord, the attacking lord has his success calculated
    // as energy/2 + despondency/2 + reckless/2 OR 255 if carrying a battle object
    // BUT the defender success chance is purely a random number between 0 and 255
    //
    
    
    
    
    void ddr_battle::doBattle ( mxitem* attacker, s32 attackers_success)
    {
        ddr_character* attacker_character=NULL;
        mxstronghold* attacker_stronghold=NULL;
        
        u32 rnd = random(255);
        u32 start = rnd % characters_here.Count();
        u32 end = start;
        
        MXTRACE("    Start=%d Rnd=%d, Count=%d", (int)start, (int)rnd, (int)characters_here.Count());
        
        mxrace_t loyalty=RA_NONE;
        
        if ( attacker->Type() == IDT_CHARACTER ) {
            attacker_character = static_cast<ddr_character*>(attacker);
            loyalty = attacker_character->Loyalty();
        }
        else if ( attacker->Type() == IDT_STRONGHOLD ) {
            attacker_stronghold= static_cast<mxstronghold*>(attacker);
            loyalty = attacker_stronghold->Loyalty();
        } else
            return;
        
        // find someone to fight
        ddr_character* defender = static_cast<ddr_character*>(characters_here[start]);
        while ( defender->IsDead() || defender->Loyalty() == loyalty ) {
            
            // TODO: Defender may have been displaced
            
            start++;
            if ( start == characters_here.Count() )
                start=0;
            if ( start == end )
                return;
            
            defender = static_cast<ddr_character*>(characters_here[start]);
            
        }
        
        defender->Flags().Set(cf_inbattle);
        defender->battleloc = defender->Location();
        
        MXTRACE("    Attacker = %-16s, Defender = %-16s", attacker->Symbol().GetAt(), defender->Symbol().GetAt());
        
        if ( attacker_character ) {
            
            attacker_character->Flags().Set(cf_inbattle);
            attacker_character->battleloc = attacker->Location();

            
            if ( !attacker_character->Flags().Is(cf_killed_foe))
                attacker_character->fighting_against = defender ;
                    
            s32 defenders_success = random(255);
            
            
            MXTRACE("    %-16s (%d) vs %-16s (%d)", attacker->Symbol().GetAt(), (int)attackers_success
                                                    , defender->Symbol().GetAt(), (int)defenders_success);
            
            if ( attackers_success > defenders_success  ) {
                MXTRACE("    Defender: %-16s lost fight", (LPSTR)defender->Symbol() );
                
                loseFight( defender, defenders_success );
                if ( defender->IsDead() ) {
                    attacker_character->Flags().Set(cf_wonbattle|cf_killed_foe);
                    attacker_character->fighting_against=defender;
                    MXTRACE("    Defender: %-16s is dead killed by %s", (LPSTR)defender->Symbol(), (LPSTR)attacker->Symbol() );
                }
            }
        
        }
        
        battleArmies( attacker, defender );
        battleArmies( defender, attacker );
    }
    
    s32 ddr_battle::getArmySize( mxitem* item )
    {
        if ( item->Type() == IDT_STRONGHOLD ) {
            return static_cast<mxstronghold*>(item)->Total();
        } else if ( item->Type() == IDT_CHARACTER ) {
            return static_cast<ddr_character*>(item)->getArmySize();
        }
        return 0;
    }
    
    void ddr_battle::setArmySize(mxitem* item, s32 size)
    {
        if ( item->Type() == IDT_STRONGHOLD ) {
            static_cast<mxstronghold*>(item)->Total(size);
        } else if ( item->Type() == IDT_CHARACTER ) {
            static_cast<ddr_character*>(item)->setArmySize(size);
        }
    }
    
    void ddr_battle::battleArmies ( mxitem* attacker, mxitem* defender )
    {
        s32 attackerStrength = getArmyStrength(attacker);
        s32 attackerSize = getArmySize(attacker);
        
        attackerStrength *= (attackerSize/5);
        attackerStrength *= (random(255)&31)+16;
        
        s32 defenderStrength = getArmyStrength(defender);
        s32 defenderSize = getArmySize(defender);
        
        s32 killed = (attackerStrength / defenderStrength);
        killed = ((killed/256) & 255)*5 ;
        
        killed = MIN(killed,defenderSize);

        MXTRACE("      Defender: %-16s lost %d of (%d)", (LPSTR)defender->Symbol(), (int)killed, defenderSize );
        
        defenderSize = BSub(defenderSize, killed, 0);
        setArmySize( defender, defenderSize );

        if ( defenderSize == 0 ) {
            // defender lost army
            
            if ( defender->Type() == IDT_CHARACTER ) {
                
                ddr_character* defender_character = static_cast<ddr_character*>(defender);
                
                // defender flees
                mxloc mapsqr;
                mxgridref loc;
                do {
                    loc = defender_character->Location();
                    loc.x ^= (random(255)&3);
                    loc.y ^= (random(255)&3);
                    mapsqr = mx->gamemap->GetAt(loc);
                } while ( mapsqr.terrain==TN_FROZENWASTE || mapsqr.terrain==TN_ICYWASTE );
                defender_character->Location(loc);
                
                MXTRACE("      Defender: %-16s is displaced", (LPSTR)defender->Symbol() );

                
                // if we are a member of the group, then we need to leave the group
                if ( defender_character->IsFollowing() )
                    defender_character->Cmd_UnFollow(defender_character->Following());
                
                // if we are leading a group, then we must disband the group
                if ( defender_character->HasFollowers() )
                    defender_character->Cmd_DisbandGroup();
                
                defender_character->Flags().Reset(cf_wonbattle|cf_tunnel|cf_preparesbattle);
                defender_character->DecreaseDespondency(32);
                
                loseFight(defender_character, 0);
                if ( defender_character->IsDead() ) {
                    MXTRACE("      Defender: %-16s is dead, killed by soldiers.", (LPSTR)defender->Symbol() );
                    // FIX: 30/9/2014 - don't clear the fighting_against flag if the lord dying killed their foe
                    if ( !defender_character->Flags().Is(cf_killed_foe))
                        defender_character->fighting_against=NULL;
                }
                
            }
            
            if ( attacker->Type() == IDT_CHARACTER ) {
                attacker->Flags().Set(cf_wonbattle);
                MXTRACE("      Attacker: %-16s won battle", (LPSTR)attacker->Symbol() );
            }
            
            if ( defender->Type() == IDT_STRONGHOLD ) {
                //
                ddr_character* attacker_character = static_cast<ddr_character*>(attacker);
                
                // change sides
                mxstronghold* defender_stronghold = static_cast<mxstronghold*>(defender);
                defender_stronghold->MakeChangeSides(attacker_character->Race(), attacker_character);
                MXTRACE("      Stronghold Changes Sides: %-16s", (LPSTR)defender->Symbol() );

            }
        }
        
        
        //MXTRACE("Defender: %s army = (%d)", (LPSTR)defender->Symbol(), (int)getArmySize(defender) );

        
        if ( defender->Type() == IDT_CHARACTER ) {
            // character->lost = killed;
            ddr_character* defender_character = static_cast<ddr_character*>(defender);
            defender_character->battlelost=killed;
            defender_character->setArmyLost(killed);
            defender_character->Flags().Set(cf_inbattle);
            defender_character->battleloc = defender_character->Location();

            
        } else if ( defender->Type() == IDT_STRONGHOLD ) {
            mxstronghold* defender_stronghold = static_cast<mxstronghold*>(defender);
            defender_stronghold->Lost(killed);
        }
        
        if ( attacker->Type() == IDT_CHARACTER ) {
            ddr_character* attacker_character = static_cast<ddr_character*>(attacker);
            attacker_character->battleslew = killed;
            attacker_character->setArmyKilled(killed);
        } else if ( attacker->Type() == IDT_STRONGHOLD ) {
            mxstronghold* attacker_stronghold = static_cast<mxstronghold*>(attacker);
            attacker_stronghold->Killed(killed);
        }
        
    }
    
    void ddr_battle::loseFight ( ddr_character* character, s32 success )
    {
        
        if ( character->Carrying() ) {
            if ( character->Carrying()->power == OP_PROTECTION ) {
                return;
            }
        }
        
        // lose horse
        if ( (success&3) == 1 ) {
            character->Flags().Reset(cf_riding);
        }
        
        u32 r = random(255);
        if ( (r&1) == 0 )
            return;
        
        if ( character->reckless > r )
            return;
        
        if (character->Carrying() && character->Carrying()->power == OP_BATTLE ) {
            if ( random(255) >= 80 )
                return;
        }
        
        character->Cmd_Dead();
        
        
    }

    //RACE_MOONPRINCE         EQU 00h
    //RACE_FREE               EQU 02h
    //RACE_WISE               EQU 04h
    //RACE_FEY                EQU 06h
    //RACE_BARBARIAN          EQU 08h
    //RACE_ICELORD            EQU 0Ah
    //RACE_FEY2               EQU 0Ch
    //RACE_GIANT              EQU 0Dh
    //RACE_HEARTSTEALER       EQU 0Eh
    //RACE_DWARF              EQU 0Fh
    
    s32 ddr_battle::getArmyStrength( mxitem* army )
    {
        static u8 strengths[] = {
                0 // 0 NONE
            ,   0
            ,   0 // 2 DOOMGUARD
            ,   0 //
            , 220 // 4 FREE
            , 180 //
            , 200 // 6 FEY
            , 150 //
            ,   0 // 8 BARBARIAN
            ,   0
            , 220 // 10 WISE
            , 180 //
            ,   0 // 12 MORKIN
            ,   0
            ,   0 // 14 SKULKRIN
            ,   0 //
            ,   0 // 16 DRAGON
            ,   0 //
            ,   0 // 18 TROLL
            ,   0 //
            
            , 220 // 20 MOONPRINCE
            , 180 //
            , 180 // 22 BARBARIAN
            , 140 //
            , 220 // 24 ICELORD
            , 180 //
            , 220 // 26 TARITHEL
            , 220 //
            , 180 // 28 GIANT
            , 180 //
            , 220 // 30 HEARTSTEALER
            , 220 //
            , 160 // 32 DWARF
            , 160 //
            ,   0 // 34 MIDWINTER
            ,   0 //
        };
        
        s32 strength =0;
        s32 energy=0;
        s32 race = 0;
        
        if ( army->Type() == IDT_STRONGHOLD ) {
            mxstronghold* army_stronghold = static_cast<mxstronghold*>(army);
            race = army_stronghold->Race()*2;
            if ( army_stronghold->Type() == UT_WARRIORS )
                race++;
            energy = army_stronghold->Energy();
            strength = ((strengths[race]/2) + (energy/2)) /2 ;
        }else if ( army->Type() == IDT_CHARACTER ) {
            ddr_character* army_character = static_cast<ddr_character*>(army);
            race = army_character->Race()*2;
            if ( army_character->IsAllowedWarriors() )
                race++;
            energy = army_character->energy;
            strength = ((strengths[race]/2) + (energy/2)) ;
        }
    
        return strength;
        
    }
    
}
// namespace tme

#endif // _DDR_
