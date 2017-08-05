#ifndef _CMXINTERFACE_H_INCLUDED_
#define _CMXINTERFACE_H_INCLUDED_

#include "library.h"
#include "lomxtypes.h"

namespace tme {

	class mxinterface
	{
	public:
		mxinterface(void);
		virtual ~mxinterface(void);

		LPCSTR EntitySymbolById( mxid id );
		MXRESULT EntityById( mxid id, info_t* info );
		MXRESULT EntityLinkData( mxid id, const void* data );
		

		mxid EntityByName( const string& name, id_type_t type=IDT_NONE );
		MXRESULT GetEntityProperty( mxid id, const string& name, variant& arg) const ;
		MXRESULT GetEntityProperties ( mxid id, const string& name, variant argv[], u32 argc );

		MXRESULT GetProperty( const string& name, variant& arg) const ;
		MXRESULT GetProperties ( const string& name, variant argv[], u32 argc );

		MXRESULT Command ( const string& command, variant* argv=NULL, u32 args=0 );
		MXRESULT Text ( const string& command, variant* argv=NULL, u32 args=0 );

		void* EntityUserData ( mxid id );
		

	};
}

#endif //_CMXINTERFACE_H_INCLUDED_
