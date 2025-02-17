#ifndef _LOM_NOVEL_SCENARIO_INTERNAL_H_INCLUDED_
#define _LOM_NOVEL_SCENARIO_INTERNAL_H_INCLUDED_

#include "../../baseinc/tme_internal.h"
#include "../default/default_scenario_internal.h"
#include "../lom/scenario_lom_internal.h"

namespace tme {

    class lom_novel_x : public lom_x
    {
    public:
        lom_novel_x();
        virtual ~lom_novel_x();
        
        virtual scenarioinfo_t* GetInfoBlock() const;
        virtual MXRESULT Register ( mxengine* midnightx );
        virtual MXRESULT UnRegister ( mxengine* midnightx );
        
        virtual void updateAfterLoad(u32 version);
    };

    
    #define LOM_NOVEL_SCENARIO(x) static_cast<lom_novel_x*>(mx->scenario)->x
}

#endif //_LOM_NOVEL_SCENARIO_INTERNAL_H_INCLUDED_

