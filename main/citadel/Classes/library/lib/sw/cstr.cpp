#include "../../libinc/library.h"

using namespace chilli::lib;

namespace chilli {

	namespace types {

		string	strNull;

		string::string()
		{
			m_pchData = NULL ;
		}

		string::~string()
		{
			SAFEFREE ( m_pchData );
		}

		const string& string::operator= ( LPCSTR str )
		{
			SAFEFREE ( m_pchData );
			if ( str )
				m_pchData = chilli::lib::strdup ( str );
			return *this;
		}

		const string& string::operator= ( const string& str )
		{
			SAFEFREE ( m_pchData );
			if ( str.m_pchData )
				m_pchData = chilli::lib::strdup ( str.m_pchData );
			return *this;
		}


		string::string ( LPSTR str )
		{
			m_pchData  = NULL ;
			if ( str )
				m_pchData = chilli::lib::strdup ( str );
		}

		string::string ( LPCSTR str )
		{
			m_pchData  = NULL ;
			if ( str )
				m_pchData = chilli::lib::strdup ( str );
		}

		BOOL string::IsNull() const
		{
			return m_pchData == NULL ;
		}

		BOOL string::IsEmpty() const
		{
			return Length() == 0 ;
		}

		size_t string::Length() const 
		{
			return m_pchData ? lib::strlen(m_pchData) : 0 ;
		}
		archive& string::Serialize ( archive& ar )
		{ 
		//char temp[256];
		LPSTR temp = NULL;

			if ( ar.IsStoring() ) {
				ar << ((char*)m_pchData) ; 
			}else{
				ar >> &temp ; 
				SAFEFREE ( m_pchData );
				if ( temp && chilli::lib::strlen(temp) )
					m_pchData = chilli::lib::strdup(temp);
			}

			SAFEFREE ( temp );
			return ar; 
		}

	}
	// namespace types

}
// namespace tme
