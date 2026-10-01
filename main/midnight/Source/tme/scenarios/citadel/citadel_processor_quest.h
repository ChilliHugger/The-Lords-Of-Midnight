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
        virtual void Impatience ( citadel_character* lord );

    protected:
        virtual void Defend ( void );
        virtual void Wander ( void );
        virtual void Quest ( void );
        virtual bool March ( mxgridref target, bool fight );
        virtual void Attack ( void );
        virtual bool Guard ( void );
        virtual void GatherStrength ( mxstronghold* stronghold, u32 needed );
        virtual void FreeHostages ( void );
        virtual void Done ( mxquestnews_t news );

    protected:
        citadel_character*  lord = nullptr;
        bool                blocked = false;    // March met the enemy in the road
    };
}

#endif // _CITADEL_
#endif // _CITADEL_PROCESSOR_QUEST_H_INCLUDED_
