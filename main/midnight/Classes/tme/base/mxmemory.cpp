/*
 * FILE:    mxmemory.cpp
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

	mxmemory::mxmemory()
	{
		m_first = NULL ;
	}

	mxmemory::~mxmemory()
	{
		ForgetAll ();
	}
	
	//
	// METHOD:	IsMemorised
	//			Is the race known about at the given location
	// 
	// PARAMS:	loc			location
	//			race			race
	// 
	// REMARKS:	
	//
	// RETURNS:	mxfragment - fragment that records that race
	//
	mxfragment* mxmemory::IsMemorised ( mxgridref loc, mxrace_t race ) const
	{
    /*
	mxfragment* memitem = m_first;

			while ( memitem ) {
				if ( memitem->m_location == loc ) {
					if ( memitem->m_race == race )
						return memitem;
				}
				memitem=memitem->m_Next;
			}
	*/
		return NULL ;
	}


	//
	// METHOD:	Memorise
	//			Store the memory fragment for the given item and race
	// 
	// PARAMS:	info			item to memorise
	//			race			race of item
	// 
	// REMARKS:	
	//
	// RETURNS:	NONE
	//
	void mxmemory::Memorise ( mxitem* info, mxrace_t race )
	{
        /*
		mxfragment* item = IsMemorised ( info );
		if ( item ) {
			ForgetFragment ( item );
			item->m_day = sv_days;
			item->m_location = info->Location();
		}else{
			item = new mxfragment ( info );
		}
		item->m_race = race;

		// add head
		if ( m_first ) {
			item->m_Next = m_first ;
			m_first->m_Prev = item ;
		}
		m_first = item ;
        */
	}
	//
	// METHOD:	IsMemorised
	//			is the item currently in the memory
	// 
	// PARAMS:	info			item to memorise
	// 
	// REMARKS:	
	//
	// RETURNS:	mxfragment - memory fragment for the item
	//
	mxfragment* mxmemory::IsMemorised ( mxitem* item ) const
	{
    /*
	mxfragment* memitem = m_first;
		
		while ( memitem ) {
			if ( memitem->m_data == item )
				return memitem;
			memitem=memitem->m_Next;
		}
	*/
		return NULL ;
	}
	
	//
	// METHOD:	ForgetFragment
	//			Forget the given memory fragment
	// 
	// PARAMS:	item			fragment to forget
	// 
	// REMARKS:	fragment is removed from the memory chain but not deleted
	//			caller must think about deletion
	//
	// RETURNS:	NONE
	//
	void mxmemory::ForgetFragment ( mxfragment* item )
	{
    /*
		// first
		if ( m_first == item ) {
			m_first = item->m_Next ;
			//mxmemory = NULL ;
			return;
		}

		// linked
		if ( item->m_Prev ) 
			item->m_Prev->m_Next = item->m_Next ;

		if ( item->m_Next )
			item->m_Next->m_Prev = item->m_Prev;
	*/
    }

	//
	// METHOD:	DeleteFragment
	//			Forget tge given memory fragment and delete it
	// 
	// PARAMS:	item			fragment to forget
	// 
	// REMARKS:	Fragment is removed from the memory chain and deleted
	//
	// RETURNS:	NONE
	//
	void mxmemory::DeleteFragment ( mxfragment* item )
	{
    /*
		ForgetFragment(item);
		SAFEDELETE(item);
	*/
    }

	//
	// METHOD:	ForgetAll
	//			Forget and delete all memory fragments
	// 
	// REMARKS:	
	//
	// RETURNS:	NONE
	//
	void mxmemory::ForgetAll ( void )
	{
	/*
        mxfragment* memitem = m_first;
		while ( memitem ) {
			DeleteFragment(memitem);
			memitem = m_first;
		}
	 */
     }
	
	//
	// METHOD:	Update
	//			All memory fragments older than the default age
	//			are forgotten and deleted
	// 
	// REMARKS:	
	//
	// RETURNS:	NONE
	//
	void mxmemory::Update ( void )
	{
    /*
	mxfragment*		item;
	mxfragment*		next;
		
		item = m_first;
		while ( item ) {
			next = item->m_Next;
			if ( item->Age() > sv_character_default_memory_age ) {
				DeleteFragment(item);
			}
			item = next;
		}
	*/
	}


	void mxmemory::Serialize ( archive& ar )
	{
    /*
	int			count;
	mxfragment*	item;

		if ( ar.IsStoring() ) {

			count=0;
			item = m_first;
			while ( item ) {
				count++;
				item = item->m_Next;
			}		
			ar << count;

			while ( item ) {
				item->Serialize(ar) ;
				item = item->m_Next;
			}		

		}else{

			ar >> count ;

				while ( count ) {
					item = new mxfragment;
					item->Serialize(ar) ;

					// add head
					if ( m_first ) {
						item->m_Next = m_first ;
						m_first->m_Prev = item ;
					}
					m_first = item ;

					count--;
				}

		}
     */
	}


}
// namespace tme
