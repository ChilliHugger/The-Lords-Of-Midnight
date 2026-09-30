//
//  citadel_processor_quest.h
//  citadel
//

#ifndef _CITADEL_PROCESSOR_QUEST_H_INCLUDED_
#define _CITADEL_PROCESSOR_QUEST_H_INCLUDED_

#include "scenario_citadel_internal.h"

#if defined(_CITADEL_)

namespace tme {

    class citadel_quest_processor
    {
    public:
        virtual ~citadel_quest_processor() = default;

        virtual void Process ( citadel_character* lord );
        virtual void React ( citadel_character* lord );

    protected:
        virtual void Quest ( void );
        virtual bool March ( mxgridref target, bool fight );
        virtual void Attack ( void );

    protected:
        citadel_character*  lord = nullptr;
    };
}

#endif // _CITADEL_
#endif // _CITADEL_PROCESSOR_QUEST_H_INCLUDED_
