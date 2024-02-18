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
        

        mxid EntityByName( const std::string& name, id_type_t type=IDT_NONE );
        inline mxid CharacterBySymbol( const std::string& symbol ) {
            return EntityByName(symbol,IDT_CHARACTER);
        }

        MXRESULT GetEntityProperty( mxid id, const std::string& name, variant& arg) const ;
        MXRESULT GetEntityProperties ( mxid id, const std::string& name, variant argv[], u32 argc );

        MXRESULT GetProperty( const std::string& name, variant& arg) const ;
        MXRESULT GetProperties ( const std::string& name, variant argv[], u32 argc );
        MXRESULT SetProperty( const std::string& name, const std::string& value);
        
        MXRESULT Command ( const std::string& command, variant* argv=NULL, u32 args=0 );
        MXRESULT Text ( const std::string& command, variant* argv=NULL, u32 args=0 );

        void* EntityUserData ( mxid id );
        

    };
}

#endif //_CMXINTERFACE_H_INCLUDED_
