/*
 * FILE:    item_character.cpp
 * 
 * PROJECT: MidnightEngine
 * 
 * froupedD:
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
#include <memory>
#include <string>

#if defined(_DDR_)
#include "../scenarios/ddr/scenario_ddr_internal.h"
#endif

#define ENABLE_GROUPING 1

namespace tme {


        mxcharacter::mxcharacter() 
        {
            mxentity::type = IDT_CHARACTER ;
            //memory=NULL;
        }

        mxcharacter::~mxcharacter()
        {
        }

        void mxcharacter::Serialize ( archive& ar )
        {
            
            mxitem::Serialize(ar);
            if ( ar.IsStoring() ) {
                ar << longname;
                ar << shortname;
                WRITE_ENUM( looking );    
                WRITE_ENUM( time );
                ar << riders;
                ar << warriors;
                ar << battleloc;
                ar << battleslew;
                ar << reckless;
                ar << energy;
                ar << strength;
                ar << cowardess;
                ar << courage;
                ar << fear;
                ar << RecruitingKey;
                ar << RecruitedBy;
                WRITE_ENUM( race );
                ar << carrying ;
                ar << killedbyobject ;
                WRITE_ENUM( gender ) ;
                WRITE_ENUM( loyalty );
                ar << liege ;
                ar << foe ;
                WRITE_ENUM( wait );

                WRITE_ENUM(orders);
                ar << despondency;
                ar << traits ;
                
                ar << following ;
                ar << followers ;
                
            }else{
                ar >> longname;
                ar >> shortname;
                READ_ENUM ( looking );
                READ_ENUM ( time );
                ar >> riders;
                ar >> warriors;
                ar >> battleloc;
                ar >> battleslew;
                ar >> reckless;
                ar >> energy;
                ar >> strength;
                ar >> cowardess;
                ar >> courage;
                ar >> fear;
                ar >> RecruitingKey;
                ar >> RecruitedBy;
                READ_ENUM ( race );
                ar >> carrying ;
                ar >> killedbyobject ;
                READ_ENUM ( gender );
                READ_ENUM ( loyalty );
                ar >> liege ;
                ar >> foe ;
                READ_ENUM ( wait );

                READ_ENUM(orders) ;
                
                ar >> despondency;
                ar >> traits ;
            
                if ( tme::mx->SaveGameVersion() > 2 )
                    ar >> following ;
                else
                    following = NULL ;
             
                if ( tme::mx->SaveGameVersion() > 4 )
                    ar >> followers ;
                else
                    followers = 0 ;
                
                // temp bug fix
                if ( (s32)energy < 0 )
                    energy= 0;
            }

        }

        void mxcharacter::ForEachFollower(const std::function<void(mxcharacter*)> &callback)
        {
            if (!HasFollowers()) return;
            
            entities followers;
            mx->scenario->GetCharacterFollowers(this, followers);
            
            for ( u32 ii=0; ii<followers.Count(); ii++ ) {
                auto follower = static_cast<mxcharacter*>(followers[ii]);
                callback(follower);
            }
        }

        mxcharacter* mxcharacter::FindFollower(const std::function<bool(mxcharacter*)> &callback)
        {
            if (!HasFollowers()) return nullptr;
            
            entities followers;
            mx->scenario->GetCharacterFollowers(this, followers);
            
            for ( u32 ii=0; ii<followers.Count(); ii++ ) {
                auto follower = static_cast<mxcharacter*>(followers[ii]);
                if (callback(follower)) {
                    return follower;
                }
            }
            
            return nullptr;
        }

        void mxcharacter::RefreshLocationBasedVariables ( const mxlocinfo* info )
        {
            fear = info->adj_fear ;
            s32 temp  = cowardess-(fear/7);
            temp = std::max<int>( temp, 0 ) / 8 ;
            courage = std::min<int>( temp, 7 ) ;
        }

        void mxcharacter::ForcedVariableRefresh () const
        {
            // NOTE: This breaks the rules of this function being const
            // but i am only modifying temporary variables, so this should be okay
            mxcharacter* newcharacter = mx->CharacterById(Id());
            mxlocinfo* info = new mxlocinfo ( Location(), newcharacter, 0 );
            newcharacter->RefreshLocationBasedVariables ( info );
            SAFEDELETE ( info );
        }

        mxlocinfo* mxcharacter::GetLocInfo()
        {
        mxlocinfo* info;

            // create a new info class
            // for our current location and the direction
            // we are looking
            info = new mxlocinfo ( Location(), Looking(), this, (IsInTunnel() ? slf_tunnel : slf_none)  );

            //info->owner = (mxcharacter*)this ;

            if ( IsDead() )
                return info;

            // set the characters courage and fear
            RefreshLocationBasedVariables ( info );

            // if the character is hidden  then
            // nothing else is possible
            info->flags.Set(lif_hide); // = TRUE ;
            if ( IsHidden()  ) {
                info->flags.Reset(lif_enterbattle); // = FALSE ;
                return info;
            }

            // can we seek?
            info->flags.Set(lif_seek); // = TRUE ;

            // can we hide ?
            // 1. Only if we are allowed to an we have no armies
            // 2. TODO how about allowing hiding when armies
            // are below a certain number? - maybe warriors only
            if ( !IsAllowedHide() || HasArmy() || IsFollowing() || HasFollowers() )
                info->flags.Reset(lif_hide); // = FALSE ;

            // TODO Stop hiding when just entered into battle
            //if ( IsInBattle() ) 
            //    info->flags.Reset(lif_hide); ;

#if !defined(_DDR_)
            // if in battle there is nothing more we can do
            if ( info->foe.armies /*IsInBattle() */ )
                return info; 
#endif
            
            // make a list of all other recruitable
            // characters available
            // TODO in LOM this can only be one
            if ( mx->scenario->IsFeature(SF_APPROACH_DDR) ) {
                if(!mx->scenario->isLocationImpassable(info->loc_infront, info->owner)) {
                    info->FindApproachCharactersInfront(info->owner);
                }
            } else {
                info->FindRecruitCharactersHere (info->owner);
            }

            // can we recruit ?
            if ( info->objRecruit.Count() )
                info->flags.Set(lif_recruitchar); // = TRUE ;
            
            // can we recruitmen or guardmen ?
            // Recruit:
            // 1. only if there is a stronghold here
            // 2. Is the stronghold race the same as us?
            // 3. Are we allowed an army?
            // 4. does the stronghold have more than its minimum in?
            // 5. can we take any more soldiers?

            // guard:
            // 1. only if there is a stronghold here
            // 2. Is the stronghold race the same as us?
            // 3. does the stronghold have less than its maximum in?
            // 4. do we have enough soldiers of the required type?

            // TODO this is too restrictive, the stronghold::add routine
            // allows for arbitruary values to be added, but this
            // uses the LOM model of having to place 100 at a time
            // should this change?

            if ( info->objStrongholds.Count() ) {
                mxstronghold* stronghold = (mxstronghold*)info->objStrongholds[0] ;

#if defined(_LOM_)
                if ( stronghold->OccupyingRace() == Race() ) {
#endif
#if defined(_DDR_)
                    
                    if ( static_cast<ddr_stronghold*>(stronghold)->IsFriend(this) && stronghold->OccupyingRace() == Race() ) {
#endif
                    // can we recruit men ?
                    // this is not an exact check, a stronghold CAN have less than the minimum
                    // in, however you can no longer recruit after the min has been reached
                    if ( IsAllowedArmy() && stronghold->TotalTroops() > stronghold->MinTroops() ) {
#if defined(_DDR_)
                        if ( IsAllowedWarriors() )
#endif
                        if ( (stronghold->Type()==UT_WARRIORS) && (warriors.total + sv_character_recruit_amount <= sv_character_max_warriors )  )
                            info->flags.Set(lif_recruitmen) ;// = TRUE ;
                    
#if defined(_DDR_)
                        if ( IsAllowedRiders() )
#endif
                        if ( (stronghold->Type()==UT_RIDERS) && (riders.total + sv_character_recruit_amount <= sv_character_max_riders ) )
                            info->flags.Set(lif_recruitmen); // = TRUE ;
                    
                    }

                    // can we guard men ?
                    if ( stronghold->TotalTroops() + sv_character_guard_amount <= stronghold->MaxTroops() ) {
                        if ( (stronghold->Type()==UT_WARRIORS) && (warriors.total >= sv_character_guard_amount ) )
                            info->flags.Set(lif_guardmen); // = TRUE ;
                        if ( (stronghold->Type()==UT_RIDERS) && (riders.total >= sv_character_guard_amount ) )
                            info->flags.Set(lif_guardmen); // = TRUE ;
                    }

                }

            }

            // if the character is at night then
            // nothing else is possible
            //if ( IsNight() )
            //    return info;
            
            
            // use
            // 1. are we carrying our object
            // 2. have we used it
            
            // take
            // 1. is there an object here?
            
            // give
            // 1. do we have an object
            // 2. is there another character here
            // 3. are we generous
            
            // rest
            info->flags.Set(lif_rest);
            
            mxthing_t thing = (mxthing_t)info->mapsqr.object;
#if defined(_DDR_)
            if ( info->mapsqr.IsTunnelPassageway() && !IsInTunnel() )
                thing=OB_NONE;
#endif
            
            // can we fight ?
            // can we fight the object currently in this location?
            mxobject* obj = mx->ObjectById(thing);
            if ( CheckFightObject ( obj )  ) {
                info->flags.Set(lif_fight); // = TRUE ;
                info->fightthing = thing ;
            }

            
            // attack
            if ( info->flags.Is(lif_enterbattle) ) {
            // if a battle is allowed and we don't have any courage
            // then we can't actually enter the battle
                if ( IsCoward() )
                    info->flags.Reset(lif_enterbattle) ;// = FALSE ;
                
                if ( IsFollowing()  )
                    info->flags.Reset(lif_enterbattle) ;// = FALSE ;

                // if we have followers then all my followers
                // must have courage
                if ( HasFollowers() ) {
                    entities followers;
                    mx->scenario->GetCharacterFollowers(this, followers);
                    for ( u32 ii=0; ii<followers.Count(); ii++ ) {
                        mxcharacter* follower = (mxcharacter*) followers[ii];
                        if ( follower->IsCoward() ) {
                            info->stubborn_follower_battle=follower;
                            break;
                        }
                    }
                }
                
            }

            // if we are leading
            // then the whole group needs to be able to move
            if ( HasFollowers() ) {
                entities followers;
                mx->scenario->GetCharacterFollowers(this, followers);
                for ( u32 ii=0; ii<followers.Count(); ii++ ) {
                    mxcharacter* follower = (mxcharacter*) followers[ii];
                    if ( !follower->CanWalkForward() ) {
                        info->stubborn_follower_move=follower ;
                        break;
                    }
                }
            }
            
            return info ;

        }

        s32 mxcharacter::DistanceFromLoc ( mxgridref loc )
        {
            return Location() - loc ;
        }

        bool mxcharacter::CheckFightObject ( mxobject* obj ) const
        {
            return obj ? obj->CanFight() : FALSE ;
        }

        bool mxcharacter::CheckRecruitChar ( mxcharacter* character )  const
        {
            if ( character == this )
                return false ;

            if ( mx->scenario->IsFeature(SF_RECRUIT_DDR) ) {
                // TODO: recruiting logic
                return true ;
            }else{
                if(mx->isRuleEnabled(RF_LOM_UNRECRUITABLE_FEY)
                    && character->Race() == RA_FEY)
                {
                    return false;
                }
                if ( character->RecruitedBy & RecruitingKey )
                    return true ;
            }

            return false ;
        }

        void mxcharacter::LostFight ( void )
        {
        int temp;
            if ( !IsDead() ) {
                if ( IsRiding() ) {
                    if (mxrandom() & 1)
                        flags.Reset ( cf_riding );
                }

                temp = (energy/2) - 64 + reckless;
                if (mxrandom(255) >= temp) {
                    Cmd_Dead();
                }
            }
        }

        void mxcharacter::Displace ( void )
        {
        mxgridref    loc;

            if ( IsDead() )
                return;

            for (;; ) {
                loc = Location() + (mxdir_t)mxrandom(0, 7);
                
                if ( !mx->scenario->isLocationImpassable( loc, this ) ) {
                    location = loc ;
                    if ( IsRecruited() ) {
                        EnterLocation ( loc );
                        mx->scenario->LookInDirection ( Location(), Looking(), IsInTunnel() );
                    }
                    break;
                }
            }
        }
        

        void mxcharacter::SetLastCommand ( command_t cmd, mxid id )
        {
            lastcommand = cmd;
            lastcommandid = id;
        }

        void mxcharacter::CommandTakesTime ( bool success )
        {
            // ** CHEAT
            if ( sv_cheat_commands_free )
                return;
            
            mxcommand* command = mx->CommandById( GET_ID(lastcommand) );
            if ( command == NULL )
                return; // invalid command

            int timetaken = success ? command->successtime : command->failuretime;

            time = (mxtime_t) BSub(time,timetaken,0);
        }    
    
        MXRESULT mxcharacter::Cmd_MoveForward ( void )
        {
            std::unique_ptr<mxlocinfo> info;

            SetLastCommand ( CMD_MOVE, IDT_NONE);

            // lords who are following cannot be moved on their own
            if ( IsFollowing() )
                return MX_FAILED ;
            
            if ( !CanWalkForward() )
                return MX_FAILED ;
            
            // if we are leading
            // then the whole group needs to be able to move
            if ( HasFollowers() ) {
                entities followers;
                mx->scenario->GetCharacterFollowers(this, followers);
                for ( u32 ii=0; ii<followers.Count(); ii++ ) {
                    mxcharacter* follower = (mxcharacter*) followers[ii];
                    if ( !follower->CanWalkForward() )
                        return MX_FAILED ;
                }
            }
            
            
            // find where we will be moving to
            info.reset(GetLocInfo());
            
            // something blocking our path!
            if ( !info->flags.Is(lif_moveforward) ) {
                return MX_FAILED;
            }

#if defined(_LOM_)
            // if there is an army here
            // you can only move first thing
            if ( !sv_cheat_armies_noblock ) {
                if ( !IsDawn() && info->foe.armies ) {
                    return MX_FAILED;
                }
            }
            
            // something needs killing!
            if ( !sv_cheat_nasties_noblock ) {
                if ( info->flags.Is(lif_fight) ) {
                    return MX_FAILED;
                }
            }
#endif
            bool approach = false;
            if ( sv_auto_approach ) {
                 info->infront->FindRecruitCharactersHere(this);
                 approach = info->infront->objRecruit.Count() == 1;
            }
            
            return Cmd_WalkForward(sv_auto_seek, approach);
        }

        bool mxcharacter::CanWalkForward ( void )
        {
            // dead men don't walk!
            if ( IsDead() ) {
                return false;
            }
            
            // completely and untterly shattered?
            if ( energy <= sv_energy_cannot_continue ) {
                return false;
            }
            
            // hidden under a rock?
            if ( IsHidden() ) {
                // if auto unhide turned on then we must unhide and carry on
                if ( !sv_auto_unhide )
                    return false;
            }
            
            // should be sleeping!
            if ( IsNight() ) {
                return false;
            }

            return true;
        }
        
    
        MXRESULT mxcharacter::Cmd_WalkForward ( bool perform_seek, bool perform_approach )
        {
        s32            TimeCost;
        mxrace*        rinfo;

            // hidden under a rock?
            // if auto unhide turned on then we must unhide and carry on
            if ( IsHidden() &&  sv_auto_unhide ) {
                Cmd_UnHide();
            }
            
            rinfo = mx->RaceById(Race());

            // remove army from location
            mx->gamemap->SetLocationArmy(Location(),0);
            mx->gamemap->SetLocationCharacter(Location(),0);
            
            // set the current location to visited, just to make sure
            mx->gamemap->SetLocationVisited(location, TRUE);
            
            // and move us there
            location = location + Looking();

            // set the new current location to visited
            mx->gamemap->SetLocationVisited(location, TRUE);

            // add armies and characters back to the map
            // this is overkill!!!!
            mx->scenario->SetMapArmies();

            EnterLocation ( location );
            mx->scenario->LookInDirection ( Location(), Looking(), IsInTunnel() );
          
            // calculate our movement values
            TimeCost = rinfo->InitialMovementValue();

            // are we moving diagonally?
            if ( Looking()&1 )
                TimeCost += rinfo->DiagonalMovementModifier();

            // are we on horseback?
            if ( !IsRiding() )
                TimeCost = (s32) ((f64)TimeCost * rinfo->RidingMovementMultiplier()) ;

            // adjust for terrain
            TimeCost += rinfo->TerrainMovementModifier((mxterrain_t)mx->gamemap->GetAt ( location ).terrain);

            // check for max out
            TimeCost = std::min<int>( TimeCost, (int)rinfo->MovementMax() );

            // ** CHEATS
            if ( sv_cheat_movement_cheap )
                TimeCost=1;
            if ( sv_cheat_movement_free )
                TimeCost=0;
            // ** END CHEATS
            
            // time
            time = (mxtime_t) BSub(time,TimeCost,0);

            // energy
            DecreaseEnergy(TimeCost);

            CommandTakesTime(TRUE);
            
            // if we have moved, we are no longer in battle
            flags.Reset(cf_inbattle|cf_preparesbattle);
            
            // auto approach
            if ( perform_approach ) {
               // we need to recruit before anyone else joins us
                if ( Cmd_Approach() != nullptr) {
                    perform_seek = false;
                }
            }
            
            // auto seek
            if ( perform_seek ) {
                if ( mx->gamemap->getLocationObject(this, Location())!=OB_NONE ) {
                    Cmd_Seek();
                }
            }
            
            WalkFollowersForward();
            
            return MX_OK ;
        }

        void mxcharacter::WalkFollowersForward()
        {
            // if we are leading
            // then the whole group needs to be able to move
            if ( HasFollowers() ) {
                entities followers;
                mx->scenario->GetCharacterFollowers(this, followers);
                for ( u32 ii=0; ii<followers.Count(); ii++ ) {
                    auto follower = static_cast<mxcharacter*>(followers[ii]);
                    // must look the same direction to make them move in the
                    // correct direction
                    follower->looking = Looking();
                    follower->Cmd_WalkForward();
                }
            }
        }

        void mxcharacter::DecreaseEnergy ( s32 amount )
        {
            energy = BSub(energy,amount,0);
            riders.energy = BSub(riders.energy,amount,0);
            warriors.energy = BSub(warriors.energy,amount,0);
        }


        void mxcharacter::IncreaseEnergy ( s32 amount )
        {
        mxrace* rinfo = mx->RaceById(Race());

            s32 baserest = rinfo->BaseRestAmount() ;

            energy = 
                BAdd ( energy, amount + baserest, (u32)sv_character_max_energy );

            riders.energy = 
                BAdd ( riders.energy, amount + riders.BaseRestModifier(), (u32)sv_riders_max_energy );

            warriors.energy = 
                BAdd ( warriors.energy, amount  + warriors.BaseRestModifier(), (u32)sv_warriors_max_energy );

        }

        MXRESULT mxcharacter::Cmd_LookLeft ( void )
        { 
            //looking=(mxdir_t) ((looking-1)&7); 
            looking = PREV_DIRECTION(looking);
            mx->scenario->LookInDirection ( Location(), Looking(), IsInTunnel() );
#if defined(_DDR_)
            mx->scenario->MakeMapAreaVisible(Location(), this);
#endif

            return MX_OK ;
        }

        MXRESULT mxcharacter::Cmd_LookRight ( void )
        { 
            //looking=(mxdir_t)((looking+1)&7); 
            looking = NEXT_DIRECTION(looking);
            mx->scenario->LookInDirection ( Location(), Looking(), IsInTunnel() );
#if defined(_DDR_)
            mx->scenario->MakeMapAreaVisible(Location(), this);
#endif

            return MX_OK ;
        }

        MXRESULT mxcharacter::Cmd_LookDir ( mxdir_t dir )
        { 
            looking=(mxdir_t)((dir)&7); 
            mx->scenario->LookInDirection ( Location(), Looking(), IsInTunnel() );
#if defined(_DDR_)
            mx->scenario->MakeMapAreaVisible(Location(), this);
#endif
            return MX_OK ;
        }

        bool mxcharacter::EnterLocation ( mxgridref loc )
        {
            mx->scenario->MakeMapAreaVisible ( loc, this );

            return TRUE ;
        }

        MXRESULT mxcharacter::Cmd_Lookat(mxgridref loc)
        {
            return Cmd_LookDir ( location.DirFromHere(loc) );
        }

        MXRESULT mxcharacter::Cmd_Place(mxgridref loc)
        {
            Location(loc);
            EnterLocation( Location() );
            return MX_OK ;
        }
        
        MXRESULT mxcharacter::Cmd_Rest ( void )
        { 
            SetLastCommand ( CMD_REST, IDT_NONE );

            if ( IsNight() || IsResting() )
                return MX_FAILED ;

            flags.Set(cf_resting);
            
           entities followers;
            mx->scenario->GetCharacterFollowers(this, followers);
            for ( u32 ii=0; ii<followers.Count(); ii++ ) {
                mxcharacter* follower = (mxcharacter*) followers[ii];
                follower->Cmd_Rest();
            }

            CommandTakesTime(TRUE);

            return MX_OK ;
        }

        MXRESULT mxcharacter::Cmd_RecruitMen ( mxstronghold* stronghold, u32 qty )
        {
        s32                    delta;
        mxstronghold*        def_stronghold = NULL ;

            SetLastCommand ( CMD_RECRUITMEN, IDT_NONE );

            // get location info
            std::unique_ptr<mxlocinfo> info ( GetLocInfo() );
        
            if ( info->objStrongholds.Count() )
                    def_stronghold = (mxstronghold*)info->objStrongholds[0] ;

            // are we allowed to recruitmen ?
            if ( !info->flags.Is(lif_recruitmen) ) {
                return MX_FAILED ;
            }

            // get the default stronghold
            if ( stronghold == NULL ) {
                stronghold = def_stronghold ;
                if ( stronghold == NULL )
                    return MX_FAILED ;
            }

            if ( qty == 0 )
                qty = (u32)sv_character_recruit_amount;

            // try and recruit upto the RECRUIT_AMOUNT
            if ( stronghold->Type() == UT_RIDERS ) {
                delta = (s32)sv_character_max_riders - riders.total ;
                qty = stronghold->Remove( Race(), UT_RIDERS, std::min<int>((s32)qty,delta) );
                riders.total += qty;
            } else if ( stronghold->Type() == UT_WARRIORS ) {
                delta = (s32)sv_character_max_warriors - warriors.total ;
                qty = stronghold->Remove( Race(), UT_WARRIORS, std::min<int>((s32)qty,delta) );
                warriors.total += qty;
            }

            mx->SetLastActionMsg(mx->text->DescribeCharacterRecruitMen( this, stronghold, qty ));

            CommandTakesTime(TRUE);

            return MX_OK ;
        }

        MXRESULT mxcharacter::Cmd_PostMen ( mxstronghold* stronghold, u32 qty )
        {
        mxstronghold*    def_stronghold = NULL ;

            SetLastCommand ( CMD_POSTMEN, IDT_NONE );

            // get location info
            std::unique_ptr<mxlocinfo> info ( GetLocInfo() );
        
            if ( info->objStrongholds.Count() )
                    def_stronghold = (mxstronghold*)info->objStrongholds[0] ;

            // are we allowed to postmen ?
            if ( !info->flags.Is(lif_guardmen) ) {
                return MX_FAILED ;
            }

            // get the default stronghold
            if ( stronghold == NULL ) {
                stronghold = def_stronghold ;
                if ( stronghold == NULL )
                    return MX_FAILED ;
            }

            if ( qty == 0 )
                qty = (u32)sv_character_guard_amount;
            // try and guard upto the GUARD_AMOUNT
            if ( stronghold->Type() == UT_RIDERS )
                riders.total -= stronghold->Add( Race(), UT_RIDERS, std::min<int>(riders.total,qty) );
            else if ( stronghold->Type() == UT_WARRIORS )
                warriors.total -= stronghold->Add( Race(), UT_WARRIORS, std::min<int>(warriors.total,qty));


            mx->SetLastActionMsg(mx->text->DescribeCharacterPostMen( this, stronghold, qty ));
            
            CommandTakesTime(TRUE);

            return MX_OK ;
        }

        mxcharacter* mxcharacter::Cmd_Approach ( mxcharacter* character )
        {
            SetLastCommand ( CMD_APPROACH, IDT_NONE );

            if ( character == nullptr ) {

                // get location info
                std::unique_ptr<mxlocinfo> info ( GetLocInfo() );
        
                // are we allowed to approach ?
                if ( !info->flags.Is(lif_recruitchar) ) {
                    return nullptr ;
                }

                character = static_cast<mxcharacter*>(info->objRecruit.First()) ;
            }

            RETURN_IF_NULL(character) nullptr ;

            if ( character->Recruited ( this ) ) {
                SetLastCommand ( CMD_APPROACH, mxentity::SafeIdt(character) );
                character->SetLastCommand ( CMD_APPROACHED, mxentity::SafeIdt(this) );
                CommandTakesTime(true);
                return character ;
            }

            CommandTakesTime(false);
            
            return nullptr ;
        }

#if defined(_DDR_)
        bool mxcharacter::Recruited ( mxcharacter* c )
        {
            if ( !c->IsAIControlled() ) {
                // set loyalty?
                flags.Set ( cf_recruited );
                flags.Reset(cf_ai);
            }else{
                flags.Reset(cf_recruited);
                flags.Set(cf_ai);
            }

            // 1. set time of day to recruiting character
            if ( mx->scenario->IsFeature(SF_RECRUIT_TIME) )
                time = c->Time() ;

            // 2. our liege becomes the recruiting character
            if ( c == this ) {
                MXTRACE("Recruited: Error Setting Liege to self! %s", Longname().c_str());
            }
            liege = c; //->Liege() ;
            
            // 3. our loyalty race becomes recruiting character loyalty race
            loyalty = c->NormalisedLoyalty() ;
            
            // 4. get the my foe's loyalty, if they are loyal to the recruiting char's loyalty
            // then we need the recruiting characters foe
            // 
            if ( foe->NormalisedLoyalty() == loyalty ) {
                foe = c->Foe() ;
            }

            return TRUE ;
        }
            
        MXRESULT mxcharacter::EnterBattle ( void )
        {
            Flags().Set(cf_preparesbattle); // force the battle flag early!
            
            time = sv_time_night;
            
            if ( HasFollowers() ) {
                // we need to do a merge
                
                entities followers;
                mx->scenario->GetCharacterFollowers(this, followers);
                
                for ( u32 ii=0; ii<followers.Count(); ii++ ) {
                    mxcharacter* follower = (mxcharacter*) followers[ii];
                    follower->EnterBattle();
                }
                
            }
            
            return MX_OK;
        }
            
#endif
            
#if defined(_LOM_)
        bool mxcharacter::Recruited ( mxcharacter* c )
        {
            // set loyalty?
            flags.Set ( cf_recruited );
            
            // 1. set time of day to recruiting character
            if ( mx->scenario->IsFeature(SF_RECRUIT_TIME) )
                time = c->Time() ;
            
            // 2. our liege becomes the recruiting character
            liege = c->Liege() ;
            // 3. our loyalty race becomes recruiting character loyalty race
            loyalty = c->NormalisedLoyalty() ;
            // 4. get the my foe's loyalty, if they are loyal to the recruiting char's loyalty
            // then we need the recruiting characters foe
            // 
            mxcharacter* foe = Foe();
            if ( foe->NormalisedLoyalty() == loyalty ) {
                foe = c->Foe() ;
            }
            
            return TRUE ;
        }
            
        MXRESULT mxcharacter::EnterBattle ( void )
        {
            Flags().Set(cf_inbattle); // force the battle flag early!
            
            return MX_OK;
        }
#endif

        MXRESULT mxcharacter::Cmd_Attack ( void )
        {
            if ( IsFollowing() )
                return MX_FAILED ;
            
            std::unique_ptr<mxlocinfo> info ( GetLocInfo() );
        
            bool battle = info->flags.Is(lif_enterbattle) ;
            mxcharacter* stubborn_follower = info->stubborn_follower_battle;
            if ( !battle || stubborn_follower )
                return MX_FAILED;
            
            SetLastCommand(CMD_ATTACK,IDT_NONE);
            MXRESULT result = Cmd_WalkForward(false) ;
            if ( result == MX_OK ) {
                EnterBattle();
            }
            return result;
        }

        mxobject* mxcharacter::Cmd_Fight ( void )
        {
        bool    objectautokill = false ;
        int        friends_armies=0;

            SetLastCommand ( CMD_FIGHT, IDT_NONE );

            std::unique_ptr<mxlocinfo> info ( GetLocInfo() );
            
            auto fightobject = mx->ObjectById(info->fightthing) ;
            friends_armies = info->friends.armies ;

            // are we allowed to fight
            if ( !info->flags.Is(lif_fight) )
                fightobject = nullptr ;

            // is there anything to fight
            if ( fightobject == nullptr )
                return nullptr ;

            SetLastCommand ( CMD_FIGHT,SafeIdt(fightobject)) ;

            auto oinfo = Carrying();
            objectautokill = oinfo ? oinfo->CanDestroy(fightobject) : false ;
            
            // if there is any friends here
            // then we win by default
            if (  friends_armies == 0 && !objectautokill && !sv_cheat_always_win_fight ) {

                LostFight();

                if ( IsDead() ) {
                    killedbyobject = fightobject ;
                    return fightobject;
                }

            }

            // we have killed the enemy
            //
            mx->text->oinfo = fightobject;

            // describe that
            u32 message =  objectautokill ? oinfo->usedescription : SS_FIGHT ;

            mx->SetLastActionMsg(mx->text->CookedSystemString(message, this));

            CommandTakesTime(true);

            // this does an auto write to the map!
            mx->gamemap->GetAt( Location() ).RemoveObject();

            return fightobject ;
        }

        MXRESULT mxcharacter::Cmd_Hide ( void )
        {
            SetLastCommand ( CMD_HIDE, IDT_NONE );

            if ( !IsAllowedHide() )
                return MX_FAILED;

            if ( IsHidden() )
                return MX_FAILED;

            flags.Set ( cf_hidden );
            CommandTakesTime(TRUE);
            return MX_OK ;
        }

        MXRESULT mxcharacter::Cmd_UnHide ( void )
        {
            SetLastCommand ( CMD_UNHIDE, IDT_NONE );

            if ( !IsHidden() )
                return MX_FAILED;
            flags.Reset ( cf_hidden );
            CommandTakesTime(TRUE);
            return MX_OK ;
        }

        mxthing_t mxcharacter::LocationThing() const
        {
            mxloc& mapsqr = mx->gamemap->GetAt ( Location() );
            mxthing_t thing = (mxthing_t)mapsqr.object;
            if ( thing == OB_NONE ) return thing;
            mxobject* oinfo = mx->ObjectById ( thing );
            return  oinfo->CanSee() ? thing : OB_NONE ;
        }

        MXRESULT mxcharacter::Cmd_Follow ( mxcharacter* c )
        {
            if ( !CanFollow(c) )
                return MX_FAILED ;
            
            //following = (mxcharacter*)c ;

            if ( HasFollowers() ) {
                // we need to do a merge
 
                entities followers;
                mx->scenario->GetCharacterFollowers(this, followers);
                
                for ( u32 ii=0; ii<followers.Count(); ii++ ) {
                    mxcharacter* follower = (mxcharacter*) followers[ii];
                    RemoveFollower(follower);
                    c->AddFollower(follower);
                }
                
            }
            
            c->AddFollower( this );
            
            CommandTakesTime(TRUE);
            
            return MX_OK ;
        }
    
    
        MXRESULT mxcharacter::Cmd_UnFollow ( mxcharacter* c )
        {
            
            // are we following someone?
            if ( Following() != c )
                return MX_FAILED ;
            
            following->RemoveFollower( this );
            
            CommandTakesTime(TRUE);
            
            return MX_OK ;
        }
    
        bool mxcharacter::AddFollower ( mxcharacter* character )
        {
            if ( character->Following() != NULL )
                return FALSE;
            followers+=1;
            if ( followers > 0 )
                flags.Set(cf_followers);
            character->following=this;
            return TRUE;
        }

        bool mxcharacter::RemoveFollower ( mxcharacter* character )
        {
            if ( character->Following() != this )
                return FALSE;
            followers-=1;
            if ( followers == 0 )
                flags.Reset(cf_followers);
            
            character->following = NULL ;
            
            return TRUE;
        }
    
    
        MXRESULT mxcharacter::Cmd_DisbandGroup ( void )
        {
            if ( !HasFollowers() )
                return MX_FAILED;
            
            entities followers;
            mx->scenario->GetCharacterFollowers(this, followers);
            for ( u32 ii=0; ii<followers.Count(); ii++ ) {
                mxcharacter* follower = (mxcharacter*) followers[ii];
                RemoveFollower(follower);
            }
            
            CommandTakesTime(TRUE);
            
            return MX_OK;
        }
    
        MXRESULT mxcharacter::Cmd_SwapGroupLeader ( mxcharacter* character )
        {
            if ( !HasFollowers() )
                return MX_FAILED;
            
            entities followers;
            mx->scenario->GetCharacterFollowers(this, followers);
            
            // remove them all first
            for ( u32 ii=0; ii<followers.Count(); ii++ ) {
                mxcharacter* follower = (mxcharacter*) followers[ii];
                RemoveFollower(follower);
            }
            
            // then add them back in the same order
            // with the old leader taking the same slot as the new leader
            for ( u32 ii=0; ii<followers.Count(); ii++ ) {
                
                mxcharacter* follower = (mxcharacter*) followers[ii];
               
                if ( character == follower ) {
                    // I join the group
                    character->AddFollower(this);
                }else{
                    character->AddFollower(follower);
                }
                
            }
            
            CommandTakesTime(TRUE);
            
            return MX_OK;
        }

        bool mxcharacter::CanFollow( const mxcharacter* c ) const
        {
#ifdef ENABLE_GROUPING
            if ( c == NULL )
                return MX_FAILED ;
            
            if ( IsDead() || c->IsDead() )
                return FALSE;
            
            // if we are following someone, then we must unfollow
            // if they are following someone then they must unfollow
            //if ( IsFollowing() || c->IsFollowing() )
            //    return FALSE ;
            
            //if ( HasFollowers() )
            //    return FALSE;
            
            if ( IsHidden() || c->IsHidden() )
                return FALSE;
            
            if ( c->HasFollowers() && c->followers>=MAX_CHARACTERS_FOLLOWING)
                return FALSE;
            
            // problems because of const!!!!
            mxgridref l = c->Location();
            mxgridref l2 = Location();
            
            // we must be at the same location as the character
            if ( l != l2 )
                return FALSE ;
            
#if defined(_DDR_)
            // they both need to be in or out of a tunnel
            if ( c->IsInTunnel() != IsInTunnel() )
                return FALSE;
#endif
            
            
            // we must be loyal to the same character
            if ( !IsFriend(c) )
                return FALSE ;
            
            return TRUE ;
#else
            return FALSE ;
#endif
        }

        mxobject* mxcharacter::Cmd_Seek ( void )
        {
        mxthing_t    newobject;
        mxobject*    oinfo;

            bool removeObject = true;
            bool mikeseek=FALSE;
            SetLastCommand ( CMD_SEEK, IDT_NONE );
            CommandTakesTime(TRUE);

            mxloc& mapsqr = mx->gamemap->GetAt ( Location() );
            
#if defined(_DDR_)
            newobject = mx->gamemap->getLocationObject(this, Location());
            //if ( newobject == OB_NONE && location.x == 24 && location.y == 58 ) {
            //    mikeseek=TRUE;
            //    newobject=OB_GUIDANCE;
            //}
#endif
#if defined(_LOM_)
            newobject = (mxthing_t)mapsqr.object ;
            if ( location.x == 4 && location.y == 10 ) {
                mikeseek=TRUE;
                newobject=OB_GUIDANCE;
            }
#endif
            
        top:
            oinfo = mx->ObjectById ( newobject );

            if ( oinfo == nullptr ) {
                mx->SetLastActionMsg(mx->text->CookedSystemString(SS_SEEK_NOTHING, this));
                return oinfo;
            }

            //
            mx->text->oinfo = oinfo ;
            bool reset_msg=false;

#if defined(_DDR_)
            if ( newobject == OB_GUIDANCE )
                mx->SetLastActionMsg(mx->text->CookedSystemString(SS_GUIDANCE1, this));
            else
#endif
                mx->SetLastActionMsg(mx->text->CookedSystemString(SS_SEEK, this));
            
            switch ( newobject ) {

                case OB_WILDHORSES:
                    if ( IsAllowedHorse() ) {
                        flags.Set ( cf_riding );
                        //
                    }else{
                        // display think
                    }
                    break;

                case OB_SHELTER:
                    IncreaseEnergy( (s32)sv_object_energy_shelter );
                    break;

#if defined(_DDR_)
                case OB_CLAWS:
                    time = sv_time_night;
                    reset_msg=true;
                    break;
                case OB_FLAMES:
                    time = sv_time_dawn;
                    reset_msg=true;
                    break;
                case OB_THORNS:
                    despondency=0;
                    reset_msg=true;
                    break;
                case OB_BLOOD:
                    despondency=MAX_DESPONDENCY;
                    reset_msg=true;
                    break;
                case OB_LANGUOR:
                    energy=sv_object_energy_shadowsofdeath;
                    reset_msg=true;
                    break;
                case OB_SPRINGS:
                    energy=sv_object_energy_watersoflife;
                    reset_msg=true;
                    break;
                
#endif
                    
                case OB_GUIDANCE:
                    mx->scenario->GiveGuidance(this, (mikeseek?1:0) );
                    break;

                    
                case OB_SHADOWSOFDEATH:
                    energy = (u32)sv_object_energy_shadowsofdeath;
                    warriors.energy = (u32)sv_object_energy_shadowsofdeath;
                    riders.energy = (u32)sv_object_energy_shadowsofdeath;
                    reset_msg=true;
                    break;

                case OB_WATERSOFLIFE:
                    energy = (u32)sv_object_energy_watersoflife;
                    warriors.energy = (u32)sv_object_energy_watersoflife;
                    riders.energy = (u32)sv_object_energy_watersoflife;
                    reset_msg=true;
                    break;

                case OB_HANDOFDARK:
                    time = sv_time_night;
                    reset_msg=true;
                    break;

                case OB_CUPOFDREAMS:
                    time = sv_time_dawn;
                    reset_msg=true;
                    break;

                case OB_ICECROWN:
                    if ( IsAllowedIcecrown() ) {
                        Cmd_PickupObject();
                    }else{
                        newobject = OB_NONE ;
                        goto top;
                    }
                    break;

                case OB_MOONRING:
                    if ( IsAllowedMoonring() ) {
                        Cmd_PickupObject();
                    }else{
                        newobject = OB_NONE ;
                        goto top;
                    }
                    break;

                default:

                    if ( oinfo->CanFight() ) {
                        // auto fight?
#if defined(_DDR_)
                        if ( !sv_cheat_nasties_noblock ) {
                            Cmd_Fight();
                        } else {
                            removeObject = false;
                        }
#endif  
                    } else

                    // if we can pickup the new object
                    if ( oinfo->CanPickup() ) {

                        // we can only pickup a new object if can we drop our current object
                        if ( carrying && !carrying->CanDrop() ) {
                            newobject = OB_NONE ;
                            goto top;
                        }

                        Cmd_PickupObject();
                        
                        // we might have dropped an object
                        // and the pickup will have removed the object
                        
                        oinfo=NULL;
                    }
                    break;

            }

            
            
#if defined(_LOM_)
            if ( oinfo && oinfo->MapRemove() && removeObject ) {
                mapsqr.RemoveObject();
            }
#endif
#if defined(_DDR_)
            if ( reset_msg ) {
                mx->text->oinfo = oinfo ;
                mx->SetLastActionMsg( mx->text->CookedSystemString( SS_SEEK, this) );
            }
            
            if ( removeObject )
                mapsqr.RemoveObject();
#endif

            SetLastCommand ( CMD_SEEK, MAKE_ID(IDT_OBJECT,newobject) );

            return mx->ObjectById(newobject) ;
        }

        MXRESULT mxcharacter::Cmd_DropObject ( void )
        {
            SetLastCommand ( CMD_DROPOBJECT, mxentity::SafeIdt(carrying) );

            mx->scenario->DropObject ( Location(), carrying );
#if defined(_DDR_)
            carrying->carriedby=NULL;
#endif
            carrying=NULL;
            
            return MX_OK ;
        }

        mxobject* mxcharacter::Cmd_PickupObject ( void )
        {
        mxobject* oldobject=carrying;

            carrying = mx->scenario->PickupObject( Location() ) ;
            if(carrying!=nullptr) {
                MXTRACE("%s picks up object %s", Longname().c_str(), carrying->name.c_str());
            }

#if defined(_DDR_)
            if ( carrying==nullptr) {
                carrying = oldobject;
                return NULL;
            }
            
            carrying->carriedby=this;
            
            if ( oldobject ) {
                MXTRACE("%s drops object %s", Longname().c_str(), oldobject->name.c_str());
                mx->scenario->DropObject ( Location(), oldobject );
                oldobject->carriedby=nullptr;
            }
#endif
            
#if defined(_LOM_)
            if( oldobject ) {
                MXTRACE("%s drops object %s", Longname().c_str(), oldobject->name.c_str());
                mx->scenario->DropObject ( Location(), oldobject );
            }
#endif
            

            SetLastCommand ( CMD_TAKEOBJECT, mxentity::SafeIdt(carrying) );

            return carrying ;
        }
            
        void mxcharacter::StartDawn ( void )
        {
            IncreaseEnergy( Time()/2 );

            time = sv_time_dawn; 
            flags.Reset ( cf_resting );

            if ( !IsDead() ) {
#if defined(_DDR_)
                flags.Reset ( cf_inbattle|cf_wonbattle|cf_preparesbattle);
#endif
#if defined(_LOM_)
                flags.Reset ( cf_inbattle|cf_wonbattle );
#endif
                battleloc= mxgridref(-1,-1);
                battleslew = 0;
                riders.lost = 0;
                riders.slew = 0;
                warriors.lost = 0;
                warriors.slew = 0;
            }

            // reset the waiting mode
            if ( WaitMode() == WM_OVERNIGHT )
                Cmd_Wait(WM_NONE);
        }

        void mxcharacter::Cmd_Dead ( void )
        {
            
            // TODO we need to check if there is an object
            // we don't want to lose in the location underneath us
            if ( carrying && carrying->IsUnique() )
                Cmd_DropObject();

            flags.Reset(cf_alive);

            mx->scenario->CheckMoonringWearerDead();
#if defined(_DDR_)
            mx->scenario->ResetMapArmies();
#endif
        }

        bool mxcharacter::HasBattleInfo() const
        {
            mxgridref loc1 = battleloc ;
            mxgridref loc2 = mxgridref(-1,-1);
            return (loc1 !=  loc2);
        }

        void mxcharacter::CanSeeLocation ( mxgridref loc )
        {
        mxstronghold*    stronghold;
        collections::entities    objects;
        int count;

            mxloc& mapsqr = mx->gamemap->GetAt( loc );

            count=0;

            mx->scenario->ClearFromMemory(loc);

            if ( mapsqr.HasArmy() ) {
                
                if ( (count = mx->CollectRegiments ( loc, objects )) )
                    memory.Memorise ( (mxregiment*)objects[0], RA_DOOMGUARD );


                if ( mapsqr.IsStronghold() ) {
                    mx->CollectStrongholds ( loc, objects );
                    if ( objects.Count() ) {
                        stronghold = (mxstronghold*)objects[0];
                        if ( stronghold->OccupyingRace() == RA_DOOMGUARD )
                            memory.Memorise ( stronghold, RA_DOOMGUARD );
                        else 
                            memory.Memorise ( stronghold, RA_FREE );
                    }
                }
            }

        }


        archive& operator<<(archive& ar, mxcharacter* ptr )
        {
            return ar << (u32) mxentity::SafeId(ptr);
        }

        archive& operator>>( archive& ar, mxcharacter*& ptr )
        {
        int temp;
            ar >> temp ; ptr = mx->CharacterById(temp);
            return ar ;
        }

        mxrace_t mxcharacter::NormalisedLoyalty() const
        {
            return loyalty == RA_NONE ? Race() : loyalty ;
        }

        bool mxcharacter::IsFriend( const mxcharacter* c ) const
        {
            if ( c == nullptr ) return false ;
            return NormalisedLoyalty() == c->NormalisedLoyalty() ;
        }

        const std::string& mxcharacter::HeOrShe() const
        { 
            return mx->GenderById ( gender )->pronoun1; 
        }

        const std::string& mxcharacter::HisOrHer() const
        { 
            return mx->GenderById ( gender )->pronoun2; 
        }

        const std::string& mxcharacter::HimOrHer() const
        { 
            return mx->GenderById ( gender )->pronoun3; 
        }

        MXRESULT mxcharacter::Cmd_Wait ( mxwait_t mode )
        {
            SetLastCommand ( CMD_WAIT, IDT_NONE );

            wait = mode ; 
            return MX_OK;
        }

        MXRESULT mxcharacter::FillExportData ( info_t* data )
        {
            // check if we are being asked to export a character
            // army
            if ( (u32)ID_TYPE(data->id) == (u32)INFO_ARMY ) {
                defaultexport::army_t* out = (defaultexport::army_t*)data;
                out->parent = SafeIdt(this);
                out->race = Race();
                out->type = AT_CHARACTER ;

                mxunit* unit = out->unit==UT_WARRIORS ? (mxunit*)&warriors : (mxunit*)&riders ;
                out->total = unit->Total() ;
                out->energy = unit->Energy();
                out->lost = unit->Lost();
                out->killed = unit->Killed();
                return MX_OK ;
            }

            defaultexport::character_t* out = (defaultexport::character_t*)data;
            VALIDATE_INFO_BLOCK(out,INFO_CHARACTER,defaultexport::character_t);

            out->looking = looking;
            out->time = Time();
            out->longname = std::string(longname);
            out->shortname = std::string(shortname);


#if defined(_LOM_)
            out->warriors = MAKE_ARMYID(UT_WARRIORS,SafeId(this));
            out->riders = MAKE_ARMYID(UT_RIDERS,SafeId(this));
#endif
#if defined(_DDR_)
            out->warriors =  MAKE_ARMYID((128|UT_WARRIORS),SafeId(this)) ;
            out->riders = MAKE_ARMYID((128|UT_RIDERS),SafeId(this)) ;
#endif
            
            
            out->battle.location.x = battleloc.x;
            out->battle.location.y = battleloc.y;
            out->battle.killed = battleslew;

            out->reckless = reckless;
            out->energy = energy;
            out->strength = strength;
            out->cowardess = cowardess;
            out->courage = courage;
            out->fear = fear;
            
            out->loyalty = loyalty;
            out->foe = SafeIdt(foe);
            out->liege = SafeIdt(liege);

            out->traits = traits;
            out->recruit.group = RecruitingKey;
            out->recruit.by = RecruitedBy;

            out->race = race;
            out->object = SafeIdt(carrying);
            out->killedby = SafeIdt(killedbyobject);
            out->gender = gender;
            out->wait = wait;
            
            out->following = SafeIdt(following);
            out->followers = followers ;
            
            out->lastcommand = lastcommand ;
            out->lastcommandid = lastcommandid ;

            return mxitem::FillExportData ( data );
        }



}
// namespace tme
