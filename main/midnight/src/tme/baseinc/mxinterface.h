#ifndef _CMXINTERFACE_H_INCLUDED_
#define _CMXINTERFACE_H_INCLUDED_

#include "../../library/inc/library.h"
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
        

        mxid EntityByName( const c_str& name, id_type_t type=IDT_NONE );
        MXRESULT GetEntityProperty( mxid id, const c_str& name, variant& arg) const ;
        MXRESULT GetEntityProperties ( mxid id, const c_str& name, variant argv[], u32 argc );

        MXRESULT GetProperty( const c_str& name, variant& arg) const ;
        MXRESULT GetProperties ( const c_str& name, variant argv[], u32 argc );
        MXRESULT SetProperty( const c_str& name, const c_str& value);
        
        MXRESULT Command ( const c_str& command, variant* argv=NULL, u32 args=0 );
        MXRESULT Text ( const c_str& command, variant* argv=NULL, u32 args=0 );

        void* EntityUserData ( mxid id );
        

    };
}

#endif //_CMXINTERFACE_H_INCLUDED_
