// flags32.cpp: implementation of the flags32 class.
//
//////////////////////////////////////////////////////////////////////

#include "../../libinc/library.h"

using namespace chilli::lib;

namespace chilli {

	namespace types {

		flags32::~flags32()
		{
		}

		archive& flags32::Serialize ( archive& ar )
		{ 
			if ( ar.IsStoring() ) {
				ar << m_flags ; 
			}else{
				ar >> m_flags ; 
			}
			return ar; 
		}

		flags8::~flags8()
		{
		}

		archive& flags8::Serialize ( archive& ar )
		{ 
			if ( ar.IsStoring() ) {
				ar << m_flags ; 
			}else{
				ar >> m_flags ; 
			}
			return ar; 
		}

		flags16::~flags16()
		{
		}

		archive& flags16::Serialize ( archive& ar )
		{ 
			if ( ar.IsStoring() ) {
				ar << m_flags ; 
			}else{
				ar >> m_flags ; 
			}
			return ar; 
		}


	}
	// namespace types

}
// namespace tme
