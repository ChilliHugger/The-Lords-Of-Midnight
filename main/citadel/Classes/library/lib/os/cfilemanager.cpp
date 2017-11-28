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

#if 1

#include "../../libinc/library.h"
#include <stdio.h>

#ifdef _MARMALADE_
#include "IwFile.h"
#include "S3EFile.h"
#endif

// cocos2dx
#define COCOS2DX



namespace chilli {
    
    namespace os {
        
        /*
         * filemanager::Load
         * Allocates memory for the file and reads the entire contents into
         * memory.
         *
         * ENTRY:       str * filename 	- File to get!
         * EXIT:        void *          - pointer to file in memory
         *				NULL value returned on error - check GetLastError()
         *
         * NOTE:        Remember to free the memory!
         *
         */
        
        void* filemanager::Load ( LPCSTR filename, u32* size )
        {
            u32		dwSize;
            void*	lpBuffer;
            
            if ( size )
                *size=0;
            
            //DEBUG("filemanager::Load(%s)...", filename );
            if ( !Exists ( filename ) ) {
                //		DEBUG("FILE NOT FOUND...");
                return FALSE ;
            }
            
            PFILE pFile = new file ();
            if ( !pFile->Open( filename, file::modeRead ) ) {
                //		DEBUG("ERROR Open\n");
                if ( pFile ) delete pFile;
                return ( NULL );
            }
            
            /* get the file size */
            dwSize = pFile->GetLength();
            if ( dwSize == 0xffffffff )	{
                //		DEBUG("ERROR FileSize\n");
                delete pFile;
                return ( NULL );
            }
            
            /* allocate buffer to recieve data */
            if ( (lpBuffer = malloc ( dwSize+1 )) == NULL )	{
                //		DEBUG("ERROR Memory Alloc %s\n", dwSize);
                delete pFile;
                return ( NULL );
            }
            memset(lpBuffer,0,dwSize+1);
            if ( size )
                *size=dwSize;
            
#define CHUNK_SIZE	350*1024
            
            u8* temp = (u8*)lpBuffer;
            int chunk=0;
            while ( dwSize ) {
                if ( dwSize > CHUNK_SIZE )
                    chunk = CHUNK_SIZE;
                else
                    chunk = dwSize;
                
                if ( !pFile->Read ( temp, chunk ))	{
                    //			DEBUG("ERROR Read\n");
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
        }
        
        
        
        void filemanager::Unload ( u8** lpMem )
        {
            SAFEFREE( *lpMem );
            SAFEFREE( *lpMem );
        }
        
        
        
        /*
         * filemanager::Get
         *
         * 				reads a file into memory, as given by the user
         *
         * ENTRY:       LPSTR	filename 	- File to get!
         *				PVOID	memory		- where to load the file to
         *				u32		size		- maximum size of buffer
         * EXIT:		return 		TRUE on success
         *						on	FALSE GetLastError()
         */
        
        bool filemanager::Get ( LPCSTR filename, void* lpBuffer, u32 dwMaxSize)
        {
            u32	dwSize;
            
            //DEBUG("filemanager::Get(%s)... ", filename );
            
            //	_MXASSERTE ( IsValidAddress( lpBuffer, dwMaxSize, TRUE ) );
            if ( !Exists ( filename ) ) {
                //		DEBUG("FILE NOT FOUND...");
                return FALSE ;
            }
            
            PFILE pFile = new file ();
            if ( !pFile->Open( filename, file::modeRead) ) {
                //		DEBUG("ERROR Open\n");
                if ( pFile ) delete pFile;
                return ( NULL );
            }
            
            /* get the file size */
            dwSize = pFile->GetLength();
            if ( dwSize == 0xffffffff )	{
                //		DEBUG("ERROR FileSize\n");
                delete pFile;
                return ( FALSE );
            }
            
            /* if the filesize is greater than the buffer then truncate the read,
             * perform the read operation but return an warning error
             */
            if ( dwSize > dwMaxSize )	{
                if ( !pFile->Read ( lpBuffer, dwMaxSize ) ) {
                    //			DEBUG("ERROR TruncRead\n");
                    return ( FALSE );
                }
                
                delete pFile;
                //		SetLastError( FILEIO_BUFFER_EXCEEDED );
                return ( FALSE );
            }
            
            /* read the data */
            if ( !pFile->Read( lpBuffer, dwSize ) ) {
                //		DEBUG("ERROR Read\n");
                return ( FALSE );
            }
            
            delete pFile;
            
            //DEBUG("OK\n");
            
            return ( TRUE );
        }
        
        
        u32 filemanager::Size ( LPCSTR filename )
        {
            DWORD	dwSize;
            
            PFILE pFile = new file ( );
            if ( !pFile->Open(  filename, file::modeRead|file::shareCompat) ){
                if ( pFile ) delete pFile;
                return ( 0 );
            }
            
            dwSize = pFile->GetLength();
            
            delete pFile;
            
            return ( dwSize );
        }
        
        
        
        /*
         * filemanager::Save
         *
         * 				Saves data from memory to file
         *
         * ENTRY:       LPSTR	filename 	- File to get!
         *				PVOID	lpBuffer	- address of data to write
         *				DWORD	dwSize		- amount of bytes to write
         * EXIT:        TRUE or FALSE
         *
         */
        
        bool filemanager::Save( LPCSTR filename, const void* lpBuffer, u32 dwSize )
        {
            //DEBUG("...filemanager::Save(%s) %lp | %d", filename, lpBuffer, dwSize);
            
            //	_MXASSERTE ( IsValidAddress( lpBuffer, dwSize ) );
            
            PFILE pFile = new file (  );
            if ( !pFile->Open(filename, file::modeWrite|file::modeCreate) ) {
                //		DEBUG("ERROR Open\n");
                if ( pFile ) delete pFile;
                return ( FALSE );
            }
            
            /* write the data */
#ifndef _DONT_DEFINE_MFC_CLASSES
            if ( !pFile->Write ( lpBuffer, dwSize ))
            {
                //		DEBUG("ERROR Write\n");
                delete pFile;
                return ( FALSE );
            }
#else
            pFile->Write ( lpBuffer, dwSize );
            
#endif
            
            delete pFile;
            
            //DEBUG("OK\n");
            
            return ( TRUE );
            
            
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
#else
#if defined(_UNIX_) || defined(_LINUX_)
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
            
            //S3E_API s3eFileList* s3eFileListDirectory(const char* dirName);
            
            return FALSE;
        }
        
        bool filemanager::Rename(LPCSTR lpszOldName, LPCSTR lpszNewName)
        {
#ifdef _MARMALADE_
            return s3eFileRename(lpszOldName, lpszNewName ) == S3E_RESULT_SUCCESS ;
#else
            return rename((LPSTR)lpszOldName, (LPSTR)lpszNewName) == 0;
#endif
        }
        
        bool filemanager::Remove(LPCSTR lpszFileName)
        {
            return remove((LPSTR)lpszFileName) == 0;
        }
        
        bool filemanager::Copy ( LPCSTR lpszSrcName, LPCSTR lpszDstName )
        {
#ifdef _MARMALADE_
            return IwFileCopy( lpszDstName, lpszSrcName ) == S3E_RESULT_SUCCESS ;
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
            
            return TRUE ;
            
        }
        
        /*
         * Function name	: GetCurrentDirectory
         * 
         * Return type		: void
         * 
         * Arguments		: int max
         *                  : LPSTR dir
         * 
         * Description	    : 
         * 
         */
        
        LPSTR filemanager::GetCurrentDirectory ( int max, LPSTR dir )
        {
#ifdef WIN32
//            ::GetCurrentDirectory( max, dir );
			return "";
#else
            //getcwd ( dir, max );
#endif
            return dir ;
        }
        
        void filemanager::SetCurrentDirectory ( LPCSTR dir )
        {
#ifdef WIN32
//            ::SetCurrentDirectory( dir );
#else
            //	setcwd ( dir );
#endif
        }
        
        
        
    }
    // namespace os
}
// namespace tme

#endif
