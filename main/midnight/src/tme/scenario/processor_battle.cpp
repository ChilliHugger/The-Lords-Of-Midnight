/*
 * FILE:    processor_battle.cpp
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

#include "../baseinc/processor_battle.h"
#include "../baseinc/variables.h"

#include <algorithm>
#include <string>

namespace tme {


mxbattle::mxbattle()
{
}

mxbattle::~mxbattle()
{
}


void mxbattle::ResetBattlesFought()
{ 
    battlesfought=0;
    if ( battleareas.Size() == 0 )
        battleareas.Create( sv_areas );

    battleareas.ClrAll();
}

/*
 * Function name    : mxbattle::KickOffAtLocation
 * 
 * Return type        : void
 * 
 * Arguments        : mxgridref loc
 * 
 * Description        : 
 * 
 */

void mxbattle::KickOffAtLocation ( mxgridref loc )
{

    // calculate the information required for the location
    info = new mxlocinfo ( loc, NULL, 0 );

    if ( info->foe.armies ) {

        // calculate how many men each character
        // in this location is kills
        CalculateCharactersKillRates();

        // Do First Pass
        AllOutWar(0);

        // Do Second Pass
        //AllOutWar(1);
        
        // Who Won?
        CheckVictors();
    }

    // Describe the deaths
    SAFEDELETE( info );
}




/*
 * Function name    : mxbattle::MakeFriendOrFoeList
 * 
 * Return type        : void
 * 
 * Arguments        : const mxcharacter* character
 * 
 * Description        : 
 * 
 */

void mxbattle::MakeFriendOrFoeList ( const mxcharacter* character )
{
u32 j;

    foes.Clear();
    friends.Clear();

    for ( j=0; j<info->nArmies; j++ ) {
        if ( info->armies[j]->total ) {
            //if ( info->armies[j].loyalto != character->loyalty ) 
            if ( info->armies[j]->race == RA_DOOMGUARD )
                foes.Add(info->armies[j]);
            else
                friends.Add(info->armies[j]);
        }
    }

    nFriends = friends.Count();
    nFoes = foes.Count();
}


/*
 * Function name    : mxbattle::AllOutWar
 * 
 * Return type        : void
 * 
 * Arguments        : void
 * 
 * Description        : 
 * 
 */


void mxbattle::AllOutWar ( int pass )        // TMP: pass parameter added
{
u32 ii;

    MakeFriendOrFoeList();
    if ( nFriends==0 || nFoes==0 )
        return;

    for (ii=0; ii<nFriends; ii++) {
        if ( friends[ii]->total )
            friends[ii]->killed += Fight( friends[ii]->total/5, friends[ii]->success, foes, nFoes);
    }

    MakeFriendOrFoeList();
    if ( nFriends==0 || nFoes==0 )
        return;

    for (ii=0; ii<nFoes; ii++) {
        if ( foes[ii]->total )
            foes[ii]->killed += Fight( foes[ii]->total/5, foes[ii]->success, friends, nFriends );
    }
}


/*
 * Function name    : mxbattle::CalculateCharactersKillRates
 * 
 * Return type        : void
 * 
 * Arguments        : void
 * 
 * Description        : 
 * 
 */

void mxbattle::CalculateCharactersKillRates( void )
{
u32 i;
mxcharacter* character;

    for ( i=0; i<info->objCharacters.Count(); i++ ) {
        character = (mxcharacter*)info->objCharacters[i];
    
        if ( character->Race()==RA_MIDWINTER )
            continue;
        
        // build the foe list
        MakeFriendOrFoeList ( character );
        if ( nFoes ) {
            character->battleslew = Fight( character->strength, character->energy+128, foes, nFoes );
        }
    }
}



/*
 * Function name    : mxbattle::Fight
 * 
 * Return type        : int
 * 
 * Arguments        : int chances
 *                  : int success
 *                  : mxarmy* foes[]
 *                  : int nFoes
 * 
 * Description        : 
 * 
 */



u32 mxbattle::Fight ( u32 attacks, u32 success, c_army& foes, u32& nFoes )
{
    int army;
    int    killed=0;

    while ( attacks-- && nFoes ) {

        if ((u32)mxrandom(0, 255) < (u32)success) {

            // pick a random army to fight from the
            // foe table
            army = mxrandom(0, nFoes - 1);

            if ((u32)mxrandom(0, 255) > (u32)foes[army]->success) {

                int amount  = std::min<int>((u32)5,foes[army]->total);

                foes[army]->total-=amount;
                killed+=amount;

                // if this army has been wiped out
                // then remove it from the army table
                if ( foes[army]->total <= 0) {
                    foes[army]->total=0;
                    foes[army]=foes[nFoes-1];
                    nFoes--;
                }
            }
        }
    }

    return killed;
}



/*
 * Function name    : mxbattle::CheckVictors
 * 
 * Return type        : void
 * 
 * Arguments        : void
 * 
 * Description        : 
 * 
 */

void mxbattle::CheckVictors ( void )
{
u32                 ii;
mxbattlestatus_t    battlestatus;
mxstronghold*       stronghold;
mxregiment*         regiment;
mxcharacter*        character;
mxunit*             unit;

    MakeFriendOrFoeList();
    
    battlestatus=BA_CONTINUES;

    if ( nFoes==0 ) 
        battlestatus=BA_FRIEND;
    else if ( nFriends==0 ) 
        battlestatus=BA_FOE;


    for ( ii=0; ii<info->nArmies; ii++ ) {

        auto army = info->armies[ii];

        switch ( army->armytype ) {

            case AT_STRONGHOLD:

                // loses a stronghold
                stronghold = (mxstronghold*)army->parent;
                stronghold->Lost(stronghold->TotalTroops()-army->total);            // TMP: Added !
                stronghold->TotalTroops(army->total) ;
                stronghold->Killed(army->killed);
                stronghold->CheckForZero();

                if ( army->race == RA_DOOMGUARD && battlestatus==BA_FRIEND ) {

#if 0
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
#else
                   stronghold->MakeChangeSides( RA_FREE, DEF_SCENARIO(luxor) );
#endif

                } else if ( army->race != RA_DOOMGUARD && battlestatus==BA_FOE ) {
                    // find the first foe
                   stronghold->MakeChangeSides(RA_DOOMGUARD,DEF_SCENARIO(doomdark));
                }
                break;

            case AT_REGIMENT:
                regiment = (mxregiment*)army->parent;
                regiment->Lost(regiment->Total()-army->total);                // TMP: Added !
                regiment->Total(army->total) ;
                regiment->Killed(army->killed);
                break;

            case AT_CHARACTER:
                character = (mxcharacter*)army->parent;
                
                unit = army->type == UT_RIDERS
                    ? static_cast<mxunit*>(&character->riders)
                    : static_cast<mxunit*>(&character->warriors) ;

                unit->Lost(unit->Total()-army->total);
                unit->Total(army->total); 
                unit->Killed(army->killed);
                unit->Energy ( BSub(unit->Energy(), (u32)sv_battle_default_energy_drain, 0) );
                break;
                
            default:
                break;

        }
    }

    // mark all the lords as in battle, etc
    for ( ii=0; ii<info->objCharacters.Count(); ii++ ) {
        character = (mxcharacter*)info->objCharacters[ii];
        if ( character->Race() == RA_MIDWINTER )
            continue;

        character->energy = BSub(character->energy, (u32)sv_battle_default_char_energy_drain, 0);
        character->battleloc = info->Location() ;

        if ( battlestatus==BA_CONTINUES ){
            character->Flags().Set(cf_inbattle);
        }else if ( battlestatus == BA_FRIEND ){
            character->Flags().Set(cf_wonbattle);
        }else{
            character->LostFight();
            character->LostBattle(true);
        }
    }


    int area = info->mapsqr.area;

    // The bloody sword of battle brings death in the domain of
    // ashimar and of Valethor
    mx->text->loc = info->Location();
    if ( battlesfought==0 ) {
        mx->SetLastActionMsg(mx->LastActionMsg() + mx->text->CookedSystemString(SS_BATTLE2)) ;
    } else {
        if ( !battleareas[area] )
            mx->SetLastActionMsg(mx->LastActionMsg() + mx->text->CookedSystemString(SS_BATTLE3) );
    }


    // flag a battle as taking place in this area
    if ( sv_collate_battle_areas )
        battleareas.Set ( area );    

    mx->NightCallback(NULL);

    battlesfought++;

}





/*
 * Function name    : mxbattle::BaseDoomdarkSuccess
 * 
 * Return type        : int
 * 
 * Arguments        : mxrace_t race
 *                  : mxunit_t unit
 *                  : const mxlocinfo& locinfo
 * 
 * Description        : 
 * 
 */

s32 mxbattle::BaseDoomdarkSuccess ( mxrace_t race, mxunit_t unit, const mxlocinfo& locinfo )
{
s32 success;
s32    modifier;
    // TODO this should be a lookup table
    // TODO this should be race dependent
    modifier = (unit == UT_WARRIORS) 
        ? (s32)sv_battle_success_regiment_warriors : (s32)sv_battle_success_regiment_riders ;
    success = locinfo.foe.adjustment + ( locinfo.adj_fear / modifier ); 
    return success;
}


}
// namespace tme
