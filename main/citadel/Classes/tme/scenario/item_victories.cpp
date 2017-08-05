/*
 * FILE:    item_victories.cpp
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


		mxvictory::mxvictory()
		{
			mxentity::type = IDT_VICTORY ;
		}

		mxvictory::~mxvictory()
		{
		}

		void mxvictory::Serialize ( archive& ar )
		{
			mxentity::Serialize ( ar );
			if ( ar.IsStoring() ) {
				ar << priority;
				ar << mission;
				ar << string;
			}else{
				ar >> priority;
				ar >> mission;
				ar >> string;
			}
		}

		MXRESULT mxvictory::FillExportData ( info_t* data )
		{
			return MX_FAILED;
		}

		int mxvictory::Compare ( mxentity* o, int hint ) const
		{
			if ( hint == 1 )
				return chilli::lib::Compare( Priority(), ((mxvictory*)o)->Priority() );


			return mxentity::Compare(o,hint);
		}

		BOOL mxvictory::CheckComplete ( void )
		{
			// should we be checking
			if ( IsDisabled() )
                return false;
            
            if ( IsComplete() )
				return TRUE;

			mxmission* m = mx->MissionById(GET_ID(mission));
			if ( m==NULL || !m->IsComplete() ) {
				return FALSE ;
			}
	
			flags.Set(vf_complete);

			return TRUE;
		}

		mxmission::mxmission()
		{
			mxentity::type = IDT_MISSION ;
		}

		mxmission::~mxmission()
		{
		}

		void mxmission::Serialize ( archive& ar )
		{
			mxentity::Serialize ( ar );
			if ( ar.IsStoring() ) {
				ar << priority;
				WRITE_ENUM(objective);
				WRITE_ENUM(condition);
				for ( u32 ii=0; ii<NUMELE(reference); ii++ )
					ar << reference[ii];
				ar << points;
				ar << scorer;
				WRITE_ENUM(action);
				ar << actionid;
			}else{
				ar >> priority;
				READ_ENUM(objective);
				READ_ENUM(condition);
				for ( u32 ii=0; ii<NUMELE(reference); ii++ )
					ar >> reference[ii];
				ar >> points;
				ar >> scorer;
				READ_ENUM(action);
				ar >> actionid;
			}
		}

		MXRESULT mxmission::FillExportData ( info_t* data )
		{
			return MX_FAILED;
		}

		int mxmission::Compare ( mxentity* o, int hint ) const
		{
			if ( hint == 1 )
				return chilli::lib::Compare( Priority(), ((mxmission*)o)->Priority() );

			return mxentity::Compare(o,hint);
		}

		BOOL mxmission::Completed ( void )
		{
			if ( IsComplete() )
				return TRUE;

			flags.Set(mf_complete);
	
			// complete actions
			if ( action == MA_ENABLE ) {
				mxitem* i = (mxitem*)mx->EntityByIdt(actionid);
				if ( i ) {
					i->Flags().Reset(et_disabled);
				}
			}

			if ( action == MA_DISABLE ) {
				mxitem* i = (mxitem*)mx->EntityByIdt(actionid);
				if ( i ) {
					i->Flags().Set(et_disabled);
					mxobject* obj = mx->ObjectById(GET_ID(actionid));				
					// lose the object
					mxcharacter* c = mx->scenario->WhoHasObject(obj);
					if ( c ) c->carrying = NULL ;
				}
			}

			// no points to flags
			if ( objective == MO_FLAG )
				return TRUE;
	
			// allocate points

			//object* o = mx->ObjectByIdt(scorer);
			//o->points += points;
			return TRUE;
		}

		mxgridref mxmission::GetLocation ( mxid id ) const
		{
		mxitem* i=NULL;

			switch ( ID_TYPE(id) ) {

				case IDT_CHARACTER:
				case IDT_REGIMENT:
				case IDT_STRONGHOLD:
				case IDT_ROUTENODE:
				case IDT_PLACE:
					{
						i = (mxitem*)mx->EntityByIdt(id);
						if ( i ) return i->Location();
					}

				case IDT_OBJECT:
					{
						mxobject* obj = mx->ObjectById(GET_ID(id));
						i = mx->scenario->WhoHasObject(obj);
						if ( i ) return i->Location();
					}
					break;

				default:
					break;

					
			}

			return mxgridref(-1,-1);
		}


		BOOL mxmission::CheckComplete ( void )
		{
		u32			ii;
		u32			usedref=0;
		u32			ref_id[NUMELE(reference)];
		u32			ref_type[NUMELE(reference)];

			// should we be checking
			if ( IsDisabled() )
				return FALSE;

			// are we already complete?
			if ( objective != MO_FLAG && IsComplete() )
				return FALSE;
	

			// how many of the references are used?
			// and split them
			usedref=0;
			for ( ii=0; ii<NUMELE(reference); ii++ ) {
				if ( reference[ii] != 0 ) usedref++;
				ref_id[ii] = GET_ID(reference[ii]);
				ref_type[ii] = ID_TYPE(reference[ii]);
			}

			switch ( condition ) {
				case MC_NONE:
					break;

				case MC_STRONGHOLD_TAKEN:
				// stronghold has fallen to race or character
					{
						if ( ref_type[0] != IDT_STRONGHOLD ) 
							return FALSE;

						mxstronghold* s = mx->StrongholdById(ref_id[0]);
						if ( ref_type[1] == IDT_RACEINFO ) {
							if ( s->OccupyingRace() == (mxrace_t)ref_id[1] )
								return Completed();
							return FALSE;
						}

						if ( ref_type[1] == IDT_CHARACTER ) {
							if ( s->Occupier()->Id() == ref_id[1] )
								return Completed();
							return FALSE;
						}

						return FALSE;
					}

				case MC_STRONGHOLD_FALLEN:
				// all of the five strongholds have fallen
					{
						mxstronghold* s=NULL ;
						u32 success=0;
						for ( ii=0; ii<NUMELE(reference); ii++ ) {
							if ( reference[ii] == 0 )
								continue;					
							if ( ref_type[ii] != IDT_STRONGHOLD ) 
								continue;
							s = mx->StrongholdById(ref_id[ii]);
							if ( s->HasFallen() )
								success++;
						}
						if ( (IsAny() && success) || ( success == usedref ) )
							return Completed();
					}
					break;

				case MC_DESTROY_OBJECT:
					// object has been destroyed
					break;

				case MC_CHARACTER_DEAD:
				// all of the five characters are dead
					{
						u32 success=0;
						for ( ii=0; ii<NUMELE(reference); ii++ ) {
							if ( reference[ii] == 0 )
								continue;					
							if ( ref_type[ii] != IDT_CHARACTER ) 
								continue;
							mxcharacter* c = mx->CharacterById(ref_id[ii]);
							if ( c->IsDead() )
								success++;
						}
						if ( (IsAny() && success) || ( success == usedref ) )
							return Completed();
					}
					break;

				case MC_MISSION_COMPLETE:
				// all of the five victory scores are complete
					{
						u32 success=0;
						for ( ii=0; ii<NUMELE(reference); ii++ ) {
							if ( reference[ii] == 0 )
								continue;					
							if ( ref_type[ii] != IDT_MISSION ) 
								continue;
							mxmission* s = mx->MissionById(ref_id[ii]);
							if ( s->IsComplete() )
								success++;
						}

						if ( (IsAny() && success) || ( success == usedref ) )
							return Completed();
					}

					break;

				case MC_SAME_LOCATION:
					{
						u32 success=0;
						mxgridref startloc;
						mxgridref loc;

						startloc = GetLocation(reference[0]);
						usedref--;

						for ( ii=1; ii<NUMELE(reference); ii++ ) {
							if ( reference[ii] == 0 )
								continue;		
							loc = GetLocation(reference[ii]);
							if ( startloc == loc )
								success++;
						}
						if ( (IsAny() && success) || ( success == usedref ) )
							return Completed();
					break;
					}
			}

			if ( objective == MO_FLAG )
				flags.Reset(mf_complete);

			return FALSE;
		}


}
// namespace tme


