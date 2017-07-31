#ifndef _TYPES_H_INCLUDED
#define _TYPES_H_INCLUDED

#define _UNIX_


#define SAFEFREE(x)	if ( x ) { free(x); x=NULL; }
#define SAFEDELETE(x)	if ( x ) { delete x; x=NULL; }



typedef unsigned int        DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef signed int			LONG;
typedef char				CHAR;
typedef BYTE		        *LPBYTE;
typedef CHAR				*LPSTR;
typedef const CHAR			*LPCSTR;

#define MAX_PATH			260
#define MAX_DRIVE			260
#define MAX_EXT             260

#ifndef FALSE
#define FALSE               0
#endif
#ifndef TRUE
#define TRUE                1
#endif

typedef signed char			s8;
typedef unsigned char		u8;
typedef signed short		s16;
typedef unsigned short		u16;
typedef signed int			s32;
typedef unsigned int		u32;
typedef signed long long	s64;
typedef unsigned long long	u64;

typedef u32					flags32_t ;
typedef u16					flags16_t ;
typedef u8					flags8_t ;
typedef double				f64;
typedef float				f32;

typedef struct token_t {
	LPCSTR			token;
	int				value;
} token_t ;


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

#define BIT(x)				(1<<x)

#define NUMELE(a)       (sizeof(a)/sizeof(a[0]))
#define LASTELE(a)      ((a) + (NUMELE(a)-1))
#define TOOHIGH(a,p)    ((long)(p) - (long)(a) > (long)(NUMELE(a)-1))
#define TOOLOW(a,p)     ((long)(p) - (long)(a) < 0)
#define INBOUNDS(a,p)   ( ! (TOOHIGH(a,p) || TOOLOW(a,p)) )
#define MAX(a,b)  	(((a) > (b)) ? (a) : (b))
#define MIN(a,b)  	(((a) < (b)) ? (a) : (b))
#define ABS(a)   	((a) >= 0 ? (a) : -(a))
#define SIGN(a)		((a) > 0 ? 1 : -1)
#define RANGE(a,b,c)    ( (a) <= (b) && (b) <= (c) )
#define _IS(t,x) 	(((t)1 << (x)) != 0)   /* Evaluates true if the width of */
                        	                /* variable of type t is < x.     */
	                                        /* The != 0 assures that the      */
	                                        /* answer is 1 or 0               */

#define NBITS(t) 	(4 * (1 + _IS(t,4) + _IS(t,8) + _IS(t,12) + _IS(t,16) \
                        	 + _IS(t,20) + _IS(t,24) + _IS(t,28) + _IS(t,32)))

#define MAXINT          (((unsigned)~0) >> 1)


#define	FILLARRAY(x,y)		memset( x,  y, sizeof( x ) )
#define	FILLSTRUCT(x,y)		memset( &x,  y, sizeof( x ) )
#define FILLELEMENT(x,y)	memset( &x, y, sizeof( x ) )

#define	CLEARARRAY(x)		FILLARRAY(x,0)
#define	CLEARSTRUCT(x)		FILLSTRUCT(x,0)
#define CLEARELEMENT(x)		FILLELEMENT(x,0)

#define PERCENT(v,p)		( v * p ) / 100


#define PI	3.141592654358979323
#define PI2	(PI*2)
#define	DEGREETORAD(a)	(a*PI/180)
#define RADTODEGREE(a)	(a*(180/PI))

#define NONE			(0)


#endif //_TYPES_H_INCLUDED


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
