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
        
        virtual mxcharacter* IceCrownCarrier( void ) const ;
        virtual u32 CalcFearAdjuster( mxlocinfo* locinfo ) const;
        virtual void NightStop(void);
        virtual void NightStart(void);
        mxregiment* FindEmptyRegiment();

        virtual void initialise(u32 version);
        virtual void initialiseAfterCreate(u32 version);
        virtual void updateAfterLoad(u32 version);

    public:
        mxplace*        towerofdoom;
        mxcharacter*    morkin;

    private:
        bool            luxorAlive;
        bool            morkinAlive;
    };

    //namespace lom {
    
        class lom_entityfactory : public mxentityfactory
        {
        public:
            virtual mxentity* Create(id_type_t type) override;
        };
    
        // info classes
        class lom_stronghold  : public mxstronghold
        {
        public:
            lom_stronghold();
            virtual ~lom_stronghold();
            virtual u32 Remove ( mxrace_t race, mxunit_t type, u32 total );
            virtual u32 Add ( mxrace_t race, mxunit_t type, u32 total );
        };
    //}
    
    #define LOM_SCENARIO(x) static_cast<lom_x*>(mx->scenario)->x
}

#endif //_LOMSCENARIO_INTERNAL_H_INCLUDED_

