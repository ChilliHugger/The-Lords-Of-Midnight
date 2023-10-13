#ifndef _DDR_NIGHTPROCESSOR_H_INCLUDED_
#define _DDR_NIGHTPROCESSOR_H_INCLUDED_

#include "../../baseinc/processor_night.h"

namespace tme {
    class ddr_night : public mxnight
    {
    public:
        ddr_night();
        virtual ~ddr_night();

        virtual void Process(void);
        virtual void MoveMidwinter ();
        virtual void RemoveMidwinterFromMap();
     };
}

#endif //_DDR_NIGHTPROCESSOR_H_INCLUDED_
