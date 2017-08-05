/*
 * FILE:    os.cpp
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

#include "../../libinc/library.h"
#include <stdlib.h>
#include <string.h>

namespace chilli {
	
	namespace os {

		static u32 m_dwPageSize ;

		void Sleep ( int mili )
		{
		#ifdef WIN32
			::Sleep(mili);
		#endif
		}

		/*  Assume that pb and cb are both rounded to page boundaries
		*  and that PageSize has been determined from GetSystemInformation().
		*/
		#define cbLag  (16 * m_dwPageSize)
		void MakeRegionPresent(volatile BYTE *pb, u32 cb)
		{
		u32 ib;  

		//	_ASSERTE(((DWORD)pb & (gdwPageSize - 1)) == 0);
		//	_ASSERTE(((DWORD)cb & (gdwPageSize - 1)) == 0);

			for (ib = cbLag; ib < cb; ib += m_dwPageSize) {
				pb[ib - cbLag];    
				pb[ib];
			}  
		}

	#ifdef WIN32
		/*
		* Function name	: SystemInfoCheck
		* 
		* Return type		: void
		* 
		* Arguments		: void
		* 
		* Description	    : 
		* 
		*/

		void SystemInfoCheck ( void )
		{
		SYSTEM_INFO	si;

			GetSystemInfo ( &si );
			m_dwPageSize = si.dwPageSize;

			//if ( si.dwProcessorType == PROCESSOR_INTEL_486 ||
			//	 si.dwProcessorType == PROCESSOR_INTEL_386 )
			//	 COMPLAIN( "A Pentium Processor is required to run this game" );

		}

		/*
		* Function name	: LoadLibrary
		* 
		* Return type		: HLIBRARY
		* 
		* Arguments		: LPCSTR filename
		* 
		* Description	    : 
		* 
		*/

		HLIBRARY LoadLibrary ( LPCSTR filename )
		{
			return ::LoadLibrary ( filename );
		}


		/*
		* Function name	: UnloadLibrary
		* 
		* Return type		: void
		* 
		* Arguments		: HLIBRARY hLib
		* 
		* Description	    : 
		* 
		*/

		void UnloadLibrary ( HLIBRARY hLib )
		{
			_ASSERTE ( hLib != NULL );
			FreeLibrary ( (struct HINSTANCE__ *)hLib );
		}


		/*
		* Function name	: GetFunctionAddress
		* 
		* Return type		: FUNCADDRESS
		* 
		* Arguments		: HLIBRARY hLib
		*                  : LPCSTR functionname
		* 
		* Description	    : 
		* 
		*/

		FUNCADDRESS GetFunctionAddress ( HLIBRARY hLib, LPCSTR functionname )
		{
			_ASSERTE ( hLib != NULL );
			return GetProcAddress( (struct HINSTANCE__ *)hLib, functionname );
		}
	#endif //WIN32

		void SetCurrentDirectory ( LPCSTR dir )
		{
		}
		
		LPSTR GetCurrentDirectory ( u32 max, LPSTR dir )
		{
			return (LPSTR)"" ;
		}

		char* Fullpath ( char *UserBuf, const char *path, u32 maxlen )
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
						chilli::lib::strcat(buf, path+1);
					}
					else
					{
						chilli::lib::strcat(buf, path);
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
				lib::strcpy(buf, path);
			}
			
			return buf;
			
		}
		
		
		
	}
	// namespace os
}
// namespace tme














/*





*/
