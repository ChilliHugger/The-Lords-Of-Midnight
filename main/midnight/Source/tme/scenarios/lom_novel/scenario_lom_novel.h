#ifndef _LOM_NOVEL_SCENARIO_H_INCLUDED_
#define _LOM_NOVEL_SCENARIO_H_INCLUDED_

#include "../../tme.h"
#include "../lom/scenario_lom.h"


namespace tme {

    class lom_novel  : public lom
    {
    public:
        lom_novel();
        virtual ~lom_novel();

        virtual scenarioinfo_t* GetInfoBlock() const override ;
                
        static MXRESULT MXAPI Create ( tme::mxinterface* mx );

    };
}

#endif //_LOM_NOVEL_SCENARIO_H_INCLUDED_
