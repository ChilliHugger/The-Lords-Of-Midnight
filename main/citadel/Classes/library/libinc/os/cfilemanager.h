#ifndef _CFILEMANAGER_H_INCLUDED
#define _CFILEMANAGER_H_INCLUDED

#include "types.h"
#include "datatypes.h"


/* the file size is greater than the buffer
 * buffer read will be truncated
 */
#define	FILEIO_BUFFER_EXCEEDED		(u32)SETERROR( ERR_WARNING, 1, 1)

class  CFileManager
{

public:
	static BOOL Exists ( LPCSTR lpFileName ) ;
	static BOOL Get ( LPCSTR filename, PVOID lpBuffer, DWORD dwMaxSize) ;
	static PVOID Load ( LPCSTR filename, int* size=NULL ) ;
	static u32 Size ( LPCSTR filename ) ;
	static void Unload ( u8** ptr ) ;
	static BOOL Save( LPCSTR filename, const PVOID lpBuffer, DWORD dwSize ) ;
	static BOOL Remove(LPCSTR lpszFileName) ;
	static BOOL Rename(LPCSTR lpszOldName, LPCSTR lpszNewName) ;
    

};


#endif //_CFILEMANAGER_H_INCLUDED

