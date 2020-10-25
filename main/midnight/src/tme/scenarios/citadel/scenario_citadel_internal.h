#ifndef _CITADELSCENARIO_INTERNAL_H_INCLUDED_
#define _CITADELSCENARIO_INTERNAL_H_INCLUDED_

#include "../../baseinc/tme_internal.h"
#include "../default/default_scenario_internal.h"

namespace tme {

    namespace scenarios {
    
    class citadel_x : public mxscenario
    {
    public:
        citadel_x();
        virtual ~citadel_x();
        
        virtual scenarioinfo_t* GetInfoBlock() const;
        virtual MXRESULT Register ( mxengine* midnightx );
        virtual MXRESULT UnRegister ( mxengine* midnightx );    
        virtual mxentity* CreateEntity ( id_type_t type );
        
    };

    }
    
}

#endif //_CITADELSCENARIO_INTERNAL_H_INCLUDED_

