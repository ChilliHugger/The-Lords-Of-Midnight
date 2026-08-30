#ifndef _CITADELSCENARIO_INTERNAL_H_INCLUDED_
#define _CITADELSCENARIO_INTERNAL_H_INCLUDED_

#include "../../baseinc/tme_internal.h"
#include "../default/default_scenario_internal.h"

namespace tme {

    class citadel_x : public mxscenario
    {
    public:
        citadel_x();
        virtual ~citadel_x();

        virtual scenarioinfo_t* GetInfoBlock() const;
        virtual MXRESULT Register ( mxengine* midnightx );
        virtual MXRESULT UnRegister ( mxengine* midnightx );

        virtual void initialise ( u32 version );
        virtual void initialiseAfterCreate ( u32 version );

    public:
        // The Dark Citadel of Maranor, seat of Boroth the Wolfheart. Taking
        // it, and killing Boroth, is the object of the game.
        mxstronghold*   darkCitadel;
        mxcharacter*    boroth;
    };

    #define CITADEL_SCENARIO(x) static_cast<citadel_x*>(mx->scenario)->x
}

#endif //_CITADELSCENARIO_INTERNAL_H_INCLUDED_
