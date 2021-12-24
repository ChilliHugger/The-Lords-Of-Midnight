#ifndef _OS_H_INCLUDED_
#define _OS_H_INCLUDED_

#include "mxtypes.h"


namespace chilli {

    namespace os {

        using namespace chilli::types;
        
        // class filemanager
        class  filemanager
        {
        public:
            
            static char* Fullpath ( char *UserBuf, const char *path, u32 maxlen );
        
            static bool Exists ( LPCSTR lpFileName ) ;
            static bool ExistsDir ( LPCSTR lpFileName ) ;
            static bool Get ( LPCSTR filename, void* lpBuffer, u32 maxSize) ;
            static void* Load ( LPCSTR filename, u32* size=NULL ) ;
            static u32 Size ( LPCSTR filename ) ;
            static void Unload ( u8** ptr ) ;
            static bool Save( LPCSTR filename, const void* lpBuffer, u32 size ) ;
            static bool Remove(LPCSTR lpszFileName) ;
            static bool Rename(LPCSTR lpszOldName, LPCSTR lpszNewName) ;

            static bool CreateDirectory(LPCSTR path);
            static bool DestroyDirectory ( LPCSTR path );
            
            static LPSTR GetCurrentDirectory ( int max, LPSTR dir );
            static void SetCurrentDirectory ( LPCSTR dir );
            
            static bool Copy ( LPCSTR lpszSrcName, LPCSTR lpszDstName );

            static s64 DateTime(LPCSTR lpFileName);
    

        };
        // class filemanager

    }
}

#endif // _OS_H_INCLUDED_
