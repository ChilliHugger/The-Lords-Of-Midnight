#ifndef _MXTYPES_H_INCLUDED_
#define _MXTYPES_H_INCLUDED_

// chilli

#if defined(__GNUC__) && ( defined(__APPLE_CPP__) || defined(__APPLE_CC__) || defined(__MACOS_CLASSIC__) )
#define _UNIX_
#define __MACOSX__
#endif

#if defined _MSC_VER
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#include <string>
#endif

#ifdef __ANDROID__
#define _UNIX_
#endif

#include <functional>

typedef std::function<void(void)> MXVoidCallback;

#define IS_NOT_NULL(x,y,z) \
    (x==nullptr) ? z : y

#define IF_NOT_NULL(x) \
    if ( (x) != nullptr ) (x)

#define CONTINUE_IF( x )  \
    if ( (x) ) continue

#define CONTINUE_IF_NULL( x ) \
    CONTINUE_IF( (x) == nullptr )

#define RETURN_IF_NULL( x )  \
    if ( (x) == null ) return

#define BREAK_IF( x )  \
    if ( (x) ) break

#define BREAK_IF_NULL( x )  \
    BREAK_IF( (x) == nullptr )

#define FORWARD_REFERENCE(x) \
    class x

#define SAFEFREE(x)    \
    if ( x ) { free(x); x=nullptr; }

#define SAFEDELETE(x) \
    if ( x ) { delete x; x=nullptr; }

#define SAFERELEASE(x) \
    if ( x ) { x->Release(); x=nullptr; }

#define SAFEDELETEARRAY(x) \
    if ( x ) { delete [] x; x=nullptr; }

#define GET_PROPERTY(t,n,m)\
    t n() const { return m; } 

#define SET_PROPERTY(t,n,m)\
    void n ( t value ) { m = value ; }

#define FLAG_PROPERTY( n, f ) \
    GET_FLAG_PROPERTY(n,f)

#define GET_FLAG_PROPERTY( n, f ) \
    bool n() const { return flags.Is(f); }

#define SET_FLAG_PROPERTY( n, f ) \
    void n()  { flags.Set(f); }

#define CLR_FLAG_PROPERTY( n, f ) \
    void n()  { flags.Reset(f); }

#define _SET_FLAG_PROPERTY( c, n, f ) \
    c* n()  { flags.Set(f); return this; }

#define _CLR_FLAG_PROPERTY( c, n, f ) \
    c* n()  { flags.Reset(f); return this; }


#define PROPERTY(t,n,m)\
    GET_PROPERTY(t,n,m)\
    SET_PROPERTY(t,n,m)

#define _USEINLINE_
#ifndef _USEINLINE_
    #define    MXINLINE
#else
    #define MXINLINE inline
#endif



// defines usually taken from windows headers
//
#ifndef WIN32_LEAN_AND_MEAN
typedef unsigned int        DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef signed int            LONG;
typedef char                CHAR;
typedef BYTE                *LPBYTE;
typedef CHAR                *LPSTR;
typedef const CHAR            *LPCSTR;
#define MAX_PATH            260
#ifndef FALSE
#define FALSE               0
#endif
#ifndef TRUE
#define TRUE                1
#endif
#endif

#define MXAPI 
//#define MXAPI __stdcall

#include <string>

namespace chilli {
    namespace lib {
        class archive;
    }
}

namespace chilli {
    
    namespace types {

        using namespace lib;
        
        // forward references 
        class size;
        class point;
        class rect;
        //

        typedef signed char            s8;
        typedef unsigned char        u8;
        typedef signed short        s16;
        typedef unsigned short        u16;
        typedef signed int            s32;
        typedef unsigned int        u32;

        // define int64 signed and unsigned
        #if defined(_UNIX_) || defined(_LINUX_)
            typedef signed long long    s64;
            typedef unsigned long long    u64;
        #else
//            typedef uint64_t u64;
//            typedef int64_t s64;

            typedef unsigned __int64    u64;
            typedef signed __int64        s64;
        #endif
        typedef u32                    flags32_t ;
        typedef u16                    flags16_t ;
        typedef u8                    flags8_t ;
        typedef double                f64;
        typedef float                f32;
        typedef u32                    mxid ;

        typedef struct token_t {
            LPCSTR            token;
            int                value;
        } token_t ;

        typedef struct range_t {
            int                min;
            int                max;
        } range_t ;
    
        // empty
        class  empty
        {
        public:
            empty();
            virtual ~empty();
        };

        // range
        class  range : public range_t
        {
        public:
            range();
            ~range();
            range( int min, int max );
            bool IsInRange( int value );
            int Range() const ;

            range& operator= ( range_t value ) ;

            friend  archive& operator<<(archive& ar, const range& value);
            friend  archive& operator>>(archive& ar, range& value);
        };

        // bitarray
        class  bitarray 
        {
        public:

            ~bitarray();
            bitarray();
            bitarray ( int nElements );
            int Size() const;
            void ClrAll();
            void SetAll();
            void Attatch ( u8* pData, int nElements );
            void Detatch();
            void Destroy();
            bool operator[](int bit) const;

            bool Create ( int nElements );
            void Set ( int bit );
            void Clr ( int bit );
            void Xor ( int bit );
            bool Get ( int bit ) const;

            archive& Serialize ( archive& ar );
            friend  archive& operator<<(archive& ar, bitarray& array );
            friend  archive& operator>>( archive& ar, bitarray& array );


        private:
            u8    *m_pData;
            int    m_nElements;
        };

        
        MXINLINE bool bitarray::operator[](int bit) const                { return Get(bit); }
        MXINLINE bitarray::bitarray()                                    { Detatch(); }
        MXINLINE bitarray::bitarray ( int nElements )                    { Create (nElements ); }
        MXINLINE bitarray::~bitarray()                                { Destroy(); }
        MXINLINE void bitarray::ClrAll()                                { memset( m_pData, 0x00, Size() ); }
        MXINLINE void bitarray::SetAll()                                { memset( m_pData, 0xff, Size() ); }
        MXINLINE void bitarray::Attatch ( u8* pData, int nElements )    { m_pData = pData; m_nElements = nElements ; }
        MXINLINE void bitarray::Detatch()                                { m_pData = NULL; m_nElements = 0 ;}
        MXINLINE void bitarray::Destroy()                                { if ( m_pData ) delete m_pData ;    m_pData = NULL;    m_nElements = 0; }
        MXINLINE archive& operator<<( archive& ar, bitarray& array )    { return array.Serialize(ar); }
        MXINLINE archive& operator>>( archive& ar, bitarray& array )    { return array.Serialize(ar); }

        
        
        // flags
        class  flags32
        {
        public:
            flags32();
            ~flags32();
            void Set ( u32 flags ) ;
            void Reset ( u32 flags ) ;
            void Toggle ( u32 flags );
            void Clear()                                    { m_flags = 0; }
            bool Is ( u32 flag ) const ;
            void ShiftLeft()                                { m_flags <<=1; }
            void ShiftRight()                               { m_flags >>=1; }
            archive& Serialize ( archive& ar );
            operator u32() const ;
            friend  archive& operator<<(archive& ar, flags32& flags );
            friend  archive& operator>>( archive& ar, flags32& flags );
        private:
            u32        m_flags;
        };

        class  flags8  
        {
        public:
            flags8();
            ~flags8();
            void Set ( u8 flags ) ;
            void Reset ( u8 flags ) ;
            void Toggle ( u32 flags );
            void Clear()                                    { m_flags = 0; }
            bool Is ( u8 flag ) const ;
            void ShiftLeft()                                { m_flags <<=1; }
            void ShiftRight()                               { m_flags >>=1; }
            archive& Serialize ( archive& ar );
            operator u8() const ;
            friend  archive& operator<<(archive& ar, flags8& flags );
            friend  archive& operator>>( archive& ar, flags8& flags );
        private:
            u8        m_flags;
        };

        class  flags16  
        {
        public:
            flags16();
            ~flags16();
            void Set ( u16 flags ) ;
            void Reset ( u16 flags ) ;
            void Toggle ( u32 flags );
            bool Is ( u16 flag ) const ;
            void ShiftLeft()                                { m_flags <<=1; }
            void ShiftRight()                               { m_flags >>=1; }
            void Clear()                                    { m_flags = 0; }
            archive& Serialize ( archive& ar );
            operator u16() const ;
            friend  archive& operator<<(archive& ar, flags16& flags );
            friend  archive& operator>>( archive& ar, flags16& flags );
        private:
            u16        m_flags;
        };
        
        
        /* flags32 */
        MXINLINE flags32::flags32()                                    { m_flags = 0 ; }
        MXINLINE void flags32::Set ( u32 f )                        { m_flags |= f ; }
        MXINLINE void flags32::Reset ( u32 f )                        { m_flags &= ~f ; }
        MXINLINE void flags32::Toggle ( u32 f )                     { if ( Is(f) ) Reset(f); else Set(f); }
        MXINLINE bool flags32::Is ( u32 f) const                    { return (m_flags&f)==f ? TRUE : FALSE  ; }
        MXINLINE flags32::operator u32() const                        { return m_flags; }
        MXINLINE archive& operator<<( archive& ar, flags32& f )        { return f.Serialize(ar); }
        MXINLINE archive& operator>>( archive& ar, flags32& f )        { return f.Serialize(ar); }
        
        /* flags8 */
        MXINLINE flags8::flags8()                                    { m_flags = 0 ; }
        MXINLINE void flags8::Set ( u8 f )                            { m_flags |= f ; }
        MXINLINE void flags8::Reset ( u8 f )                        { m_flags &= ~f ; }
        MXINLINE void flags8::Toggle ( u32 f )                      { if ( Is(f) ) Reset(f); else Set(f); }
        MXINLINE bool flags8::Is ( u8 f ) const                        { return (m_flags&f)==f ? TRUE : FALSE  ; }
        MXINLINE flags8::operator u8() const                        { return m_flags; }
        MXINLINE archive& operator<<( archive& ar, flags8& f )        { return f.Serialize(ar); }
        MXINLINE archive& operator>>( archive& ar, flags8& f )        { return f.Serialize(ar); }
        
        /* flags16 */
        MXINLINE flags16::flags16()                                    { m_flags = 0 ; }
        MXINLINE void flags16::Set ( u16 f )                        { m_flags |= f ; }
        MXINLINE void flags16::Reset ( u16 f )                        { m_flags &= ~f ; }
        MXINLINE void flags16::Toggle ( u32 f )                     { if ( Is(f) ) Reset(f); else Set(f); }
        MXINLINE bool flags16::Is ( u16 f ) const                    { return (m_flags&f)==f ? TRUE : FALSE  ; }
        MXINLINE flags16::operator u16() const                        { return m_flags; }
        MXINLINE archive& operator<<( archive& ar, flags16& f )        { return f.Serialize(ar); }
        MXINLINE archive& operator>>( archive& ar, flags16& f )        { return f.Serialize(ar); }

        // str
        class  c_str
        {
        public:
            c_str();
            c_str ( LPSTR str );
            c_str ( LPCSTR str );

            ~c_str();

            operator LPSTR() const;
            operator LPCSTR() const;

            size_t Length() const ;

            const c_str&  operator= ( LPCSTR value );
            const c_str&  operator= ( const c_str& str );

            archive& Serialize ( archive& ar );
            friend  archive& operator<<( archive& ar, c_str& value );
            friend  archive& operator>>( archive& ar, c_str& value );

            static archive& SerializeString ( archive& ar, std::string& string );

            bool IsNull() const;
            bool IsEmpty() const;

            LPCSTR GetAt() const { return m_pchData; }

        private:
            LPSTR    m_pchData;
        };

        extern c_str    strNull;
        
        
        MXINLINE archive& operator<<( archive& ar, c_str& s )        { return s.Serialize(ar); }
        MXINLINE archive& operator>>( archive& ar, c_str& s )        { return s.Serialize(ar); }
        MXINLINE archive& operator<<( archive& ar, std::string& s )  { return c_str::SerializeString(ar, s); }
        MXINLINE archive& operator>>( archive& ar, std::string& s )  { return c_str::SerializeString(ar, s); }

        MXINLINE c_str::operator LPSTR() const                              { return m_pchData; }
        MXINLINE c_str::operator LPCSTR() const                             { return m_pchData; }


        // random
        class randomno
        {
        public:
            randomno();
            ~randomno();

            void srand ( u32 seed );
            void seed ( u32 seed );
            int seed();
            int get ( void );
            int get ( int number );
            int get ( int lower, int upper );
            f32 getfloat();
            //bool chance ( int chance );
            bool chance ( f32 chance );
            void randomize();

        public:
            u32 m_seed;
            static randomno instance ;
        };


        class variant 
        {
        public:

            variant() 
            {
                vType=none;
                vDouble=0;
            }

            ~variant() {}

            enum var_t {
                none    =    0,
                vnumber    =    1,
                vstring    =    2,
                vyesno    =    3,
                vfloat    =    4,
                vdouble    =    5,
                vptr    =    6,
                vid    =    7,
            };


            s32& operator = ( s32 value )
                { vType=vnumber;vInt = value ; return vInt; }
            mxid& operator = ( mxid value )
                { vType=vid;vId = value ; return vId; }
            f32& operator = ( f32 value )
                { vType=vfloat;vFloat = value ; return vFloat; }
            f64& operator = ( f64 value )
                { vType=vdouble;vDouble = value ; return vDouble; }
            LPSTR& operator = ( LPSTR value )
                { vType=vstring;vString = value ; return vString; }
            LPSTR& operator = ( c_str value )
                { vType=vstring;vString = (LPSTR)value ; return vString; }
            LPSTR& operator = ( std::string value )
                {vType=vstring;vString = (LPSTR)value.c_str(); return vString; }
                    
            void*& operator = ( void* value )
                { vType=vptr;
            vPtr = value ; 
            return vPtr; }


            operator s32() const
                { return vInt; }
            operator mxid() const
                { return vId; }
            operator f32() const
                { return vFloat; }
            operator f64() const
                { return vDouble; }
            operator char*() const
                { return vString; }
            operator c_str() const
                { return vString; }
            operator void*() const
                { return vPtr; }
                
        public:
            var_t                vType;
            union {
                s32                vInt;
                mxid            vId;
                f64                vDouble;
                f32                vFloat;
                char*            vString;
                void*            vPtr;
            };
        };



    }

}

/****************************************************************************
*
*   NUMELE(array)       Evaluates to the array size in elements
*   LASTELE(array)      Evaluates to a pointer to the last element
*   INBOUNDS(array,p)   Evaluates to true if p points into the array
*   RANGE(a,b,c)        Evaluates to true if a <= b <= c
*   MAX(a,b)            Evaluates to a or b, whichever is larger
*   MIN(a,b)            Evaluates to a or b, whichever is smaller
*   ABS(a)              Evaluates to the absolute value of a
*   NBITS(type)         Returns the number of bits in a variable of the
*                       indicated type
*   MAXINT              Evaluates to the value of the largest signed integer
*
****************************************************************************/

#define MXBIT(x)                (1<<x)
//#ifndef MAKEWORD
//#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
//#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
//#define LOWORD(l)           ((WORD)(l))
//#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
//#define LOBYTE(w)           ((BYTE)(w))
//#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
//#endif

#define MAKEBYTE(a,b)        (a|(b<<4))
#define LONIBBLE(w)         (w&0x0f)
#define HINIBBLE(w)         ((w&0xf0)>>4)

#define NUMELE(a)            (sizeof(a)/sizeof(a[0]))
#define LASTELE(a)            ((a) + (NUMELE(a)-1))
#define TOOHIGH(a,p)        ((long)(p) - (long)(a) > (long)(NUMELE(a)-1))
#define TOOLOW(a,p)            ((long)(p) - (long)(a) < 0)
#define INBOUNDS(a,p)        ( ! (TOOHIGH(a,p) || TOOLOW(a,p)) )
//#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
//#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#ifndef ABS
#define ABS(a)                ((a) >= 0 ? (a) : -(a))
#endif
#define SIGN(a)                ((a) > 0 ? 1 : -1)
#define RANGE(a,b,c)        ( (a) <= (b) && (b) <= (c) )
#define _IS(t,x)            (((t)1 << (x)) != 0)   /* Evaluates true if the width of */
                                            /* variable of type t is < x.     */
                                            /* The != 0 assures that the      */
                                            /* answer is 1 or 0               */

#define NBITS(t)            (4 * (1 + _IS(t,4) + _IS(t,8) + _IS(t,12) + _IS(t,16) \
                             + _IS(t,20) + _IS(t,24) + _IS(t,28) + _IS(t,32)))
//#define MAXINT                (((unsigned)~0) >> 1)
#define    FILLARRAY(x,y)        memset( x,  y, sizeof( x ) )
#define    FILLSTRUCT(x,y)        memset( &x,  y, sizeof( x ) )
#define FILLELEMENT(x,y)    memset( &x, y, sizeof( x ) )
#define    CLEARARRAY(x)        FILLARRAY(x,0)
#define    CLEARSTRUCT(x)        FILLSTRUCT(x,0)
#define CLEARELEMENT(x)        FILLELEMENT(x,0)

#define PERCENT(v,p)        ((v*p)/100.0)
#define PERCENTAGEOF(v,t)    ((v*100.0)/t)
#define PERCENTAGEDIFF(a,b)    PERCENTAGEOF((a-b),b)

#define ROUNDFLOAT(rating)    (((rating*2.0)+(1.0*SIGN(rating)))/2.0)
#define ROUNDDOWN(rating)    ((int)rating)


#define MX_PI                3.141592654358979323
#define MX_PI2                (MX_PI*2)
#define    DEGREETORAD(a)        (a*MX_PI/180)
#define RADTODEGREE(a)        (a*(180/MX_PI))

#define    STRUC_OFFSET(x,y)    ((int)&(((x*)0)->y))
#define    CLASS_OFFSET(x,y)    ((int)&(((x*)0)->y))

#ifdef _DEBUG_MXASSERT_VALID_
    #define _MXASSERT_VALID(x)            x->AssertValid()
    #define    _MXASSERT_VALID_IFUSED(x)        if (x) _MXASSERT_VALID(x)
#else
    #define _MXASSERT_VALID(x)        ((void)0)
    #define    _MXASSERT_VALID_IFUSED    ((void)0)
#endif


#if defined(_DEBUG_MXASSERT_) && defined(BREAKPOINT)
    /* Asserts */
    #define _MXASSERT(expr) \
        do { if (!(expr) && \
        (1 == AssertMsg( __FILE__, __LINE__, NULL))) \
        BREAKPOINT; } while (0)

    #define _MXASSERTE(expr) \
        do { if (!(expr) && \
        (1 == AssertMsg( __FILE__, __LINE__, #expr))) \
        BREAKPOINT; } while (0)
#else
    #define _MXASSERT(expr)    ((void)0)
    #define _MXASSERTE(expr)    ((void)0)
#endif


#include "drawing.h"

#endif //_DATATYPES_H_INCLUDED_


#if defined _MSC_VER

#pragma warning( disable : 4244 ) // Level 4
/* 'conversion' conversion from 'type1' to 'type2', possible loss of data
 * An integral type was converted to a smaller integral type. This is a level-4 warning if type2 is int and the size of type1 is smaller than int. Otherwise it is a level-3 warning.
 * The conversion may have due to implicit integral conversions. For example:
 *   
 * short a, b, c;
 * a = b + c;    // warning
 */  

#pragma warning( disable : 4245 ) // Level 3
/* 'conversion' : conversion from 'type1' to 'type2', signed/unsigned mismatch
 * You attempted to convert a signed const that has a negative value to an unsigned.
 * The following code shows this warning:
 *   
 * const int i = -1;
 * unsigned int j = i; // warning C4245
 * 
 * const int k = 1;
 * unsigned int l = k; // okay
 * 
 * int m = -1;
 * unsigned int n = m; // okay
 */  

#pragma warning( disable : 4018 ) // Level 3
/* 'expression' : signed/unsigned mismatch
 * There was an attempt to compare a signed and unsigned number. The signed value was converted to an unsigned type for the comparison.
 * When performing an equal (= =) or not equal (!=) comparison between signed and unsigned types, cast one type to the other to ensure proper comparison.
 */

#pragma warning( disable : 4761 ) // Level 1
/* integral size mismatch in argument : conversion supplied
 * The base types of the actual and formal parameters of a function were different.
 * The compiler converted the actual parameter to the type of the formal parameter.
 */

#pragma warning( disable : 4200 ) // Level 2 & 4
/* nonstandard extension used : zero-sized array in struct/union
 * A structure or union contained an array with zero size.
 * This is a level-4 warning when Microsoft extensions are enabled (/Ze) and a level-2 warning when Microsoft extensions are disabled (/Za).
 * The following is an example of this error:
 *   
 * struct S { int a[0]; };  // warning
 */  

#ifdef _DREAMCAST
#pragma warning( disable : 4305 ) // Level 1
/* Compiler Warning (level 1) C4305
 * 'identifier' : truncation from 'type1' to 'type2'
 *
 * The specified identifier type was converted to a smaller type. Information was lost in the casting.
 */
#endif

#endif //_UNIX_

// can't imagine any case where you would include this file
// and not want the namespace
using namespace chilli::types;



