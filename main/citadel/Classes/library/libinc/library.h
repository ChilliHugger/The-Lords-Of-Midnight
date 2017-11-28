#ifndef _LIBRARY_H_INCLUDED_
#define _LIBRARY_H_INCLUDED_

//#if !defined _MSC_VER
//#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundefined-bool-conversion"
#pragma clang diagnostic ignored "-Wno-tautological-compare"
//#endif



#if defined(__GNUC__) && ( defined(__APPLE_CPP__) || defined(__APPLE_CC__) || defined(__MACOS_CLASSIC__) )
#define _UNIX_
#define __MACOSX__
#endif


#ifdef _DEBUG_MXASSERT_VALID_
#define _MXASSERT_VALID(x)			x->AssertValid()
#define	_MXASSERT_VALID_IFUSED(x)		if (x) _MXASSERT_VALID(x)
#else
#define _MXASSERT_VALID(x)		((void)0)
#define	_MXASSERT_VALID_IFUSED	((void)0)
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
#define _MXASSERT(expr)	((void)0)
#define _MXASSERTE(expr)	((void)0)
#endif
		
namespace chilli {
	namespace lib {
		class archive;
	};
};


		#include "mxtypes.h"
		#include "../../tinyxml/tinyxml.h"

		#include "collections.h"

		#include "os/cfile.h"
		#include "sw/carchive.h"

		#include "sw/misc.h"

		#include "os/os.h"



		#ifdef _USEINLINE_
		#include "sw/librarysw.inl"
		#endif

		#include "misc/cmemfile.h"
		#include "misc/xml.h"


//#if !defined _MSC_VER
//#pragma clang diagnostic pop
//#endif



#endif //_LIBRARY_H_INCLUDED_
