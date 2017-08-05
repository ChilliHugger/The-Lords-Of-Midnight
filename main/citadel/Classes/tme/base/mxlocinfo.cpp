/*
 * FILE:    mxlocinfo.cpp
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



namespace tme {

	//namespace item {


		mxlocinfo::mxlocinfo()
		{
			Clear();
		}

		mxlocinfo::~mxlocinfo()
		{
			SAFEDELETE ( infront );
		}

		void mxlocinfo::Clear()
		{
			looking = DR_NONE;
			sel_flags.Reset(0xffffffff);
			flags.Reset(0xffffffff);
			flags.Set(lif_moveforward);
			fightthing = OB_NONE ;
			adj_fear = NONE ;
			adj_moral = NONE ;
			adj_stronghold = NONE ;

			CLEARARRAY ( armies );
            friends.Clear();
            foe.Clear();
			doomdark.Clear();

			infront = NULL ;
			owner = NULL;

			nRecruited = NONE;
			nArmies = NONE;

			objCharacters.Clear();
			objRegiments.Clear();
			objRoutenodes.Clear();
			objRecruit.Clear();
            
            stubborn_follower_move=NULL;
            stubborn_follower_battle=NULL;
#if defined(_DDR_)
            object_to_take=NULL;
            someone_to_give_to=NULL;
#endif
		}

		mxlocinfo::mxlocinfo( mxgridref loc, mxcharacter* iowner, flags32_t sflags )
				: mxitem ( loc )
		{
			// and find out what/who is with us
			Clear();
			owner = iowner ;
			sel_flags.Set( sflags );
			WorkOutLocationDetails();
		}

		mxlocinfo::mxlocinfo( mxgridref loc, mxdir_t dir, mxcharacter* iowner, flags32_t sflags ) 
				: mxitem ( loc )
		{
		mxterrain* tinfo;

			Clear();
			owner = iowner ;
			sel_flags.Set( sflags ) ;
			WorkOutLocationDetails();
			
			looking = dir;
			loc_lookingat = mx->scenario->FindLookingTowards( location, looking );
			loc_infront = location + looking ;

			// now check the location in front
			infront = new mxlocinfo(loc_infront,iowner,sel_flags);
			
			//TN_GATE TN_TEMPLE TN_PIT TN_PALACE
#if defined(_DDR_)
            flags.Reset(lif_blocked);
			if ( iowner && iowner->IsInTunnel() ) {
				// we can only move through tunnels when in a tunnel
				if ( !(infront->mapsqr.HasTunnel()) ) {
					flags.Reset(lif_moveforward);
                    flags.Set(lif_blocked);
                }
				// check for exit
				//if ( mapsqr.HasTunnelExit() )
				//	flags.Set(lif_exit_tunnel);  ;
			}else{
#endif
				// can we pass through the terrain infront?
				tinfo = mx->TerrainById(infront->mapsqr.terrain);
				if ( tinfo->IsBlock() ) {
					flags.Reset(lif_moveforward);// = FALSE;
                    flags.Set(lif_blocked);
                }
                
#if defined(_DDR_)
				//
				if ( mapsqr.HasTunnelEntrance() )
					flags.Set(lif_enter_tunnel);  ;
				
			}
#endif


			// the only way to move forard when there is an army infront of you
			// is to enter battle
			if ( infront->foe.armies) {
				if ( !sv_cheat_armies_noblock )
					flags.Reset(lif_moveforward); // = FALSE;
				flags.Set(lif_enterbattle); // = TRUE ;
			}
            
            // make sure we don't take too many lords into battle
            u32 extra= owner!=NULL ? owner->followers : 0 ;
            
			// but if there are too many people there, then we can't
			if ( infront->friends.armies+extra >= MAX_CHARACTERS_INLOCATION ) {
				flags.Reset(lif_moveforward); // = FALSE;
				flags.Reset(lif_enterbattle); // = FALSE;
			}

			// we can't enter another battle while we are in one, which we must be
			// if we are in the same location as the enemy
			if ( foe.armies  )
				flags.Reset(lif_enterbattle); // = FALSE ;

		}

		void mxlocinfo::WorkOutLocationDetails()
		{
			mapsqr = mx->gamemap->GetAt ( location );

			friends.Clear();
            foe.Clear();
			doomdark.Clear();

			// the midnightx engine stores last nights adj_stronghold
			// the scenario holds current, thus we call midnightx
			adj_stronghold = sv_strongholdadjuster;

			adj_fear = mx->scenario->CalcFearAdjuster(this);

			// a bi product of FindArmiesHere
			// is that Characters, Regiments, Strongholds, 
			// are called as well
			FindArmiesHere ();

			// routenodes
			mx->CollectRoutenodes ( location, objRoutenodes );
            
#if defined(_DDR_)
            object_to_take = mx->scenario->FindObjectAtLocation(location);
            if ( object_to_take!= NULL )
                flags.Set(lif_take);
#endif

		}

		void mxlocinfo::FindApproachCharactersInfront ( const mxcharacter* c )
		{
			infront->FindRecruitCharactersHere(c);
            
#if defined(_DDR_)
            if ( infront->objCharacters.Count()>1 )
                return;
#endif
			for (u32 ii = 0; ii <infront->objRecruit.Count(); ii++)
				objRecruit.Add ( infront->objRecruit[ii] );
		}

	//
	// METHOD:	FindRecruitCharactersHere
	//			Creates a list of characters that have not been recruited  
	// 
	// PARAMS:	c			Current Character
	// 
	// REMARKS:	
	//
	// RETURNS:	NONE
	//
		void mxlocinfo::FindRecruitCharactersHere ( const mxcharacter* c )
		{
			if ( c == NULL )
				return;

			// tunnel check
			BOOL isInTunnel = c->IsInTunnel() ;

			objRecruit.Create(MAX_CHARACTERS_INLOCATION);
			
			for (u32 ii = 0; ii <objCharacters.Count(); ii++) {

				mxcharacter* character = (mxcharacter*)objCharacters[ii];
				
				// not dead, hidden, or recruited
				if ( character==NULL || character->IsDead() || character->IsHidden() || character->IsRecruited() )
					continue;
				
				// not self
				if ( character == c )
					continue;
				
				// character location
				if ( character->Location() != location )
					continue;

				// tunnel status must be the same
				if ( character->IsInTunnel() != isInTunnel )
					continue;

				// TODO check global flag AlwaysAttemptRecruit
				// and maybe set the character up for recruit anyway
#if defined(_LOM_)
				if ( c->CheckRecruitChar ( character ) )
#endif
                    objRecruit += character ;
			}

		}

	//
	// METHOD:	FindCharactersHere
	//			Creates a list of characters that are also here, not including non recruited characters
	// 
	// REMARKS:	
	//
	// RETURNS:	NONE
	//
		void mxlocinfo::FindCharactersHere ( void )
		{
		int ii;
		mxcharacter*	moonringcarrier=NULL;
		mxcharacter*	c=NULL;
		mxobject*		moonring=NULL;
		BOOL			bInTunnel = FALSE ;

			
			
			// tunnel check
			bInTunnel = (sel_flags & slf_tunnel); //owner && owner->IsInTunnel();

			nRecruited=0;

			// setup an empty collection
			objCharacters.Create(MAX_CHARACTERS_INLOCATION);

			// is anyone carrying the moonring?
			// if they are not then the only person we are going to be able to see
			// are people who could potentially carry the moonring
			if ( mx->scenario->IsFeature(SF_MOONRING) ) {
				moonring = (mxobject*)mx->EntityByName("OB_MOONRING"); 
				//moonringcarrier = mx->scenario->WhoHasObject(moonring);
                moonringcarrier = mx->scenario->CurrentMoonringWearer();

            }

			// work through all characters to find the already recruited characters
			for (ii = 0; ii < sv_characters; ii++) {
				c = mx->CharacterById(ii+1);

				// can't see dead hidden or not recruited characters
				if ( c==NULL || c->IsDead() || c->IsHidden() || !c->IsRecruited() )
					continue;	
				// must be at this location
				if ( c->Location() != location )
					continue;

				// check tunnel
				if ( !bInTunnel && c->IsInTunnel() )	continue;
				if ( bInTunnel && !c->IsInTunnel() )	continue;
				//

				// potential moonring carrier or someone is carrying the moonring
				//if ( moonring!=NULL ) {
				//	if ( moonringcarrier==NULL && !c->IsAllowedMoonring() )
				//		continue;
				//}
				objCharacters += c ;
				nRecruited++;
			}

			// now lets get the unrecruited characters
			for (ii = 0; ii < sv_characters; ii++) {
				c = mx->CharacterById(ii+1);
				// can't see dead hidden or not recruited characters
				if ( c==NULL || c->IsDead() || c->IsHidden() || c->IsRecruited() )
					continue;	
				// must be at this location
				if ( c->Location() != location )
					continue;
				// check tunnel
				if ( !bInTunnel && c->IsInTunnel() )	continue;
				if ( bInTunnel && !c->IsInTunnel() )	continue;
				//
				objCharacters += c ;
			}
            
		}
	
	//
	// METHOD:	FindArmiesHere
	//			Creates a list of all armies at the current location
	// 
	// 
	// REMARKS:	NOT FUTURE PROOF
	//			This function ideally needs to use some mechanism of friend or foe
	//			based on the character requesting the location info
	//
	// RETURNS:	NONE
	//
		void mxlocinfo::FindArmiesHere ( void )
		{
		u32		ii;
		mxarmy*	army;
		int		success;
		BOOL	bInTunnel=FALSE;

			nArmies=0;

			// doomdark is currently foe
			mxcharacter* ch_doomdark = (mxcharacter*)mx->EntityByName("CH_DOOMDARK");
			
			// luxor is currently friend
			mxcharacter* ch_luxor = (mxcharacter*)mx->EntityByName("CH_LUXOR");

			FindCharactersHere();

			// no strongholds when under the ground
			bInTunnel = (sel_flags & slf_tunnel) ; //owner && owner->IsInTunnel();

			if ( !bInTunnel ) {
				mx->CollectRegiments ( location, objRegiments );
				mx->CollectStrongholds ( location, objStrongholds );
			}

			army = armies;

			foe.Type ( IDT_ARMYTOTAL );
			foe.Location ( Location() );
			foe.adjustment=0;
			foe.character = ch_doomdark;

			doomdark.Type ( IDT_ARMYTOTAL );
			doomdark.Location ( Location() );
			doomdark.adjustment=0;
			doomdark.character = ch_doomdark;

			friends.Type ( IDT_ARMYTOTAL );
			friends.Location ( Location() );
			friends.adjustment=0;
			friends.character = ch_luxor;

			// TODO all these need to be goverened by "friendorfoe"

			// all the strongholds here
			if ( objStrongholds.Count() ) {

					mxstronghold* stronghold = (mxstronghold*)objStrongholds[0];

					success = mx->TerrainById(mapsqr.terrain)->Success();

                    BOOL isLoyal = stronghold->OccupyingRace() != RA_DOOMGUARD ;
#if defined(_DDR_)
                isLoyal = stronghold->Loyalty() == ch_luxor->Loyalty();
#endif
                
					//if ( stronghold->Owner() && (!stronghold->Owner()->IsFriend(owner)) )  {
					if ( !isLoyal ) {
						foe.adjustment = success ;
						foe.armies++;
						if ( stronghold->Type() == UT_WARRIORS )
							foe.warriors+=stronghold->Total();
						else if ( stronghold->Type() == UT_RIDERS )
							foe.riders+=stronghold->Total();
					} else {
						friends.adjustment = success ;
						friends.armies++;
						if ( stronghold->Type() == UT_WARRIORS )
							friends.warriors+=stronghold->Total();
						else if ( stronghold->Type() == UT_RIDERS )
							friends.riders+=stronghold->Total();
					}

					army->parent = stronghold ;
					army->armytype = AT_STRONGHOLD ;
					army->race = stronghold->OccupyingRace();
					army->type = stronghold->Type();
					army->total = stronghold->Total();
					army->success = stronghold->BattleSuccess((mxlocinfo&)*this);
					army->killed = NONE;
					army->loyalto = stronghold->Owner() ? stronghold->Owner()->loyalty : RA_NONE ;
					army++;
			}

			// all the regiments here
			for ( ii=0; ii<objRegiments.Count(); ii++ ) {

				mxregiment* reg = (mxregiment*)objRegiments[ii] ;

				if ( reg->Total() ) {
					army->armytype = AT_REGIMENT ;
					army->parent = reg ;
					army->race = reg->Race();
					army->type = reg->Type();
					army->total = reg->Total();
					army->success = reg->BattleSuccess( (mxlocinfo&)*this );
					army->killed = NONE;
					army->loyalto = reg->Loyalty()->loyalty ;

					//if ( reg->Loyalty()->IsFriend(owner) ) {
					//	if ( army->type == UT_WARRIORS ) {
					//		friends.warriors+=army->total;
					//	} else if ( army->type == UT_RIDERS ) {
					//		friends.riders+=army->total;
					//	}
					//}else{
						if ( army->type == UT_WARRIORS ) {
							foe.warriors+=army->total;
							doomdark.warriors+=army->total;
						} else if ( army->type == UT_RIDERS ) {
							foe.riders+=army->total;
							doomdark.riders+=army->total;
						}
						foe.armies++;
					//}
					army++;	
				}
			}


            
            
			// lets get all the armies belonging to the character
			for ( ii=0; ii<objCharacters.Count(); ii++ ) {

				mxcharacter* c = (mxcharacter*)objCharacters[ii];

                if ( c->warriors.Total() ) {
					army->armytype = AT_CHARACTER ;
					army->parent = c ;
					army->race = c->Race();
					army->type = UT_WARRIORS;
					army->total = c->warriors.Total();
					army->success = c->warriors.BattleSuccess ( (mxlocinfo&)*this, c );
					army->killed = NONE;
					army->loyalto = c->loyalty ;

#if defined(_DDR_)
					if ( c->IsFriend(ch_luxor) ) {
						friends.armies++;
						friends.warriors+=army->total;
					}else{
						foe.armies++;
						foe.warriors+=army->total;
					}
#else
                    friends.armies++;
                    friends.warriors+=army->total;
#endif
                
					army++;
				}

				if ( c->riders.Total() ) {
					army->armytype = AT_CHARACTER ;
					army->parent = c;
					army->race = c->Race();
					army->type = UT_RIDERS;
					army->total = c->riders.Total();
					army->success = c->riders.BattleSuccess ( (mxlocinfo&)*this, c );
					army->killed = NONE;
#if defined(_DDR_)
					if ( c->IsFriend(ch_luxor) ) {
						friends.armies++;
						friends.riders+=army->total;
					}else{
						foe.armies++;
						foe.riders+=army->total;
					}
#else
                    friends.armies++;
                    friends.riders+=army->total;
#endif
					army++;
				}

			}

			
			nArmies = army - armies ;
		}

		mxarmytotal::mxarmytotal()
		{
			Type( IDT_ARMYTOTAL );
		}

		mxarmytotal::~mxarmytotal()
		{
		}

	//}
	// namespace item
}
// namespace tme
