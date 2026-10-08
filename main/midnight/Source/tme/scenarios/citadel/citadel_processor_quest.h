//
//  citadel_processor_quest.h
//  citadel
//

#ifndef _CITADEL_PROCESSOR_QUEST_H_INCLUDED_
#define _CITADEL_PROCESSOR_QUEST_H_INCLUDED_

#include "scenario_citadel_internal.h"

#include <vector>

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
        virtual bool Guard ( void );
        virtual void GatherStrength ( mxstronghold* stronghold, u32 needed );
        virtual void FreeHostages ( void );
        virtual void Done ( mxquestnews_t news );
        virtual void Order ( mxreaction_t reaction, mxquest_t quest, mxid target );

        bool Nearer ( mxstronghold* stronghold, mxstronghold* best ) const;
        bool FightsFor ( mxstronghold* stronghold ) const;
        mxstronghold* HomeKeep ( mxrace_t people, mxgridref from ) const;
        std::vector<mxregiment*> Host () const;
        mxregiment* Menace ( const std::vector<mxregiment*>& host, mxgridref where, s32 range ) const;
        template<typename T> T FirstReachable ( std::vector<T>& candidates ) const;

    protected:
        static constexpr u32 IMPATIENCE = 3;    // nights an impatient lord of yours waits before he sets out alone
        static constexpr s32 THREAT     = 4;    // how near Boroth's host comes before a keep is in danger
        static constexpr s32 SERVICE    = 12;   // how far a lord of the realms will march to lend you his service
        static constexpr s32 WANDERING  = 8;    // how far a wanderer strays on one journey

        citadel_character*  lord = nullptr;
        bool                blocked = false;    // March met the enemy in the road
    };
}

#endif // _CITADEL_
#endif // _CITADEL_PROCESSOR_QUEST_H_INCLUDED_
