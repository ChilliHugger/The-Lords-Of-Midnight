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



        // class fileentry
        class  fileentry
        {
        public:
            fileentry();
            virtual ~fileentry();

        public:
            c_str m_filename;
        };
        // class fileentry

        // class filelist
        class  filelist
        {
        public:

            enum { // see io.h
                NORMAL    =0x00, /* _A_NORMAL    Normal file - No read/write restrictions */
                RDONLY    =0x01, /* _A_RDONLY    Read only file */
                HIDDEN    =0x02, /* _A_HIDDEN    Hidden file */
                SYSTEM    =0x04, /* _A_SYSTEM    System file */
                SUBDIR    =0x10, /* _A_SUBDIR    Subdirectory */
                ARCH    =0x20, /* _A_ARCH       Archive file */
            };

            filelist();
            filelist( c_str filespec, u32 attribs=NORMAL );
            virtual ~filelist();

            bool CreateDir ( c_str filespec, u32 attribs=NORMAL );
            bool Destroy();

            int Count() const    { return m_count; };
            void Sort ( void );

            fileentry& GetAt ( u32 index ) const ;
            fileentry& operator[] ( u32 index ) const    { return GetAt(index); }

        protected:
            void MakeDirFileList ( void );
            u32 CountDirFiles ();

        protected:
            c_str        m_filespec;
            u32            m_count;
            fileentry*    m_dir;
            u32            m_attribs;

        };
        // filelist

    }
}

#endif // _OS_H_INCLUDED_
