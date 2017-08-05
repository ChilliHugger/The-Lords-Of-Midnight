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
 * Function name	: mxbattle::KickOffAtLocation
 * 
 * Return type		: void
 * 
 * Arguments		: mxgridref loc
 * 
 * Description	    : 
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
 * Function name	: mxbattle::MakeFriendOrFoeList
 * 
 * Return type		: void
 * 
 * Arguments		: const mxcharacter* character
 * 
 * Description	    : 
 * 
 */

void mxbattle::MakeFriendOrFoeList ( const mxcharacter* character )
{
u32 j;

//	_ASSERTE ( character!=NULL );

	nFoes=0;
	nFriends=0;
	CLEARARRAY ( foes );
	CLEARARRAY ( friends );

	for ( j=0; j<info->nArmies; j++ ) {
		if ( info->armies[j].total ) {
			//if ( info->armies[j].loyalto != character->loyalty ) 
			if ( info->armies[j].race == RA_DOOMGUARD ) 
				foes[nFoes++] = &info->armies[j];
			else
				friends[nFriends++] = &info->armies[j];
		}
	}

	_ASSERTE ( nFriends < NUMELE( friends ) );
	_ASSERTE ( nFoes < NUMELE( foes ) );
}


/*
 * Function name	: mxbattle::AllOutWar
 * 
 * Return type		: void
 * 
 * Arguments		: void
 * 
 * Description	    : 
 * 
 */


void mxbattle::AllOutWar ( int pass )		// TMP: pass parameter added
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
 * Function name	: mxbattle::CalculateCharactersKillRates
 * 
 * Return type		: void
 * 
 * Arguments		: void
 * 
 * Description	    : 
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
		if ( nFoes )
			character->battleslew = Fight( character->strength, character->energy+128, foes, nFoes );
		//else
		//	character->battleslew = 0;
	}
}



/*
 * Function name	: mxbattle::Fight
 * 
 * Return type		: int
 * 
 * Arguments		: int chances
 *                  : int success
 *                  : mxarmy* foes[]
 *                  : int nFoes
 * 
 * Description	    : 
 * 
 */



u32 mxbattle::Fight ( u32 attacks, u32 success, mxarmy* foes[], u32& nFoes )
{
	int army;
	int	killed=0;

	while ( attacks-- && nFoes ) {

		if ( (u32)random(0,255) < (u32)success ) {

			// pick a random army to fight from the
			// foe table
			army = random(0,nFoes-1);

			if ( (u32)random(0,255) > (u32)foes[army]->success ) {

				int amount  = MIN(5,foes[army]->total);

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
 * Function name	: mxbattle::CheckVictors
 * 
 * Return type		: void
 * 
 * Arguments		: void
 * 
 * Description	    : 
 * 
 */

void mxbattle::CheckVictors ( void )
{
u32					ii;
mxbattlestatus_t		battlestatus;
mxstronghold*	stronghold;
mxregiment*		regiment;
mxcharacter*	character;
mxunit*		unit;

	MakeFriendOrFoeList();
	
	battlestatus=BA_CONTINUES;

	if ( nFoes==0 ) 
		battlestatus=BA_FRIEND;
	else if ( nFriends==0 ) 
		battlestatus=BA_FOE;


	for ( ii=0; ii<info->nArmies; ii++ ) {

		mxarmy*	army = &info->armies[ii];

		switch ( army->armytype ) {

			case AT_STRONGHOLD:

				// loses a stronghold
				stronghold = (mxstronghold*)army->parent;
				stronghold->Lost(stronghold->Total()-army->total);			// TMP: Added !
				stronghold->Total(army->total) ;
				stronghold->Killed(army->killed);
				stronghold->CheckForZero();

				if ( army->race == RA_DOOMGUARD && battlestatus==BA_FRIEND ) {

#if 0
                    
					// find the first character that is allowed an army
					// and allow them to occupy the keep
					BOOL changesides=true;
                    int j=0;
					for ( j=0; j<info->objCharacters.Count(); j++ ) {
						mxcharacter* character = (mxcharacter*)info->objCharacters[j];
						if ( character->IsAllowedArmy() ) {
							stronghold->MakeChangeSides( RA_FREE, character );
							changesides=false;
							break;
						}
					}

					// otherwise default to the free
					if ( changesides ) {
						stronghold->MakeChangeSides( RA_FREE, (mxcharacter*)info->objCharacters[0] );
					}
#else
 					mxcharacter* luxor = (mxcharacter*)mx->EntityByName("CH_LUXOR");
                   stronghold->MakeChangeSides( RA_FREE, luxor );
                  
#endif


				} else if ( army->race != RA_DOOMGUARD && battlestatus==BA_FOE ) {

					// find the first foe

					mxcharacter* doomdark = (mxcharacter*)mx->EntityByName("CH_DOOMDARK");
					stronghold->MakeChangeSides(RA_DOOMGUARD,doomdark);	



				}
				break;

			case AT_REGIMENT:
				regiment = (mxregiment*)army->parent;
				regiment->Lost(regiment->Total()-army->total);				// TMP: Added !
				regiment->Total(army->total) ;
				regiment->Killed(army->killed);
				break;

			case AT_CHARACTER:
				character = (mxcharacter*)army->parent;
				
				if ( army->type == UT_RIDERS ) 
					unit = &character->riders  ;
				else
					unit = &character->warriors ;

				unit->Lost(unit->Total()-army->total);
				unit->Total(army->total); 
				unit->Killed(army->killed);

				// reduce armies energy
				// unit->Energy ( MAX(0,(unit->Energy() - (s32)sv_battle_default_energy_drain)) );
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

        
		// reduce characters energy
		//character->energy -= (u32)sv_battle_default_char_energy_drain ;
		//if ( character->energy <= 0 )
		//	character->energy=0;
        character->energy = BSub(character->energy, (u32)sv_battle_default_char_energy_drain, 0);
		
		character->battleloc = info->Location() ;

		if ( battlestatus==BA_CONTINUES ){
			character->Flags().Set(cf_inbattle);
		}else if ( battlestatus == BA_FRIEND ){
			character->Flags().Set(cf_wonbattle);
		}else{
			character->LostFight();
			character->Displace();
		}
	}


	int area = info->mapsqr.area;

	// The bloody sword of battle brings death in the domain of
	// ashimar and of Valethor

	//mx->text->ainfo = mx->AreaById(area);
	mx->text->loc = info->Location();
	if ( battlesfought==0 )
		chilli::lib::strcpy ( EOS(mx->LastActionMsg()),mx->text->CookedSystemString(SS_BATTLE2) );
	else {
		if ( !battleareas[area] )
			chilli::lib::strcpy ( EOS(mx->LastActionMsg()),mx->text->CookedSystemString(SS_BATTLE3) );
	}


	// flag a battle as taking place in this area
    if ( sv_collate_battle_areas )
        battleareas.Set ( area );	

	mx->NightCallback(NULL);

	battlesfought++;

}





/*
 * Function name	: mxbattle::BaseDoomdarkSuccess
 * 
 * Return type		: int
 * 
 * Arguments		: mxrace_t race
 *                  : mxunit_t unit
 *                  : const mxlocinfo& locinfo
 * 
 * Description	    : 
 * 
 */

s32 mxbattle::BaseDoomdarkSuccess ( mxrace_t race, mxunit_t unit, const mxlocinfo& locinfo )
{
s32 success;
s32	modifier;
	// TODO this should be a lookup table
	// TODO this should be race dependent
	modifier = (unit == UT_WARRIORS) 
		? (s32)sv_battle_success_regiment_warriors : (s32)sv_battle_success_regiment_riders ;
	success = locinfo.foe.adjustment + ( locinfo.adj_fear / modifier ); 
	return success;
}


}
// namespace tme
