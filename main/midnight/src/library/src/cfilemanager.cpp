/*
 * FILE:    filemanager.cpp
 *
 * PROJECT: LibraryOS
 *
 * CREATED:
 *
 * AUTHOR:  Chris Wild
 *
 * � Copyright 1999 The Midnight Council.
 *
 * PURPOSE:
 *
 *
 */

// cocos2dx
#define COCOS2DX 1

#include "../inc/mxtypes.h"
#include "../inc/cfile.h"
#include "../inc/os.h"
#include "../inc/misc.h"

#include <stdio.h>

#ifdef _MARMALADE_
    #include "IwFile.h"
    #include "S3EFile.h"
#endif

#if defined(COCOS2DX)
#include "platform/CCFileUtils.h"
#endif



namespace chilli {
    
    namespace os {
        
        /*
         * filemanager::Load
         * Allocates memory for the file and reads the entire contents into
         * memory.
         *
         * ENTRY:       str * filename     - File to get!
         * EXIT:        void *          - pointer to file in memory
         *                NULL value returned on error - check GetLastError()
         *
         * NOTE:        Remember to free the memory!
         *
         */
        
        void* filemanager::Load ( LPCSTR filename, u32* size )
        {
#if defined(COCOS2DX)
            auto data = cocos2d::FileUtils::getInstance()->getDataFromFile(filename);
            *size = (u32)data.getSize();
            if ( data.getSize() == 0 )
                return nullptr;
            void* buffer = malloc ( data.getSize() );
            memcpy(buffer, data.getBytes(), data.getSize());
            return buffer;
#else
            u32        dwSize;
            void*    lpBuffer;
            
            if ( size )
                *size=0;
            
            //DEBUG("filemanager::Load(%s)...", filename );
            if ( !Exists ( filename ) ) {
                //        DEBUG("FILE NOT FOUND...");
                return FALSE ;
            }
            
            PFILE pFile = new file ();
            if ( !pFile->Open( filename, file::modeRead ) ) {
                //        DEBUG("ERROR Open\n");
                if ( pFile ) delete pFile;
                return ( NULL );
            }
            
            /* get the file size */
            dwSize = pFile->GetLength();
            if ( dwSize == 0xffffffff )    {
                //        DEBUG("ERROR FileSize\n");
                delete pFile;
                return ( NULL );
            }
            
            /* allocate buffer to recieve data */
            if ( (lpBuffer = malloc ( dwSize+1 )) == NULL )    {
                //        DEBUG("ERROR Memory Alloc %s\n", dwSize);
                delete pFile;
                return ( NULL );
            }
            memset(lpBuffer,0,dwSize+1);
            if ( size )
                *size=dwSize;
            
#define CHUNK_SIZE    350*1024
            
            u8* temp = (u8*)lpBuffer;
            int chunk=0;
            while ( dwSize ) {
                if ( dwSize > CHUNK_SIZE )
                    chunk = CHUNK_SIZE;
                else
                    chunk = dwSize;
                
                if ( !pFile->Read ( temp, chunk ))    {
                    //            DEBUG("ERROR Read\n");
                    delete pFile;
                    free ( lpBuffer );
                    return ( NULL );
                }
                
                temp += chunk ;
                dwSize -= chunk;
                //Sleep(0);
            }
            
            delete pFile;
            
            //DEBUG("OK\n");
            
            return ( lpBuffer );
#endif
        }
        
        
        
        void filemanager::Unload ( u8** lpMem )
        {
            SAFEFREE( *lpMem );
        }
        
        u32 filemanager::Size ( LPCSTR filename )
        {
#if defined(COCOS2DX)
            return (u32)cocos2d::FileUtils::getInstance()->getFileSize(filename);
#else
            DWORD    dwSize;
            
            PFILE pFile = new file ( );
            if ( !pFile->Open(  filename, file::modeRead|file::shareCompat) ){
                if ( pFile ) delete pFile;
                return ( 0 );
            }
            
            dwSize = pFile->GetLength();
            
            delete pFile;
            
            return ( dwSize );
#endif
        }
        
        
        
        /*
         * filemanager::Save
         *
         *                 Saves data from memory to file
         *
         * ENTRY:       LPSTR    filename     - File to get!
         *                PVOID    lpBuffer    - address of data to write
         *                DWORD    dwSize        - amount of bytes to write
         * EXIT:        TRUE or FALSE
         *
         */
        
        bool filemanager::Save( LPCSTR filename, const void* lpBuffer, u32 dwSize )
        {
#if defined(COCOS2DX)
            cocos2d::Data data;
            data.copy((const unsigned char* )lpBuffer, dwSize);
            return cocos2d::FileUtils::getInstance()->writeDataToFile(data, filename);
#else
            PFILE pFile = new file (  );
            if ( !pFile->Open(filename, file::modeWrite|file::modeCreate) ) {
                //        DEBUG("ERROR Open\n");
                if ( pFile ) delete pFile;
                return ( FALSE );
            }
            
            /* write the data */
#ifndef _DONT_DEFINE_MFC_CLASSES
            if ( !pFile->Write ( lpBuffer, dwSize ))
            {
                //        DEBUG("ERROR Write\n");
                delete pFile;
                return ( FALSE );
            }
#else
            pFile->Write ( lpBuffer, dwSize );
            
#endif
            
            delete pFile;
            return ( TRUE );
#endif
            
        }
        
        
        
        
        
        /*
         * filemanager::Exists
         * Checks if a file exists or not
         *
         * ENTRY:       LPCSTR source   - file to find
         * EXIT:        TRUE or FALSE
         *
         */
        
        bool filemanager::Exists ( LPCSTR lpFileName )
        {
#ifdef _MARMALADE_
            return IwFileCheckExists(lpFileName);
            
#elif defined(COCOS2DX)
            return cocos2d::FileUtils::getInstance()->isFileExist(lpFileName);
            
#elif defined(_UNIX_) || defined(_LINUX_)
            filelist files;
            files.CreateDir ( (LPSTR)lpFileName );
            return files.Count() > 0 ;
#else
            
            PFILE pFile = new file (  );
            if ( pFile == NULL )
                return FALSE ;
            
            if ( !pFile->Open(lpFileName, file::modeRead|file::shareDenyNone) ) {
                delete pFile;
                return ( FALSE );
            }
            
            delete pFile;
            return TRUE ;
#endif            
        }
        
        s64 filemanager::DateTime(LPCSTR lpFileName)
        {
#ifdef _MARMALADE_
            int64 value = s3eFileGetFileInt(lpFileName, S3E_FILE_MODIFIED_DATE);
            return value ;
#endif
            return 0;
        }
        
        
        
        bool filemanager::ExistsDir(LPCSTR lpFileName)
        {
#if defined(COCOS2DX)
            return cocos2d::FileUtils::getInstance()->isDirectoryExist(lpFileName);
#endif
            //S3E_API s3eFileList* s3eFileListDirectory(const char* dirName);
            return FALSE;
        }
        
        bool filemanager::Rename(LPCSTR lpszOldName, LPCSTR lpszNewName)
        {
#ifdef _MARMALADE_
            return s3eFileRename(lpszOldName, lpszNewName ) == S3E_RESULT_SUCCESS ;
#elif defined(COCOS2DX)
            cocos2d::FileUtils::getInstance()->renameFile(lpszOldName,lpszNewName);
#else
            return rename((LPSTR)lpszOldName, (LPSTR)lpszNewName) == 0;
#endif
            return false;
        }
        
        bool filemanager::Remove(LPCSTR lpszFileName)
        {
#if defined(COCOS2DX)
            return cocos2d::FileUtils::getInstance()->removeFile(lpszFileName);
#else
            return remove((LPSTR)lpszFileName) == 0;
#endif
        }
        
        bool filemanager::Copy ( LPCSTR lpszSrcName, LPCSTR lpszDstName )
        {
#ifdef _MARMALADE_
            return IwFileCopy( lpszDstName, lpszSrcName ) == S3E_RESULT_SUCCESS ;
#elif defined(COCOS2DX)
            auto data = cocos2d::FileUtils::getInstance()->getDataFromFile(lpszSrcName);
            return cocos2d::FileUtils::getInstance()->writeDataToFile(data, lpszDstName);
#else

#endif
            return false;
        }
        
        bool filemanager::CreateDirectory(LPCSTR path)
        {
#ifdef WIN32
//            return ::CreateDirectory(path,NULL);
            return false;
#else
#ifdef _MARMALADE_
            if ( s3eFileMakeDirectory (path) == S3E_RESULT_SUCCESS )
                return TRUE;
#endif
#endif
            
#if defined(COCOS2DX)
            return cocos2d::FileUtils::getInstance()->createDirectory(path);
#endif
            
            return FALSE ;
        }
        
        bool filemanager::DestroyDirectory ( LPCSTR path )
        {
#ifdef _MARMALADE_
            char filename[MAX_PATH]={0};
            char filename2[MAX_PATH]={0};
            
            s3eFileList* file = s3eFileListDirectory(path);
            if ( file == NULL )
                return TRUE ;
            
            while ( s3eFileListNext( file, filename, MAX_PATH ) == S3E_RESULT_SUCCESS ) {
                sprintf(filename2, "%s/%s", path, filename);
                filemanager::Remove(filename2);
                CLEARARRAY(filename2);
            }
            
            s3eFileListClose(file);
#endif
            
#if defined(COCOS2DX)
            return cocos2d::FileUtils::getInstance()->removeDirectory(path);
#endif
            
            return TRUE ;
            
        }
        
        /*
         * Function name    : GetCurrentDirectory
         * 
         * Return type        : void
         * 
         * Arguments        : int max
         *                  : LPSTR dir
         * 
         * Description        : 
         * 
         */
        
        LPSTR filemanager::GetCurrentDirectory ( int max, LPSTR dir )
        {
#ifdef WIN32
            ::GetCurrentDirectoryA( max, dir );
#else
            //getcwd ( dir, max );
#endif
            return dir ;
        }
        
        void filemanager::SetCurrentDirectory (LPCSTR dir )
        {
#ifdef WIN32
            ::SetCurrentDirectoryA( dir );
#else
            //    setcwd ( dir );
#endif
        }
    
        char* filemanager::Fullpath ( char *UserBuf, const char *path, u32 maxlen )
        {
            char    *buf;
            size_t   cbPath;
            
            if( !path || !*path )
                return( (char*)GetCurrentDirectory( maxlen, UserBuf ) );
            
            if (UserBuf && (strlen(path) > maxlen))
                return NULL;
            
            // allocate buffer if necessary
            
            if( !UserBuf )
            {
                if( !(buf = (char*)malloc(MAX_PATH)) )
                {
                    //                 errno = ENOMEM;
                    return( NULL );
                }
                else
                {
                    maxlen = MAX_PATH;
                }
            }
            else
            {
                buf = UserBuf;
            }
            
            cbPath = strlen(path);
            if (cbPath > maxlen)
            {
                if (!UserBuf)
                {
                    free(buf);
                }
                //            errno = ERANGE;
                return NULL;
            }
            
            maxlen = maxlen - (u32)cbPath;
            if (*path == ':')
            {
                maxlen++;        //this ":" is not counted
            }
            
            if ((*path == ':') ||
                (strstr(path, ":") == NULL) )
            {
                //partial path
                if (GetCurrentDirectory( maxlen,buf ) != NULL)
                {
                    if (*path == ':')
                    {
                        lib::c_strcat(buf, path+1);
                    }
                    else
                    {
                        lib::c_strcat(buf, path);
                    }
                }
                else
                {
                    if (!UserBuf)
                    {
                        free(buf);
                    }
                    return( NULL);
                }
            }
            else
            {
                //fullpath
                lib::c_strcpy(buf, path);
            }
            
            return buf;
            
        }

        
        
    }
    // namespace os
}
// namespace tme

