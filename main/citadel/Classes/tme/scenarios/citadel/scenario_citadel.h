#ifndef _CITADELSCENARIO_H_INCLUDED_
#define _CITADELSCENARIO_H_INCLUDED_

#include "../../tme.h"


namespace tme {

    namespace scenarios {
    
	class citadel  : public mxscenario_i
	{
	public:
		citadel();
		virtual ~citadel();

		scenarioinfo_t* GetInfoBlock() const;
		
		MXRESULT Command ( const string& arg, variant argv[], u32 argc );
		MXRESULT GetProperties ( const string& arg, variant argv[], u32 argc );
		MXRESULT Text ( const string& command, variant* argv=NULL, u32 args=0 );
		
		static MXRESULT MXAPI Create ( tme::mxinterface* mx );

	};

    }
	
}

#endif //_CITADELSCENARIO_H_INCLUDED_
