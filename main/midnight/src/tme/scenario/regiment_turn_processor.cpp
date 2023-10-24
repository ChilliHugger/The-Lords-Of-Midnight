//
//  regiment_turn_processor.cpp
//  midnight
//
//  Created by Chris Wild on 06/09/2023.
//

#include "../baseinc/regiment_turn_processor.h"
#include "../baseinc/tme_internal.h"
#include "../scenario/helpers/race_terrain_movement_modifier.h"

namespace tme {


    RegimentTurnProcessor::RegimentTurnProcessor() :
        regiment(nullptr)
    {
    }

    void RegimentTurnProcessor::Process ( mxregiment* regiment )
    {
        RETURN_IF_NULL(regiment);
    
        this->regiment = regiment;
    
        regiment->LastLocation( regiment->Location() );
        turns = (u32)sv_regiment_default_moves;
        while ( turns>0 )
            Next();
    }

    bool RegimentTurnProcessor::LookForInterestingLocationNearby()
    {
        // do a complete scan around us and see if there
        // is anything more interesting to do than our current orders
        // ie: someone else is in the next location
        if ( !regiment->IsFlags(rf_direct) ) {
            for (mxdir_t dir=DR_NORTH; dir<=DR_NORTHWEST; dir=(mxdir_t)((int)dir+1))    {
                targetlocation = regiment->Location()+dir;
                if ( mx->gamemap->IsLocationSpecial(targetlocation)) {
                    return true;
                }
            }
        }
        return false;
    }

    void RegimentTurnProcessor::Next ( void )
    {
    s32     totaldoomdarkarmies;
    s32     movement;

        // check that this regiment isn't dead already
        if ( regiment->Total() == 0) {
            // terminate the movement
            Complete();
            return;
        }

        // check that this regiment isn't on hold...
        if ( regiment->Orders() == OD_HOLD ) {
            // terminate the movement
            Complete();
            return;
        }

        // let this army hang around a little
        //if ( regiment->Orders() == OD_DELAYED_WANDER ) {
            if ( regiment->Delay() ) {
                regiment->Delay( regiment->Delay() -1 );
                Complete();
                return;
            }//else
             //   Orders(OD_WANDER);
        //}

        // if there is something about this location then
        // don't go any futher
        if ( mx->gamemap->IsLocationSpecial(regiment->Location()) ) {
            Complete();
            return;
        }

        //
        // If we have not found a special location
        // then we are going to fall back on our orders
        //
        if ( !LookForInterestingLocationNearby() ) {
            // now check what we must do
            switch ( regiment->Orders() ) {
                case OD_NONE:
                    Complete();
                    return;

                case OD_GOTO:
                    Goto();
                    break;

                case OD_WANDER:
                    Wander();
                    break;

                case OD_FOLLOW:
                    Follow();
                    break;

                case OD_ROUTE:
                    Route();
                    break;
                    
                default:
                    break;
            }

            if ( regiment->Orders() != OD_WANDER ) {
                // lets see if we are actually at the location
                // we are trying to get to
                if ( regiment->Location() == targetlocation ) {
                    Complete();
                    return;
                }

                // where must we walkto to move in the direction we wish
                // to go?
                if ( !Retarget() ) {
                    Complete();
                    return;
                }
            }
        }
        
        // impassable mountains
        if( mx->scenario->isLocationImpassable(targetlocation, regiment) ) {
            Complete();
            return;
        }
        
        // get the location we want
        flags32_t f = regiment->IsInTunnel() ? slf_tunnel : slf_none ;
        
        // TODO: This is an expensive check - make cheaper
        auto info = new mxlocinfo ( targetlocation, nullptr, f );
        totaldoomdarkarmies = info->foe.armies ;
        SAFEDELETE(info);

        // we can't move into the location if it is full
        // or our armies
        if ( totaldoomdarkarmies >= MAX_FOEARMIES_INLOCATION ) {
            Complete();
            return;
        }
        
        // calculate our movement values
        auto rinfo = mx->RaceById(regiment->Race());
        
        movement = rinfo->InitialMovementValue();
        
        // are we moving diagonally?
        if ( IS_DIAGONAL(regiment->Location().DirFromHere ( targetlocation )) )
            movement += rinfo->DiagonalMovementModifier();
        
        // adjust for terrain
        movement += TME::helpers::RaceTerrainMovementModifier::Get(
            regiment->Race(),
            (mxterrain_t)mx->gamemap->GetAt ( regiment->Location() ).terrain
            );
        
        // are we on horseback?
        if ( regiment->Type()!=UT_RIDERS )
            movement = (int) ((f64)movement * rinfo->RidingMovementMultiplier()) ;
        
        // check for max out
        movement = std::min<int>( movement, (s32)rinfo->MovementMax() );
        
        s32 temp = turns;
        temp -= movement;
        turns = std::max<int>(0,temp);
        
        regiment->Location( targetlocation );

        // add us into the location we have just occupied
        //mx->gamemap->SetLocationArmy(Location(),1);
    }

    void RegimentTurnProcessor::Goto ( void )
    {
        auto item = static_cast<mxitem*>(mx->EntityByIdt(regiment->TargetId()));

        Target ( item );

        // if there is nothing special
        // about the location we are going to
        // then the thing we were going to has
        // probably gone
        if ( !mx->gamemap->IsLocationSpecial( targetlocation ) ) {
            targetlocation=regiment->Location();
            Complete();
            return;
        }
    }

    void RegimentTurnProcessor::Wander ( void )
    {
        while (true) {
            auto dir = (mxdir_t)mxrandom(DR_NORTH, DR_NORTHWEST);
            targetlocation = regiment->Location() + dir;
            if ( !mx->scenario->isLocationImpassable(targetlocation,regiment) )
                break;
        }
    }

    void RegimentTurnProcessor::Follow ( void )
    {
        auto item = static_cast<mxitem*>(mx->EntityByIdt(regiment->TargetId()));

        if ( item->Type() == IDT_CHARACTER ) {
            auto character = static_cast<mxcharacter*>(item);
            if ( character->IsDead() )
                item = mx->scenario->MoonringWearer();
        }

        Target ( item );
    }

    void RegimentTurnProcessor::Route ( void )
    {
        auto item = static_cast<mxitem*>(mx->EntityByIdt(regiment->TargetId()));

        if ( item->Type() == IDT_ROUTENODE ) {
            auto routenode = static_cast<mxroutenode*>(item);
            if ( routenode->Location() == regiment->Location() )
                item = mxrandom(1) ? routenode->Right() : routenode->Left();
        }

        Target ( item );
    }

    void RegimentTurnProcessor::Target ( const mxitem* newtarget )
    {
        regiment->TargetId(regiment->SafeIdt(newtarget) );
        targetlocation = newtarget->Location();
    }

    void RegimentTurnProcessor::Complete()
    {
        turns = REGIMENT_STOP_MOVE;
    }

    bool RegimentTurnProcessor::Retarget ( void )
    {
    mxdir_t    directions[4];
    bool       impassable;
    
        auto dir = regiment->Location().DirFromHere ( targetlocation );

        // when moving in a direction
        // consider the direction wanted twice to give priority
        // and then one either side
        directions[0] = dir;
        directions[1] = dir;
        directions[2] = (mxdir_t)((dir-1)&0x07);
        directions[3] = (mxdir_t)((dir+1)&0x07);

        for (int ii = 0; ii < 8; ii++) {
            impassable = false;
            int route = mxrandom(0, 3);
            targetlocation = regiment->Location() + directions[route];
            
            mxloc& mapsqr = mx->gamemap->GetAt( targetlocation );
            auto tinfo = mx->TerrainById( mapsqr.terrain );
            
            impassable = mx->scenario->isTerrainImpassable((mxterrain_t)mapsqr.terrain, regiment);
            
            CONTINUE_IF(impassable);
            
            // TODO should be a lookup
            // Doomguards don't like forest or mountains so try and
            // avoid them
            if ( mapsqr.terrain != TN_MOUNTAIN && mapsqr.terrain != TN_FOREST )
                break;
        }

        // if we drop through to here, we only care if
        // the location infront can actually be entered.
        // regardless if we don't like the terrain type

        return !impassable ;
    }
}
// namespace tme
