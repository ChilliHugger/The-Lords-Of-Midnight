#ifndef _DDR_NIGHTPROCESSOR_H_INCLUDED_
#define _DDR_NIGHTPROCESSOR_H_INCLUDED_

#include "../../baseinc/processor_night.h"

namespace tme {
    class ddr_night : public mxnight
    {
    public:
        ddr_night();
        virtual ~ddr_night();

        virtual void Process(void) override;
        virtual void MoveMidwinter () override;
        virtual void RemoveMidwinterFromMap();
        
        virtual void LordsProcessStart();
        virtual void LordsProcessEnd();

    protected:
        virtual void SetSpecialLocationsStrongholds() override;
        virtual void ResetLocationSpecial ( mxgridref loc ) override;

    };
}

#endif //_DDR_NIGHTPROCESSOR_H_INCLUDED_
