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
    
    void ddr_battle::Process( void )
    {
        // fights
        FOR_EACH_CHARACTER(character) {
            Battle(static_cast<ddr_character*>(character));
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
    
    void ddr_battle::Battle ( ddr_character* character )
    {
        if ( character->IsInBattle() )
            return;
        
        if ( character->IsInTunnel())
            return;
        
        MXTRACE("BATTLE: [%3d] %-16s %s", (int)character->Id(), character->Symbol().c_str(), character->Longname().c_str() );
      
        // current garrison
        auto scenario = static_cast<ddr_x*>(mx->scenario);
        auto stronghold =  static_cast<ddr_stronghold*>(scenario->StrongholdFromLocation(character->Location()));
        
        // how many enemies
        u32 enemies=0;
        
        if ( stronghold ) {
            if ( !stronghold->IsFriend(character) ) {
                enemies++;
            }
        }
        
        characters_here.Clear();
        enemies += GetCharactersHereForBattle(characters_here, character);
        
        // anything to fight?
        if ( enemies == 0 ) {
            MXTRACE("  No enemies!" );
            return;
        }
        
        //auto location = character->Location();
        
        // DEBUG output
        for ( u32 ii=0; ii<characters_here.Count(); ii++ ) {
            auto attacker = static_cast<mxcharacter*>(characters_here[ii]);
            MXTRACE("  [%d:%d] %-16s %s", (int)ii, (int)attacker->Id(), attacker->Symbol().c_str(), attacker->Longname().c_str());
        }
        // END DEBUG output
        
        // give all the lords first hit
        for ( auto attacker : characters_here ) {
            MXTRACE("  skirmish: %-16s", attacker->Symbol().c_str() );
            CharacterAttacks(static_cast<ddr_character*>(attacker));
        }
        
        // Then the strong hold armies
        if ( stronghold ) {
            MXTRACE("  stronghold=%-16s", stronghold->Symbol().c_str());
            StrongholdAttacks(stronghold);
        }
        
        //Announce(location);
    }
    
    s32 ddr_battle::GetCharactersHereForBattle(c_character& characters, mxcharacter* character)
    {
        s32 enemies = 0;
        FOR_EACH_CHARACTER(c)
        {
            CONTINUE_IF(c->IsDead());

            CONTINUE_IF(c->IsInTunnel());

            CONTINUE_IF(c->Location() != character->Location());
            
            CONTINUE_IF(c->IsBattleOver());
            
            characters.Add(c);
            if ( !character->IsFriend(c) )
                enemies++;
        }
        
        return enemies;
    }
    
    s32 ddr_battle::CalcCharacterAttackerHP(const ddr_character* attacker)
    {
        if ( attacker->HasBattleObject() ) {
            return 255;
        }
        
        return (((attacker->energy/2)+(attacker->despondency/2))/2)+attacker->reckless/2;
    }
    
    void ddr_battle::CharacterAttacks( ddr_character* attacker )
    {
        doBattle( attacker, CalcCharacterAttackerHP(attacker) );
    }

    void ddr_battle::StrongholdAttacks( mxstronghold* attacker )
    {
        doBattle( attacker, 0 );
    }

    // When a lord fights another lord, the attacking lord has his success calculated
    // as energy/2 + despondency/2 + reckless/2 OR 255 if carrying a battle object
    // BUT the defender success chance is purely a random number between 0 and 255
    //
    
    void ddr_battle::doBattle ( mxitem* attacker, s32 attackers_success)
    {
        ddr_character* attacker_character=nullptr;
        ddr_stronghold* attacker_stronghold=nullptr;
         
        mxrace_t loyalty = RA_NONE;
        
        if ( attacker->IsType(IDT_CHARACTER) ) {
            attacker_character = static_cast<ddr_character*>(attacker);
            loyalty = attacker_character->NormalisedLoyalty();
        }
        else if ( attacker->IsType(IDT_STRONGHOLD) ) {
            attacker_stronghold= static_cast<ddr_stronghold*>(attacker);
            loyalty = attacker_stronghold->Loyalty();
        } else
            return;
        
        // find someone to fight
        auto defender = SelectDefender(characters_here, loyalty);
        
        RETURN_IF_NULL(defender);
        
        PutCharacterInBattle(defender);
        
        MXTRACE("    Attacker = %-16s, Defender = %-16s", attacker->Symbol().c_str(), defender->Symbol().c_str());
        
        if ( attacker_character ) {
            CharacterVsCharacter(attacker_character, attackers_success, defender);
        }
        
        battleArmies( attacker, defender );
        battleArmies( defender, attacker );
    }

    void updateFightingAgainst( ddr_character* character, ddr_character* against) {
        // don't override any previous battle if the character
        // has already killed foe
        if ( !character->Flags().Is(cf_killed_foe))
            character->fighting_against = against;
    }


    void ddr_battle::PutCharacterInBattle( ddr_character* character)
    {
        character->Flags().Set(cf_inbattle);
        character->battleloc = character->Location();
    }
    
    
    void ddr_battle::CharacterVsCharacter( ddr_character* attacker, s32 attackers_hp, ddr_character* defender)
    {
        PutCharacterInBattle(attacker);
        updateFightingAgainst(attacker, defender);
        updateFightingAgainst(defender, attacker);

        s32 defenders_hp = CalcCharacterDefenderHP(defender);
        
        MXTRACE("    %-16s (%d) vs %-16s (%d)", attacker->Symbol().c_str(), (int)attackers_hp
                                                , defender->Symbol().c_str(), (int)defenders_hp);
        
        if ( attackers_hp > defenders_hp  ) {
            MXTRACE("    Defender: %-16s lost fight", defender->Symbol().c_str() );
            DefenderLoses(defender, defenders_hp, attacker);
        }
    }
    
    s32 ddr_battle::CalcCharacterDefenderHP( const ddr_character* defender )
    {
        return mxrandom(255);
    }
    
    void ddr_battle::DefenderLoses(ddr_character* defender, s32 defender_hp, ddr_character* attacker )
    {
        defender->LostFight( defender_hp );
        if ( defender->IsDead() ) {
            MXTRACE("    Defender: %-16s is dead killed by %s", defender->Symbol().c_str(), attacker->Symbol().c_str() );
            CharacterKilledByCharacter(defender, attacker);
        }
    }

    void ddr_battle::CharacterKilledByCharacter(ddr_character* defender, ddr_character* attacker )
    {
        defender->LostBattle(false);
        defender->fighting_against = attacker;
        defender->killedby = KB_LORD;

        if (attacker->HasBattleObject()) {
            defender->killedbyobject = attacker->Carrying();
            defender->killedby = KB_LORD_OBJECT;
        }
        
        attacker->Flags().Set(cf_wonbattle|cf_killed_foe);
        attacker->fighting_against=defender;
    }

    ddr_character* ddr_battle::SelectDefender(c_character& characters, mxrace_t loyalty)
    {
        u32 rnd = mxrandom(255);
        u32 start = rnd % characters.Count();
        u32 end = start;
        
        MXTRACE("    Start=%d Rnd=%d, Count=%d", (int)start, (int)rnd, (int)characters.Count());
       
        auto defender = static_cast<ddr_character*>(characters[start]);
        while ( defender->IsBattleOver() || defender->IsDead() || defender->NormalisedLoyalty() == loyalty ) {
            
            start++;
            if ( start == characters.Count() )
                start=0;
            if ( start == end )
                return nullptr;
            
            defender = static_cast<ddr_character*>(characters[start]);
        }
        
        return defender;
    }
    
    s32 ddr_battle::getArmySize( mxitem* item )
    {
        if ( item->IsType(IDT_STRONGHOLD) ) {
            return static_cast<mxstronghold*>(item)->TotalTroops();
        } else if ( item->IsType(IDT_CHARACTER) ) {
            return static_cast<ddr_character*>(item)->getArmySize();
        }
        return 0;
    }
    
    void ddr_battle::setArmySize(mxitem* item, s32 size)
    {
        if ( item->IsType(IDT_STRONGHOLD) ) {
            static_cast<mxstronghold*>(item)->TotalTroops(size);
        } else if ( item->IsType(IDT_CHARACTER) ) {
            static_cast<ddr_character*>(item)->setArmySize(size);
        }
    }
    
    void ddr_battle::AttackerArmiesWinBattle(tme::mxitem *attacker)
    {
        if ( attacker->IsType(IDT_CHARACTER) ) {
            attacker->Flags().Set(cf_wonbattle);
            MXTRACE("      Attacker: %-16s won battle", attacker->Symbol().c_str() );
        }
    }
    
    void ddr_battle::DefenderArmiesLosesBattle(tme::mxitem *attacker, tme::mxitem *defender)
    {
        if ( defender->IsType(IDT_CHARACTER) ) {
            CharacterLosesArmy(static_cast<ddr_character*>(defender));
        }
                
        if ( defender->IsType(IDT_STRONGHOLD) ) {
            StrongholdLosesArmy(
                static_cast<mxstronghold*>(defender),
                static_cast<mxcharacter*>(attacker) // attacker must be a character
                );
        }
    }

    void ddr_battle::StrongholdLosesArmy(mxstronghold* stronghold, mxcharacter* attacker)
    {
        // change sides
        stronghold->MakeChangeSides(attacker->Race(), attacker);
        MXTRACE("      Stronghold Changes Sides: %-16s",stronghold->Symbol().c_str() );
    }

    void ddr_battle::CharacterLosesArmy(ddr_character* defender)
    {
        MXTRACE("      Defender: %-16s lost battle", defender->Symbol().c_str() );
            
        defender->LostBattle(true);
        defender->Flags().Reset(cf_wonbattle);
        defender->DecreaseDespondency(32);
        defender->LostFight(0);
        if ( defender->IsDead() ) {
            MXTRACE("      Defender: %-16s is dead, killed by soldiers.", defender->Symbol().c_str() );
            defender->killedby = KB_ARMY;
        }
    }

    s32 ddr_battle::CalcArmyAttackerHP( mxitem* attacker )
    {
        s32 attackerStrength = getArmyStrength(attacker);
        s32 attackerSize = getArmySize(attacker);
        
        attackerStrength *= (attackerSize/5);
        attackerStrength *= (mxrandom(255)&31)+16;
        
        return attackerStrength;
    }
    
    s32 ddr_battle::CalcArmyDefenderHP( mxitem* defender )
    {
        return getArmyStrength(defender);
    }

    s32 ddr_battle::CalcDefenderLoses( mxitem* attacker, mxitem* defender )
    {
        s32 attackerHP = CalcArmyAttackerHP(attacker);
        s32 defenderHP = CalcArmyDefenderHP(defender);
        
        s32 defenderSize = getArmySize(defender);
        
        s32 killed = (attackerHP / defenderHP);
        killed = ((killed/256) & 255)*5 ;
        killed = std::min<int>(killed,defenderSize);
        
        return killed;
    }
    
    void ddr_battle::battleArmies ( mxitem* attacker, mxitem* defender )
    {
        s32 loses = CalcDefenderLoses(attacker, defender);
        s32 defenderSize = getArmySize(defender);

        MXTRACE("      Defender: %-16s lost %d of (%d)", defender->Symbol().c_str(), (int)loses, defenderSize );
        
        defenderSize = BSub(defenderSize, loses, 0);
        setArmySize( defender, defenderSize );
        
        if( defenderSize == 0 ) {
            AttackerArmiesWinBattle(attacker);
            DefenderArmiesLosesBattle(attacker, defender);
        }
                
        //MXTRACE("Defender: %s army = (%d)", (LPSTR)defender->Symbol(), (int)getArmySize(defender) );

        UpdateDefenderLoses(defender, loses);
        UpdateAttackerKills(attacker, loses);
    }
    
    void ddr_battle::UpdateDefenderLoses(mxitem* defender, s32 loses)
    {
        if ( defender->IsType(IDT_CHARACTER) ) {
            // character->lost = killed;
            auto defender_character = static_cast<ddr_character*>(defender);
            defender_character->battlelost=loses;
            defender_character->setArmyLost(loses);
            defender_character->Flags().Set(cf_inbattle);
            defender_character->battleloc = defender_character->Location();

        } else if ( defender->IsType(IDT_STRONGHOLD) ) {
            auto defender_stronghold = static_cast<mxstronghold*>(defender);
            defender_stronghold->Lost(loses);
        }
    }
    
    void ddr_battle::UpdateAttackerKills(mxitem *attacker, s32 kills)
    {
        if ( attacker->IsType(IDT_CHARACTER) ) {
            auto attacker_character = static_cast<ddr_character*>(attacker);
            attacker_character->battleslew = kills;
            attacker_character->setArmyKilled(kills);
        } else if ( attacker->IsType(IDT_STRONGHOLD) ) {
            auto attacker_stronghold = static_cast<mxstronghold*>(attacker);
            attacker_stronghold->Killed(kills);
        }
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
        s32 index = 0;
        
        if ( army->IsType(IDT_STRONGHOLD) ) {
            ddr_stronghold* army_stronghold = static_cast<ddr_stronghold*>(army);
            index = army_stronghold->Race()*2;
            if ( army_stronghold->Type() == UT_WARRIORS )
                index++;
            energy = army_stronghold->Energy();
            strength = ((strengths[index]/2) + (energy/2)) /2 ;
        }else if ( army->IsType(IDT_CHARACTER) ) {
            ddr_character* army_character = static_cast<ddr_character*>(army);
            index = army_character->Race()*2;
            if ( army_character->IsAllowedWarriors() )
                index++;
            energy = army_character->energy;
            strength = ((strengths[index]/2) + (energy/2)) ;
        }
    
        return strength;
        
    }
}
// namespace tme

#endif // _DDR_
