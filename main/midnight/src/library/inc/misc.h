#ifndef _MISC_H_INCLUDED_
#define _MISC_H_INCLUDED_

#include "mxtypes.h"
#include "collections.h"

namespace chilli {
    namespace lib {

        using namespace types;
        
        void JumbleArray ( int* array, int max );
        int ConcatArray( int* array, int max );

        int     c_stricmp(LPCSTR dst, LPCSTR src);
        int     c_strnicmp ( LPCSTR first, LPCSTR last, size_t count );
        LPSTR   c_strdup ( LPCSTR s );
        LPSTR   c_strupr ( LPSTR text );
        LPSTR   c_strlwr ( LPSTR text );
        LPSTR   c_strcpy ( LPSTR dst, LPCSTR src );
        LPSTR   c_strcat ( LPSTR dst, LPCSTR src );
        size_t  c_strlen ( LPCSTR str );
        
        int     wildicmp(LPCSTR wild, LPCSTR src);
        int     wildcmp(LPCSTR wild, LPCSTR src);

        char     toupper ( char c );
        char     tolower ( char c );
        int      Compare ( s32 number1, s32 number2 );
        int      Compare ( f64 number1, f64 number2 );
        void     swap ( s32& num1, s32& num2 );
        void     swap ( u32& num1, u32& num2 );
        void     swap ( u8& num1, u8& num2 );
    
        
        u16      u16Swap (u16 l);
        u32      u32Swap (u32 l);
        u64      u64Swap (u64 l);
        f32      f32Swap (f32 l);
        f64      f64Swap (f64 l);

        void splitpath( LPCSTR path, LPSTR drive, LPSTR dir, LPSTR fname, LPSTR ext );
        void makepath( LPSTR path, LPCSTR drive, LPCSTR dir, LPCSTR fname, LPCSTR ext );

        u32 BSub(u32 value, u32 amount, u32 min);
        u32 BAdd(u32 value, u32 amount, u32 max);
        long atol( LPCSTR text );

        // std::string extensions
        using chilli::collections::c_string;

        namespace StringExtensions
        {
            archive& SerializeString ( archive& ar, std::string& string );
            long atol( const std::string& value );
            f32 atof( const std::string& value );
            s32 atoi( const std::string& value );
            int split ( const std::string& source, char delim, c_string& lines );
            c_string split_by_newline(const std::string& str);
            std::string right(std::string& input, int amount);
            std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
            std::string toUpper(const std::string& value);
            std::string Format ( LPCSTR format, ... );
            bool stringicompare(const std::string& str1, const std::string& str2);
            int GetToken ( const std::string& token, token_t array[], int max );

        }
    }
}

#if !defined(_UNIX_) && !defined(_LINUX_)
#define BYTEORDER64
#define BYTEORDER32
#define BYTEORDER16
#define BYTEORDERF32
#define BYTEORDERF64
#else
#define BYTEORDER64        lib::u64Swap
#define BYTEORDER32        lib::u32Swap
#define BYTEORDER16        lib::u16Swap
#define BYTEORDERF32    lib::f32Swap
#define BYTEORDERF64    lib::f32Swap
#endif

#define mklower(c)  ( (c)-'A'+'a' )
#define mkupper(c)  ( (c)-'a'+'A' )



#endif //_MISC_H_INCLUDED_
