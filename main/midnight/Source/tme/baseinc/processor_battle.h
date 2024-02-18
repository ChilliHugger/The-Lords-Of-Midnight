#ifndef _CBATTLEPROCESSOR_H_INCLUDED_
#define _CBATTLEPROCESSOR_H_INCLUDED_

#include "info.h"

namespace tme {

    class mxbattle
    {
    public:
        virtual void ProcessLocation ( mxgridref loc );
        virtual void Process ();
        virtual void Reset();
        virtual void Announce( mxgridref loc );
    protected:
        u32         battles;
        bitarray    areas;
    };
        
}

#endif //_CBATTLEPROCESSOR_H_INCLUDED_
