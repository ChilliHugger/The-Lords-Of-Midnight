#ifndef _DDRSCENARIO_H_INCLUDED_
#define _DDRSCENARIO_H_INCLUDED_


#include "../../tme.h"

#if defined(_DDR_)
namespace tme {
	
	class ddr  : public mxscenario_i
	{
	public:
		ddr();
		virtual ~ddr();
		
		scenarioinfo_t* GetInfoBlock() const;
		
		MXRESULT Command ( const c_str& arg, variant argv[], u32 argc );
		MXRESULT GetProperties ( const c_str& arg, variant argv[], u32 argc );
		MXRESULT Text ( const c_str& command, variant* argv=NULL, u32 args=0 );
		
		static MXRESULT MXAPI Create ( tme::mxinterface* mx );
	};
}

#endif // _DDR_

#endif //_CDDRSCENARIO_H_INCLUDED_
