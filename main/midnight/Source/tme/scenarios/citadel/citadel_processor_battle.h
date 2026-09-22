#ifndef _CITADEL_BATTLEPROCESSOR_H_INCLUDED_
#define _CITADEL_BATTLEPROCESSOR_H_INCLUDED_

#include "../lom/lom_processor_battle.h"
#include "scenario_citadel_internal.h"

#if defined(_CITADEL_)
namespace tme {

    //
    // The Citadel fights Lords of Midnight's battle, but not everyone the night finds is at war.
    //
    class citadel_battle : public lom_battle
    {
    protected:
        virtual bool HasDefenders() const override;
        virtual bool TakesPart ( const mxarmy* army ) const override;
        virtual bool TakesPart ( const mxcharacter* character ) const override;
    };

}
#endif // _CITADEL_

#endif //_CITADEL_BATTLEPROCESSOR_H_INCLUDED_
