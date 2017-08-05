#ifndef _DEFAULT_SCENARIO_H_INCLUDED_
#define _DEFAULT_SCENARIO_H_INCLUDED_

namespace tme {

	class default_scenario  : public mxscenario_i
	{
	public:
		default_scenario();
		virtual ~default_scenario();
		
		scenarioinfo_t* GetInfoBlock() const;
		MXRESULT Register ( mxengine* midnightx );
		MXRESULT UnRegister ( mxengine* midnightx );	
		
		MXRESULT Command ( const string& arg, variant argv[], u32 argc );
		MXRESULT GetProperties ( const string& arg, variant argv[], u32 argc );
		MXRESULT Text ( const string& command, variant* argv=NULL, u32 args=0 );
		
		static MXRESULT MXAPI Create ( tme::mxinterface* mx );
	};
	

}// namespace tme


#endif //_DEFAULT_SCENARIO_H_INCLUDED_

