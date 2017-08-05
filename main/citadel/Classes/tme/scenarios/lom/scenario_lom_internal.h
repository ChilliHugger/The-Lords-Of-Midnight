#ifndef _LOMSCENARIO_INTERNAL_H_INCLUDED_
#define _LOMSCENARIO_INTERNAL_H_INCLUDED_

#include "../../baseinc/tme_internal.h"
#include "../default/default_scenario_internal.h"

namespace tme {

	class lom_x : public mxscenario	
	{
	public:
		lom_x();
		virtual ~lom_x();
		
		virtual scenarioinfo_t* GetInfoBlock() const;
		virtual MXRESULT Register ( mxengine* midnightx );
		virtual MXRESULT UnRegister ( mxengine* midnightx );	
		virtual mxentity* CreateEntity ( id_type_t type );
		
		
		virtual mxcharacter* IceCrownCarrier( void ) const ;
		virtual u32 CalcFearAdjuster( mxlocinfo* locinfo ) const;
        virtual void NightStop(void);
        virtual void NightStart(void);
        mxregiment* FindEmptyRegiment();
        

    private:
        BOOL    luxorAlive;
        BOOL    morkinAlive;
	};


	//namespace lom {
		// info classes
		class lom_stronghold  : public mxstronghold
		{
		public:
			lom_stronghold();
			virtual ~lom_stronghold();
			virtual u32 Remove ( mxrace_t race, mxunit_t type, u32 total );
			
		};
	//}
	
}

#endif //_LOMSCENARIO_INTERNAL_H_INCLUDED_

