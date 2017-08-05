#ifndef _MISC_H_INCLUDED_
#define _MISC_H_INCLUDED_

#include "chilli.h"

using namespace chilli;

namespace chilli {
	namespace lib {

		
				
		int Split ( LPCSTR source, LPCSTR delim, collections::c_string& tokens ) ;
		void JumbleArray ( int* array, int max );
		int ConcatArray( int* array, int max );

		LPSTR	strtok_r(LPSTR str, LPCSTR delim, char **nextp);

		int		stricmp(LPCSTR dst, LPCSTR src);
		int		strnicmp ( LPCSTR first, LPCSTR last, size_t count );
		LPSTR	strdup ( LPCSTR s );
		LPSTR	strupr ( LPSTR text );
		LPSTR	strlwr ( LPSTR text );
		LPSTR	strcpy ( LPSTR dst, LPCSTR src );
		LPSTR	strcat ( LPSTR dst, LPCSTR src );
		size_t	strlen ( LPCSTR str );
		
		int wildicmp(LPCSTR wild, LPCSTR src);
		int wildcmp(LPCSTR wild, LPCSTR src);

		char	toupper ( char c );
		char	tolower ( char c );
		int		Compare ( s32 number1, s32 number2 );
		int		Compare ( f64 number1, f64 number2 );
		void	swap ( s32& num1, s32& num2 );
		void	swap ( u32& num1, u32& num2 );
		void	swap ( u8& num1, u8& num2 );
	
		
		u16		u16Swap (u16 l);
		u32		u32Swap (u32 l);
		u64		u64Swap (u64 l);
		f32		f32Swap (f32 l);
		f64		f64Swap (f64 l);

		void splitpath( LPCSTR path, LPSTR drive, LPSTR dir, LPSTR fname, LPSTR ext );
		void makepath( LPSTR path, LPCSTR drive, LPCSTR dir, LPCSTR fname, LPCSTR ext );
		int GetToken ( LPCSTR token, token_t array[], int max );

		u32 BSub(u32 value, u32 amount, u32 min);
		u32 BAdd(u32 value, u32 amount, u32 max);

		BOOL isNumber( LPCSTR string );
		long atol( LPCSTR text );

	}
}

#if !defined(_UNIX_) && !defined(_LINUX_)
#define BYTEORDER64
#define BYTEORDER32
#define BYTEORDER16
#define BYTEORDERF32
#define BYTEORDERF64
#else
#define BYTEORDER64		lib::u64Swap
#define BYTEORDER32		lib::u32Swap
#define BYTEORDER16		lib::u16Swap
#define BYTEORDERF32	lib::f32Swap
#define BYTEORDERF64	lib::f32Swap
#endif

#define mklower(c)  ( (c)-'A'+'a' )
#define mkupper(c)  ( (c)-'a'+'A' )



#endif //_MISC_H_INCLUDED_
