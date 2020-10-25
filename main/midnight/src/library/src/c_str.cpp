#include "../inc/mxtypes.h"
#include "../inc/misc.h"
#include "../inc/carchive.h"

using namespace chilli::lib;

namespace chilli {

    namespace types {

        c_str    strNull;

        c_str::c_str()
        {
            m_pchData = NULL ;
        }

        c_str::~c_str()
        {
            SAFEFREE ( m_pchData );
        }

        const c_str& c_str::operator= ( LPCSTR str )
        {
            SAFEFREE ( m_pchData );
            if ( str )
                m_pchData = c_strdup ( str );
            return *this;
        }

        const c_str& c_str::operator= ( const c_str& str )
        {
            SAFEFREE ( m_pchData );
            if ( str.m_pchData )
                m_pchData = c_strdup ( str.m_pchData );
            return *this;
        }


        c_str::c_str ( LPSTR str )
        {
            m_pchData  = NULL ;
            if ( str )
                m_pchData = c_strdup ( str );
        }

        c_str::c_str ( LPCSTR str )
        {
            m_pchData  = NULL ;
            if ( str )
                m_pchData = c_strdup ( str );
        }

        bool c_str::IsNull() const
        {
            return m_pchData == NULL ;
        }

        bool c_str::IsEmpty() const
        {
            return Length() == 0 ;
        }

        size_t c_str::Length() const
        {
            return m_pchData ? c_strlen(m_pchData) : 0 ;
        }
        
        archive& c_str::Serialize ( archive& ar )
        {
        LPSTR temp = NULL;

            if ( ar.IsStoring() ) {
                ar << ((char*)m_pchData) ; 
            }else{
                ar >> &temp ; 
                SAFEFREE ( m_pchData );
                if ( temp && c_strlen(temp) )
                    m_pchData = c_strdup(temp);
            }

            SAFEFREE ( temp );
            return ar; 
        }

    }
    // namespace types

}
// namespace tme
