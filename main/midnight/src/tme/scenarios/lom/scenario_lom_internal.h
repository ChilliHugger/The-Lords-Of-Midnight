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
        
        virtual scenarioinfo_t* GetInfoBlock() const override;
        virtual MXRESULT Register ( mxengine* midnightx ) override;
        virtual MXRESULT UnRegister ( mxengine* midnightx ) override;
        virtual mxentity* CreateEntity ( id_type_t type ) override;
        virtual u32 CalcFearAdjuster( mxlocinfo* locinfo ) const override;
        virtual void NightStop(void) override;
        virtual void NightStart(void) override;
        
        virtual void initialiseAfterCreate(u32 version) override;
        virtual void updateAfterLoad ( u32 version ) override;

        virtual mxcharacter* IceCrownCarrier( void ) const ;
        mxregiment* FindEmptyRegiment();

    private:
        bool    luxorAlive;
        bool    morkinAlive;
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

