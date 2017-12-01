#ifndef _LIBRARY_H_INCLUDED_
#define _LIBRARY_H_INCLUDED_

//#if !defined _MSC_VER
//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wundefined-bool-conversion"
//#pragma clang diagnostic ignored "-Wno-tautological-compare"
//#endif



#if defined(__GNUC__) && ( defined(__APPLE_CPP__) || defined(__APPLE_CC__) || defined(__MACOS_CLASSIC__) )
#define _UNIX_
#define __MACOSX__
#endif

		#include "mxtypes.h"
		#include "../../tinyxml/tinyxml.h"
		#include "collections.h"
		#include "os/cfile.h"
		#include "sw/carchive.h"
		#include "sw/misc.h"
		#include "os/os.h"
		#include "misc/xml.h"


//#if !defined _MSC_VER
//#pragma clang diagnostic pop
//#endif



#endif //_LIBRARY_H_INCLUDED_
