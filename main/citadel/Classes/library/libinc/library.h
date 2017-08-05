#ifndef _LIBRARY_H_INCLUDED_
#define _LIBRARY_H_INCLUDED_

#define _UNIX_


#ifdef _DEBUG_ASSERT_VALID_
#define _ASSERT_VALID(x)			x->AssertValid()
#define	_ASSERT_VALID_IFUSED(x)		if (x) _ASSERT_VALID(x)
#else
#define _ASSERT_VALID(x)		((void)0)
#define	_ASSERT_VALID_IFUSED	((void)0)
#endif


#if defined(_DEBUG_ASSERT_) && defined(BREAKPOINT)
/* Asserts */
#define _ASSERT(expr) \
        do { if (!(expr) && \
                (1 == AssertMsg( __FILE__, __LINE__, NULL))) \
             BREAKPOINT; } while (0)

#define _ASSERTE(expr) \
        do { if (!(expr) && \
                (1 == AssertMsg( __FILE__, __LINE__, #expr))) \
             BREAKPOINT; } while (0)
#else
#define _ASSERT(expr)	((void)0)
#define _ASSERTE(expr)	((void)0)
#endif
		
namespace chilli {
	namespace lib {
		class archive;
	};
};


		#include "mxtypes.h"
		#include "tinyxml.h"

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





#endif //_LIBRARY_H_INCLUDED_
