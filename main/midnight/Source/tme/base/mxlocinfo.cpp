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

#if defined(_DDR_)
#include "../scenarios/ddr/scenario_ddr_internal.h"
#endif

#include <string>


namespace tme {

    //namespace item {


        mxlocinfo::mxlocinfo()
        {
            Clear();
        }

        mxlocinfo::~mxlocinfo()
        {
            Clear();
        }

        void mxlocinfo::Clear()
        {
            looking = DR_NONE;
            sel_flags.Reset(0xffffffff);
            flags.Reset(0xffffffff);
            flags.Set(lif_moveforward);
            fightthing = OB_NONE ;
            adj_fear = 0 ;
            adj_moral = 0 ;
            adj_stronghold = 0 ;

            friends.Clear();
            foe.Clear();
            regiments.Clear();

            infront.reset() ;
            owner = nullptr;
            tunnel = false;

            nRecruited = 0;

            nArmies = 0;
            
            for( int ii=0; ii<armies.size(); ii++ ) {
                SAFEDELETE(armies[ii]);
            }
            
            armies.clear();

            objCharacters.Clear();
            objRegiments.Clear();
            objRoutenodes.Clear();
            objRecruit.Clear();
            
            stubborn_follower_move=nullptr;
            stubborn_follower_battle=nullptr;
#if defined(_DDR_)
            object_to_take=nullptr;
            someone_to_give_to=nullptr;
#endif
        }

        mxlocinfo::mxlocinfo( mxgridref loc, const mxcharacter* iowner, flags32_t sflags )
                : mxitem ( loc )
        {
            // and find out what/who is with us
            Clear();
            owner = iowner ;
            sel_flags.Set( sflags );
            WorkOutLocationDetails();
        }

        mxlocinfo::mxlocinfo( mxgridref loc, mxdir_t dir, const mxcharacter* iowner, flags32_t sflags )
                : mxitem ( loc )
        {
            Clear();
            owner = iowner ;
            sel_flags.Set( sflags ) ;
            WorkOutLocationDetails();
            
            looking = dir;
            loc_lookingat = mx->scenario->FindLookingTowards( location, looking );
            loc_infront = location + looking ;

            // now check the location in front
            infront.reset( new mxlocinfo(loc_infront,owner,sel_flags) );
            
            flags.Reset(lif_blocked);
            if ( owner && owner->IsInTunnel() ) {
                // we can only move through tunnels when in a tunnel
                if ( !infront->mapsqr.HasTunnel() || IS_DIAGONAL(looking) ) {
                    flags.Reset(lif_moveforward);
                    flags.Set(lif_blocked);
                }
            }else{
                // can we pass through the terrain infront?
                if (mx->scenario->isLocationImpassable(infront->location, owner)) {
                    flags.Reset(lif_moveforward);// = FALSE;
                    flags.Set(lif_blocked);
                }
                                
                if ( mapsqr.HasTunnelEntrance() )
                    flags.Set(lif_enter_tunnel);  ;
            }

            // the only way to move forard when there is an army infront of you
            // is to enter battle
            bool AI = owner!=nullptr ? owner->IsAIControlled() : false ;
            if ( !AI && (infront->foe.armies || infront->foe.characters) ) {
                if ( !sv_cheat_armies_noblock )
                    flags.Reset(lif_moveforward); // = FALSE;
                    
                if( !mx->scenario->isLocationImpassable(loc_infront, owner) ) {
                    flags.Set(lif_enterbattle); // = TRUE ;
                }
            }
            
            // make sure we don't take too many lords into battle
            u32 extra = owner!=nullptr ? owner->followers : 0 ;

            // characters in small tunnels
            if (tunnel && infront->mapsqr.IsSmallTunnel()) {
                if ( infront->friends.characters+extra >= MAX_CHARACTERS_IN_SMALL_TUNNEL ) {
                    flags.Reset(lif_moveforward); // = FALSE;
                    flags.Set(lif_blocked); // = TRUE
                }
            }
            
            // but if there are too many people there, then we can't
            if ( infront->friends.armies+extra >= MAX_CHARACTERS_INLOCATION ) {
                flags.Reset(lif_moveforward); // = FALSE;
                flags.Reset(lif_enterbattle); // = FALSE;
                flags.Set(lif_blocked); // = TRUE
            }

#if defined(_LOM_)
            // we can't enter another battle while we are in one, which we must be
            // if we are in the same location as the enemy
            if ( foe.armies || foe.characters  )
                flags.Reset(lif_enterbattle); // = FALSE ;
#endif

        }

        void mxlocinfo::WorkOutLocationDetails()
        {
            tunnel = (sel_flags & slf_tunnel) ;

            mapsqr = mx->gamemap->GetAt ( location );

            friends.Clear();
            foe.Clear();
            regiments.Clear();

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
            auto scenario = static_cast<ddr_x*>(mx->scenario);
            object_to_take = scenario->FindObjectAtLocation(location);
            if ( object_to_take!= nullptr ) 
                flags.Set(lif_take);
#endif

        }

        void mxlocinfo::FindApproachCharactersInfront ()
        {
            infront->FindRecruitCharactersHere();
            
#if defined(_DDR_)
            if ( infront->objCharacters.Count()>1 )
                return;
#endif
            FOR_EACH(character, infront->objRecruit)
                objRecruit.Add ( character );
        }

        void mxlocinfo::FindRecruitCharactersHere ()
        {
            RETURN_IF_NULL(owner);

            objRecruit.Clear();
            
            FOR_EACH(character, objCharacters) {

                CONTINUE_IF_NULL(character);
                
                // not dead, hidden, or recruited
                CONTINUE_IF ( character->IsDead() ||
                              character->IsHidden() ||
                              character->IsRecruited() );
                
                // not self
                CONTINUE_IF ( character == owner );
                
                // character location
                CONTINUE_IF ( character->Location() != location );
                
                // tunnel status must be the same
                CONTINUE_IF ( character->IsInTunnel() != tunnel );

                // TODO check global flag AlwaysAttemptRecruit
                // and maybe set the character up for recruit anyway
#if defined(_LOM_)
                if ( owner->CheckRecruitChar ( character ) )
                    objRecruit += character ;
#else
                objRecruit += character ;
#endif
            }

        }

    //
    // METHOD:    FindCharactersHere
    //            Creates a list of characters that are also here, not including non recruited characters
    //
        void mxlocinfo::FindCharactersHere ( void )
        {
        mxcharacter*    moonringcarrier=nullptr;
        mxobject*       moonring=nullptr;

            nRecruited=0;

            objCharacters.Clear();

            // is anyone carrying the moonring?
            // if they are not then the only person we are going to be able to see
            // are people who could potentially carry the moonring
            if ( mx->scenario->IsFeature(SF_MOONRING) ) {
                moonring = DEF_SCENARIO(moonring);
                //moonringcarrier = mx->scenario->WhoHasObject(moonring);
                moonringcarrier = mx->scenario->CurrentMoonringWearer();
            }

            // work through all characters to find the already recruited characters
            FOR_EACH_CHARACTER(c) {

                CONTINUE_IF_NULL(c);

                // can't see dead hidden or not recruited characters
                CONTINUE_IF ( c->IsDead() ||
                              c->IsHidden() ||
                              !c->IsRecruited() );
  
                // must be at this location
                CONTINUE_IF ( c->Location() != location );

                // check tunnel
                CONTINUE_IF ( c->IsInTunnel() != tunnel );

                // potential moonring carrier or someone is carrying the moonring
                //if ( moonring!=NULL ) {
                //    if ( moonringcarrier==NULL && !c->IsAllowedMoonring() )
                //        continue;
                //}
                objCharacters += c ;
                nRecruited++;
            }

            // now lets get the unrecruited characters
            FOR_EACH_CHARACTER(c) {

                CONTINUE_IF_NULL(c);
                
                // can't see dead hidden or not recruited characters
                CONTINUE_IF ( c->IsDead() ||
                              c->IsHidden() ||
                              c->IsRecruited() );
                    
                // must be at this location
                CONTINUE_IF ( c->Location() != location );
                
                // check tunnel
                CONTINUE_IF ( c->IsInTunnel() != tunnel ) ;

                //
                objCharacters += c ;
            }
            
        }
    
    //
    // METHOD:    FindArmiesHere
    //            Creates a list of all armies at the current location
    //
    // RETURNS:    NONE
    //
        void mxlocinfo::FindArmiesHere ( void )
        {
            nArmies=0;
   
            // luxor is currently always the players friend
            const auto ch_friend = owner != nullptr
                ? owner
                : DEF_SCENARIO(luxor) ;

            FindCharactersHere();

            // no strongholds when under the ground
            if ( !tunnel )
            {
                mx->CollectRegiments ( location, objRegiments );
                mx->CollectStrongholds ( location, objStrongholds );
      
            }

            foe.IdType ( IDT_ARMYTOTAL );
            foe.Location ( Location() );
            foe.adjustment=0;

            regiments.IdType ( IDT_ARMYTOTAL );
            regiments.Location ( Location() );
            regiments.adjustment=0;

            friends.IdType ( IDT_ARMYTOTAL );
            friends.Location ( Location() );
            friends.adjustment=0;
 
            // all the strongholds here
            if ( !objStrongholds.Empty() ) {

                    auto stronghold = objStrongholds.First();

                    bool success = mx->TerrainById(mapsqr.terrain)->Success();

                    bool isFriend = stronghold->OccupyingRace() != RA_DOOMGUARD ;
#if defined(_DDR_)
                    isFriend = stronghold->IsFriend(ch_friend);
#endif
                
                    if ( !isFriend ) {
                        foe.adjustment = success ;
                        foe.armies++;
                        if ( stronghold->Type() == UT_WARRIORS )
                            foe.warriors+=stronghold->TotalTroops();
                        else if ( stronghold->Type() == UT_RIDERS )
                            foe.riders+=stronghold->TotalTroops();
                    } else {
                        friends.adjustment = success ;
                        friends.armies++;
                        if ( stronghold->Type() == UT_WARRIORS )
                            friends.warriors+=stronghold->TotalTroops();
                        else if ( stronghold->Type() == UT_RIDERS )
                            friends.riders+=stronghold->TotalTroops();
                    }

                    auto army = new mxarmy();
                    army->parent = stronghold ;
                    army->armytype = AT_STRONGHOLD ;
                    army->race = stronghold->OccupyingRace();
                    army->type = stronghold->Type();
                    army->total = stronghold->TotalTroops();
                    army->success = stronghold->BattleSuccess((mxlocinfo&)*this);
                    army->killed = 0;
                    army->loyalto = stronghold->Loyalty() ;
                    armies.push_back(army);

            }

            // all the regiments here
            FOR_EACH(reg, objRegiments) {

                if ( reg->Total() ) {
                
                    auto army = new mxarmy();
                    army->armytype = AT_REGIMENT ;
                    army->parent = reg ;
                    army->race = reg->Race();
                    army->type = reg->Type();
                    army->total = reg->Total();
                    army->success = reg->BattleSuccess( (mxlocinfo&)*this );
                    army->killed = 0;
                    army->loyalto = reg->Loyalty()->loyalty ;

                    if ( army->type == UT_WARRIORS ) {
                        foe.warriors+=army->total;
                        regiments.warriors+=army->total;
                    } else if ( army->type == UT_RIDERS ) {
                        foe.riders+=army->total;
                        regiments.riders+=army->total;
                    }
                    foe.armies++;
                    armies.push_back(army);
                }
            }

            // lets get all the armies belonging to the character
            FOR_EACH(c, objCharacters) {

                bool isFriend = true;
#if defined(_DDR_)
                isFriend = c->IsFriend(ch_friend);
#endif
                if ( isFriend ) {
                    friends.characters++;
                }else{
                    foe.characters++;
                }

                if ( c->warriors.Total() ) {
                    auto army = new mxarmy();
                    army->armytype = AT_CHARACTER ;
                    army->parent = c ;
                    army->race = c->Race();
                    army->type = UT_WARRIORS;
                    army->total = c->warriors.Total();
                    army->success = c->warriors.BattleSuccess ( (mxlocinfo&)*this, c );
                    army->killed = 0;
                    army->loyalto = c->NormalisedLoyalty() ;

                    if ( isFriend ) {
                        friends.armies++;
                        friends.warriors+=army->total;
                    }else{
                        foe.armies++;
                        foe.warriors+=army->total;
                    }
                    armies.push_back(army);
                }

                if ( c->riders.Total() ) {
                    auto army = new mxarmy();
                    army->armytype = AT_CHARACTER ;
                    army->parent = c;
                    army->race = c->Race();
                    army->type = UT_RIDERS;
                    army->total = c->riders.Total();
                    army->success = c->riders.BattleSuccess ( (mxlocinfo&)*this, c );
                    army->killed = 0;
                    army->loyalto = c->NormalisedLoyalty() ;

                    if ( isFriend ) {
                        friends.armies++;
                        friends.riders+=army->total;
                    }else{
                        foe.armies++;
                        foe.riders+=army->total;
                    }
                    armies.push_back(army);
                }

            }

            nArmies = (u32)armies.size();
        }

        mxarmytotal::mxarmytotal()
        {
            Clear();
            IdType( IDT_ARMYTOTAL );
        }

        mxarmytotal::~mxarmytotal()
        {
        }

    //}
    // namespace item
}
// namespace tme
