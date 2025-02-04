/*
 * FILE:    lom_processor_battle.cpp
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
 */
#include "../../baseinc/tme_internal.h"
#include "../../baseinc/variables.h"

#include "lom_processor_battle.h"

#if defined(_LOM_)

#include <algorithm>
#include <string>

namespace tme {

void lom_battle::ProcessLocation ( mxgridref loc )
{
    location = loc ;
    info.reset(new mxlocinfo ( loc, nullptr, slf_none ));

    if ( info->foe.armies ) {

        ProcessAllCharacters();

        ProcessAllArmies();

        CheckVictors();
    }
    
    info.reset();
}

bool lom_battle::MakeFriendOrFoeList ( const mxcharacter* character )
{
    foes.clear();
    friends.clear();

    for ( auto army : info->armies ) {
        if ( army->total ) {
            if ( army->race == RA_DOOMGUARD )
                foes.push_back(army);
            else
                friends.push_back(army);
        }
    }
    
    return !friends.empty() && !foes.empty();
}

void lom_battle::ProcessAllArmies ()
{
    if (!MakeFriendOrFoeList())
        return;

    for ( auto army : friends ) {
        if ( army->total )
            army->killed += Fight( army->total/5, army->success, foes);
    }

    if (!MakeFriendOrFoeList())
        return;

    for ( auto army : foes ) {
        if ( army->total )
            army->killed += Fight( army->total/5, army->success, friends );
    }
}

void lom_battle::ProcessAllCharacters( void )
{
    for( auto character : info->objCharacters ) {

        CONTINUE_IF ( character->Race() == RA_MIDWINTER );

        character->battleloc = info->Location() ;

        MakeFriendOrFoeList ( character );
        if ( !foes.empty() ) {
            character->battleslew = Fight( character->strength, character->energy+128, foes );
        }
    }
}

bool lom_battle::ChanceOfEngagement(u32 success)
{
    return (u32)mxrandom(0, 255) < success;
}

bool lom_battle::ChanceOfSuccessfullKill(u32 success)
{
    return (u32)mxrandom(0, 255) > success;
}

bool lom_battle::SelectOpponent(c_army& armies)
{
    return mxrandom(0, (u32)armies.size() - 1);
}

u32 lom_battle::Fight ( u32 attacks, u32 success, c_army& foes )
{
    u32 killed = 0;

    while ( attacks-- && !foes.empty() ) {

        if (ChanceOfEngagement(success)) {

            auto index = SelectOpponent(foes);
            auto army = foes[index];

            if (ChanceOfSuccessfullKill(army->success)) {

                int amount  = std::min<int>((u32)5,army->total);

                army->total -= amount;
                killed += amount;

                if ( army->total <= 0) {
                    army->total=0;
                    foes.erase( foes.begin()+index);
                }
            }
        }
    }

    return killed;
}

void lom_battle::CalculateBattleStatus()
{
    status = BA_CONTINUES;
    if ( foes.empty() )
        status = BA_FRIEND;
    else if ( friends.empty() )
        status = BA_FOE;
}

void lom_battle::CheckVictors ( void )
{
    MakeFriendOrFoeList();
    
    CalculateBattleStatus();

    UpdateArmies();

    UpdateCharacters();

    Announce(location);
}

void lom_battle::UpdateArmies()
{
    for ( auto army : info->armies ) {

        switch ( army->armytype ) {

            case AT_STRONGHOLD:
                UpdateStrongholdArmy(army);
                break;

            case AT_REGIMENT:
                UpdateRegimentArmy(army);
                break;

            case AT_CHARACTER:
                UpdateCharacterArmy(army);
                break;
                
            default:
                break;
        }
    }
}

void lom_battle::UpdateStrongholdArmy(mxarmy* army)
{
    auto stronghold = static_cast<mxstronghold*>(army->parent);
    stronghold->Lost(stronghold->TotalTroops()-army->total);
    stronghold->TotalTroops(army->total) ;
    stronghold->Killed(army->killed);
    stronghold->CheckForZero();

    if ( army->race == RA_DOOMGUARD && status == BA_FRIEND ) {
       stronghold->MakeChangeSides( RA_FREE, DEF_SCENARIO(luxor) );

    } else if ( army->race != RA_DOOMGUARD && status == BA_FOE ) {
       stronghold->MakeChangeSides( RA_DOOMGUARD, DEF_SCENARIO(doomdark) );
    }
}


void lom_battle::UpdateRegimentArmy(mxarmy* army)
{
    auto regiment = static_cast<mxregiment*>(army->parent);
    regiment->Lost(regiment->Total()-army->total);
    regiment->Total(army->total) ;
    regiment->Killed(army->killed);
}

void lom_battle::UpdateCharacterArmy(mxarmy* army)
{
    auto character = static_cast<mxcharacter*>(army->parent);
    
    auto unit = army->type == UT_RIDERS
        ? static_cast<mxunit*>(&character->riders)
        : static_cast<mxunit*>(&character->warriors) ;

    unit->Lost(unit->Total()-army->total);
    unit->Total(army->total);
    unit->Killed(army->killed);
    unit->Energy ( BSub(unit->Energy(), (u32)sv_battle_default_energy_drain, 0) );
}

void lom_battle::UpdateCharacters()
{
    for( auto character : info->objCharacters ) {

        CONTINUE_IF ( character->Race() == RA_MIDWINTER );

        CharacterLosesEnergy( character );

        switch (status) {
            case BA_CONTINUES:
                CharacterContinuesBattle(character);
                break;
            case BA_FRIEND:
                CharacterWinsBattle(character);
                break;
            case BA_FOE:
                CharacterLosesBattle(character);
                break;
            default:
                break;
        }
    }
}

void lom_battle::CharacterLosesEnergy( mxcharacter* character )
{
    character->energy = BSub(character->energy, (u32)sv_battle_default_char_energy_drain, 0);
}

void lom_battle::CharacterLosesBattle( mxcharacter* character )
{
    character->LostFight();
    character->LostBattle(true);
}

void lom_battle::CharacterWinsBattle( mxcharacter* character )
{
    character->Flags().Set(cf_wonbattle);
}

void lom_battle::CharacterContinuesBattle( mxcharacter* character )
{
    character->Flags().Set(cf_inbattle);
}

// Potential better stronghold change sides
//
//                    // find the first character that is allowed an army
//                    // and allow them to occupy the keep
//                    bool changesides=true;
//                    int j=0;
//                    for ( j=0; j<info->objCharacters.Count(); j++ ) {
//                        auto character = info->objCharacters[j];
//                        if ( character->IsAllowedArmy() ) {
//                            stronghold->MakeChangeSides( RA_FREE, character );
//                            changesides=false;
//                            break;
//                        }
//                    }
//
//                    // otherwise default to the free
//                    if ( changesides ) {
//                        stronghold->MakeChangeSides( RA_FREE, info->objCharacters.First() );
//                    }

}
// namespace tme

#endif // _LOM_
