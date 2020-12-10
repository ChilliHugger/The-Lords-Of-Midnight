/*
 * FILE:    collections.cpp
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

#include <algorithm>

using namespace chilli;
using namespace chilli::lib;

namespace tme {
    
    namespace collections {

constexpr u32 COLLECTION_PAGE_SIZE = 16;
constexpr u32 MINIMUM_COLLECTION_SIZE = COLLECTION_PAGE_SIZE;

        entities::entities(void)
        {
            m_max = 0 ;
            m_used = 0;
            m_objElements=NULL;
            m_bOwner=FALSE;
        }

        entities::~entities(void)
        {
            Destroy();
        }

        void entities::operator = ( const entities& src )
        {
            Create(src.Count());
            for ( u32 ii=0; ii<src.Count(); ii++ ) {
                m_objElements[ii] = src.m_objElements[ii];
                m_used++;
            }
            m_bOwner=FALSE;
        }

        bool entities::CreateIdtCollection ( c_mxid& obj )
        {
            obj.Create( Count() );
            for ( u32 ii=0; ii<Count(); ii++ )
                obj[ii] = m_objElements[ii]->SafeIdt();

            return TRUE;
        }

        bool entities::Create( u32 count )
        {
            Destroy();
            m_max = std::max<int>(count,MINIMUM_COLLECTION_SIZE);
            m_used=0;
            m_objElements = new mxentity* [ m_max ];
            return TRUE ;
        }

        bool entities::Create( mxscenario* scenario, id_type_t type, u32 count )
        {
            Create(count);

            for ( u32 ii=0; ii<count; ii++, m_used++ )
                m_objElements[ii] = scenario->CreateEntity(type);

            //m_bOwner=TRUE;
            return TRUE ;
        }

        mxentity* entities::FindSymbol ( const c_str& name )
        {
            for ( u32 ii=0; ii<Count(); ii++ )
                if (c_stricmp( m_objElements[ii]->Symbol(), name ) == 0 )
                    return m_objElements[ii] ;
            return NULL ;
        }

        s32 entities::IndexOf ( mxentity* entity )
        {
            for ( u32 ii=0; ii<Count(); ii++ ) {
                if ( m_objElements[ii] == entity )
                    return ii ;
            }
            return -1 ;
        }

        u32 entities::Count() const
        { 
            return m_used;
        }

        u32 entities::Compact( void )
        {
            Resize(Count());
            return Count();
        }

        void entities::Resize ( u32 newsize )
        {
            if ( newsize == m_max )
                return ;

            if ( newsize == 0 ) {
                // lose old array
                Clear();
            }

            // allocate new size
            mxentity** objElements = new mxentity* [ newsize ];

            // copy the original
            for ( u32 ii=0; ii<std::min<int>(newsize,m_used); ii++ ) {
                objElements[ii] = m_objElements[ii];
                m_objElements[ii]=NULL;
            }
            
            // lose old array
            SAFEDELETEARRAY(m_objElements);

            // new data
            m_objElements = objElements ;
            m_max = newsize;
            m_used = std::min<int>( m_used, m_max );

        }

        void entities::operator += ( mxentity* o )
        {
            Add(o);
        }

        bool entities::Add ( mxentity* o )
        {
            if ( m_bOwner ) return FALSE;
            if ( m_used == m_max ) {
                // add a new page                
                int size = m_max + COLLECTION_PAGE_SIZE ;
                size = (size / COLLECTION_PAGE_SIZE) * COLLECTION_PAGE_SIZE ;
                Resize( size );
            }
            m_objElements[m_used++] = o;
            return true;
        }

        

        mxentity*& entities::operator[]( u32 nSubscript )
        {
        static mxentity* iErr = NULL;

            if ( /*nSubscript>=0 &&*/ nSubscript<Count() )
                return m_objElements[ nSubscript ];
            return iErr;
        }

        void entities::Clear( void )
        {
            m_used=0;
            m_max=0;
            SAFEDELETEARRAY(m_objElements);
        }

        void entities::Serialize( archive& ar )
        {
        u32 ii;
        int id;

            if ( ar.IsStoring() ) {
                for ( ii=0; ii<Count(); ii++ ) {
                    id = ii+1;
                    ar << id;
                    m_objElements[ii]->Serialize ( ar );
                }
            }else{
                for ( ii=0; ii<Count(); ii++ ) {
                    ar >> id;
                    id--;
                    m_objElements[id]->Serialize ( ar );
                    m_objElements[id]->Id(id+1);
                }
            }
        }

        void entities::Destroy( )
        {
            if ( m_bOwner ) {
                for ( u32 ii=0; ii<Count(); ii++ )
                    SAFEDELETE ( m_objElements[ii] );
            }

            Clear();
        }

        static int sort_hint=0;
        int object_compare( const void *arg1, const void *arg2 )
        {
            mxentity* f1 = (mxentity*)*(unsigned long*)arg1;
            mxentity* f2 = (mxentity*)*(unsigned long*)arg2;
            return f1->Compare(f2,sort_hint);
        }

        void entities::Sort ( int hint )
        {
            sort_hint = hint;
            qsort( (void *)m_objElements, (size_t)Count(), sizeof( mxobject* ), object_compare );

        }


        void infos::Serialize( archive& ar )
        {
        u32 ii;
        int id;

            if ( ar.IsStoring() ) {
                for ( ii=0; ii<Count(); ii++ ) {
                    ar << ii;
                    m_objElements[ii]->Serialize ( ar );
                }
            }else{
                for ( ii=0; ii<Count(); ii++ ) {
                    ar >> id;
                    m_objElements[id]->Serialize ( ar );
                    m_objElements[id]->Id(id);
                }
            }
        }

        infos::infos(void)
        {
        }

        infos::~infos(void)
        {
        }

    }
    // namespace collections
}
// namespace tme
