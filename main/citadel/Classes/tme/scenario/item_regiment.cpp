/*
 * FILE:    item_regiment.cpp
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

		mxregiment::mxregiment()
		{
			mxentity::type = IDT_REGIMENT ;
			targetid=NONE;
			total=NONE;
			orders=OD_NONE;
            lost=0;
		}

		mxregiment::~mxregiment()
		{

		}

		void mxregiment::Serialize ( archive& ar )
		{
			mxitem::Serialize(ar);
			if ( ar.IsStoring() ) {
				WRITE_ENUM ( race );
				WRITE_ENUM ( type );
				ar << total;
				ar << targetid;
				WRITE_ENUM ( orders );
				ar << success;
				ar << loyalty;
				ar << killed ;
                ar << lastlocation ;
                ar << delay;
			}else{
				READ_ENUM ( race );
				READ_ENUM ( type );
				ar >> total;
				ar >> targetid;
				READ_ENUM ( orders );
				ar >> success;
				ar >> loyalty;
				ar >> killed;
                lost=0;
                lastlocation = Location();
                delay=0;
                
                if ( tme::mx->SaveGameVersion() > 3 )
                    ar >> lastlocation ;
                //else
                //    lastlocation = Location();

                if ( tme::mx->SaveGameVersion() > 6 )
                    ar >> delay ;
            
            }
		

        }

		s32 mxregiment::BattleSuccess ( const mxlocinfo& locinfo )
		{
			return success + mx->battle->BaseDoomdarkSuccess(Race(),Type(),locinfo);
		}

		void mxregiment::Cmd_Night ( void )
		{
           
            lastlocation = Location();
            
			turns = (u32)sv_regiment_default_moves;
			while ( turns>0 ) 
				NextTurn();
		}

		void mxregiment::NextTurn ( void )
		{
		s32 totaldoomdarkarmies;
		s32	movement;
		mxdir_t dir;

            
			// check that this regiment isn't dead already
			if ( Total() == 0) {
				// terminate the movement
				EndOfTurn();
				return;
			}

			// check that this regiment isn't on hold...		
			if ( Orders() == OD_HOLD ) {
				// terminate the movement
				EndOfTurn();
				return;
			}

            
            // let this army hang around a little
            //if ( Orders() == OD_DELAYED_WANDER ) {
                if ( delay ) {
                    delay--;
                    EndOfTurn();
                    return;
                }//else
                 //   Orders(OD_WANDER);
            //}

			// if there is something about this location then
			// don't go any futher
			if ( mx->gamemap->IsLocationSpecial(location) ) {
				EndOfTurn();
				return;
			}

			BOOL bFoundLocation = FALSE;
			
			// do a complete scan around us and see if there
			// is anything more interesting to do than our current orders
			// ie: someone else is in the next location
			if ( !IsFlags(rf_direct) ) {
				for (dir=DR_NORTH; dir<=DR_NORTHWEST; dir=(mxdir_t)((int)dir+1))	{
					targetlocation = Location()+dir;
					if ( mx->gamemap->IsLocationSpecial(targetlocation) ) {
						bFoundLocation=TRUE;
						break;
					}
				}
			}

			//
			// If we have not found a special location
			// then we are going to fall back on our orders
			//
			if ( !bFoundLocation ) {
				// now check what we must do
				switch ( Orders() ) {
					case OD_NONE:
						EndOfTurn();
						return;

					case OD_GOTO:
						Cmd_Goto();
						break;

					case OD_WANDER:
						Cmd_Wander();
						break;

					case OD_FOLLOW:
						Cmd_Follow();
						break;

					case OD_ROUTE:
						Cmd_Route();
						break;
						
					default:
						break;
				}

				
				if ( Orders() != OD_WANDER ) {
					// lets see if we are actually at the location
					// we are trying to get to
					if ( Location() == targetlocation ) {
						EndOfTurn();
						return;
					}

					// where must we walkto to move in the direction we wish
					// to go?
					if ( !RetargetTarget() ) {
						EndOfTurn();
						return;
					}
				}
			}
			

			// get the location we want
			flags32_t f = 0;
			if ( IsInTunnel() ) f|= slf_tunnel ;
			mxlocinfo*	info = new mxlocinfo ( targetlocation, NULL, f );
			totaldoomdarkarmies = info->foe.armies ;
			SAFEDELETE(info);

			// we can't move into the location if it is full
			// or our armies
			if ( totaldoomdarkarmies >= MAX_FOEARMIES_INLOCATION ) {
				EndOfTurn();
				return;
			}
			
			// calculate our movement values
			mxrace* rinfo = mx->RaceById(Race());
            
			movement = rinfo->InitialMovementValue();
			
            // are we moving diagonally?
			dir = Location().DirFromHere ( targetlocation );			
            if ( dir&1 )
				movement += rinfo->DiagonalMovementModifier();
            
            // adjust for terrain
			movement += rinfo->TerrainMovementModifier((mxterrain_t)mx->gamemap->GetAt ( location ).terrain);
			
            // are we on horseback?
			if ( Type()!=UT_RIDERS )
				movement = (int) ((f64)movement * rinfo->RidingMovementMultiplier()) ;
			
            // check for max out
			movement = MIN ( movement, (s32)rinfo->MovementMax() );		
            
            s32 temp = turns;
            temp -= movement;
            turns = MAX(0,temp);
            
			location = targetlocation;

			// add us into the location we have just occupied
			//mx->gamemap->SetLocationArmy(Location(),1);
		}

		void mxregiment::Cmd_Goto ( void )
		{
		mxitem*	i = (mxitem*)mx->EntityByIdt(targetid);

			Target ( i );

			// if there is nothing special
			// about the location we are going to
			// then the thing we were going to has
			// probably gone
			if ( !mx->gamemap->IsLocationSpecial( targetlocation ) ) {
				targetlocation=Location();
				EndOfTurn();
				return;
			}
		}

		void mxregiment::Cmd_Wander ( void )
		{
			while (TRUE) {
				mxdir_t dir  = (mxdir_t)random(DR_NORTH,DR_NORTHWEST);
				targetlocation = Location() + dir;
				if ( !mx->gamemap->IsLocationBlock(targetlocation) )
					break;
			}
		}

		void mxregiment::Cmd_Follow ( void )
		{
		mxitem*	i = (mxitem*)mx->EntityByIdt(targetid);

			if ( i->Type() == IDT_CHARACTER ) {
				if ( ((mxcharacter*)i)->IsDead() )
					i = mx->scenario->MoonringWearer();
			}

			Target ( i );
		}

		void mxregiment::Cmd_Route ( void )
		{
		mxitem*	i = (mxitem*)mx->EntityByIdt(targetid);

			mxgridref loc1,loc2;
			
			if ( i->Type() ==  IDT_ROUTENODE ) {
				mxroutenode* r = (mxroutenode*)i;
				loc1 = r->Location();
				loc2 = Location();
				if ( loc1 == loc2 )
					i = random(1) ? r->Right() : r->Left();
			}

			Target ( i );
		}

		void mxregiment::Target ( const mxitem* newtarget )
		{
			targetid = newtarget->SafeIdt();
			targetlocation = newtarget->Location();
		}

		void mxregiment::EndOfTurn()
		{
			turns = REGIMENT_STOP_MOVE;
		}

		BOOL mxregiment::RetargetTarget ( void )
		{
		int		route;
		int		ii;
		mxdir_t	dir;
		mxdir_t	directions[4];
		mxterrain*	tinfo;

			dir = Location().DirFromHere ( targetlocation );

			// when moving in a direction
			// consider the direction wanted twice to give priority
			// and then one either side
			directions[0] = dir;
			directions[1] = dir;
			directions[2] = (mxdir_t)((dir-1)&0x07);
			directions[3] = (mxdir_t)((dir+1)&0x07);

			for (ii = 0; ii < 8; ii++) {
				route = random(0,3);
				targetlocation = Location() + directions[route];
				
				mxloc& mapsqr = mx->gamemap->GetAt( targetlocation );
				tinfo = mx->TerrainById( mapsqr.terrain );
				if ( tinfo->IsBlock() )
					continue;

				// TODO should be a lookup
				// Doomguards don't like forest or mountains so try and
				// avoid them
				if ( mapsqr.terrain != TN_MOUNTAIN && mapsqr.terrain != TN_FOREST )
					break;
			}

			// if we drop through to here, we only care if
			// the location infront can actually be entered.
			// regardless if we don't like the terrain type

			return !tinfo->IsBlock() ;
		}

		mxgridref mxregiment::TargetLocation() const 
		{ 
		mxitem*	i;


			switch ( Orders() ) {

				case OD_WANDER:
					return Location();

				case OD_FOLLOW:
				case OD_ROUTE:
				case OD_GOTO:
					i = (mxitem*)mx->EntityByIdt(targetid);
					if ( i == NULL ) {
						return Location();
					}
					return i->Location();
				
				default:
					break;
			}

			return Location();
		}

		MXRESULT mxregiment::FillExportData ( info_t* data )
		{
			// check if we are being asked to export the regiment as
			// army
			if ( (u32)ID_TYPE(data->id) == (u32)INFO_ARMY ) {
				defaultexport::army_t* out = (defaultexport::army_t*)data;
				out->parent = SafeIdt();
				out->race = Race();
				out->type = AT_REGIMENT ;
				out->total = Total() ;

#ifndef _WOTS_MODS_
				out->energy = 0 ;	//unit->Energy();
				out->lost = 0;		//unit->Lost();
#endif // _WOTS_MODS_

				out->killed = Killed();		
				return MX_OK ;
			}

			defaultexport::regiment_t* out = (defaultexport::regiment_t*)data;

			VALIDATE_INFO_BLOCK(out,INFO_REGIMENT,defaultexport::regiment_t);

			out->race = Race();
			out->type = Type();
			out->total = Total();
			out->target = TargetId();
			out->targetlocation = TargetLocation();

			out->orders = Orders();
			out->success = success;
			out->killed = Killed();
			out->loyalty = loyalty->SafeIdt();

            out->lastlocation = lastlocation ;
            
			return mxitem::FillExportData ( data );
		}


}
// namespace tme
