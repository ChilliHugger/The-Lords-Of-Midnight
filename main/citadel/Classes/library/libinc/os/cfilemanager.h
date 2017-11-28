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
	static bool Exists ( LPCSTR lpFileName ) ;
	static bool Get ( LPCSTR filename, PVOID lpBuffer, DWORD dwMaxSize) ;
	static PVOID Load ( LPCSTR filename, int* size=NULL ) ;
	static u32 Size ( LPCSTR filename ) ;
	static void Unload ( u8** ptr ) ;
	static bool Save( LPCSTR filename, const PVOID lpBuffer, DWORD dwSize ) ;
	static bool Remove(LPCSTR lpszFileName) ;
	static bool Rename(LPCSTR lpszOldName, LPCSTR lpszNewName) ;
    

};


#endif //_CFILEMANAGER_H_INCLUDED

