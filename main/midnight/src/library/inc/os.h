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
            
            static std::string Fullpath ( char *UserBuf, LPCSTR path, u32 maxlen );
        
            static bool Exists ( const std::string& lpFileName ) ;
            static bool ExistsDir ( const std::string& lpFileName ) ;
            static bool Get ( const std::string& filename, void* lpBuffer, u32 maxSize) ;
            static void* Load ( const std::string& filename, u32* size=NULL ) ;
            static u32 Size ( const std::string& filename ) ;
            static void Unload ( u8** ptr ) ;
            static bool Save( const std::string& filename, const void* lpBuffer, u32 size ) ;
            static bool Remove(const std::string& lpszFileName) ;
            static bool Rename(const std::string& lpszOldName, const std::string& lpszNewName) ;

            static bool CreateDirectory(const std::string& path);
            static bool DestroyDirectory ( const std::string& path );
            
            static std::string GetCurrentDirectory ( int max, const std::string& dir );
            static void SetCurrentDirectory ( const std::string& dir );
            
            static bool Copy ( const std::string& lpszSrcName, const std::string& lpszDstName );

            static s64 DateTime(const std::string& lpFileName);
    

        };
        // class filemanager

    }
}

#endif // _OS_H_INCLUDED_
