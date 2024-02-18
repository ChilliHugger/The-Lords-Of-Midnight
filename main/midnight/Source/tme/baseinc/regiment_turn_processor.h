//
//  regiment_turn_processor.h
//  midnight
//
//  Created by Chris Wild on 06/09/2023.
//

#ifndef regiment_turns_h
#define regiment_turns_h

#include "lomxtypes.h"
#include "info.h"

namespace tme {

    class RegimentTurnProcessor
    {
    public:
        RegimentTurnProcessor();
        virtual void Process ( mxregiment* regiment );

    // internal
    protected:
        virtual bool LookForInterestingLocationNearby();
        virtual void Target ( const mxitem* target );
        virtual void Next ( void );
        virtual void Complete ( void );
        virtual bool Retarget ( void );
        virtual void Goto ( void );
        virtual void Wander ( void );
        virtual void Follow ( void );
        virtual void Route ( void );

    protected:
        mxregiment*     regiment;
        u32             turns;
        mxgridref       targetlocation;

    // const
        const int REGIMENT_STOP_MOVE = 0;
    };
}

#endif /* regiment_turns_h */
