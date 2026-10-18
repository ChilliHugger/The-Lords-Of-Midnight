//
//  citadel_processor_quest.h
//  citadel
//

#ifndef _CITADEL_PROCESSOR_QUEST_H_INCLUDED_
#define _CITADEL_PROCESSOR_QUEST_H_INCLUDED_

#include "scenario_citadel_internal.h"

#include <map>
#include <optional>
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
        virtual void Survey ( void );

        mxdanger_t Danger ( u32 place ) const;
        mxdanger_t RealmDanger ( mxrace_t realm ) const;
        u32 PlaceOf ( mxgridref loc ) const;

    protected:
        struct place_t {
            citadel_stronghold*     keep = nullptr;
            std::vector<u32>        around;           // the lands with keeps around it
            std::vector<mxitem*>    figures;        // the lords and regiments standing in it
            mxdanger_t              danger = LS_AT_PEACE;
        };

        using handover_t = std::optional<mxreaction_t>;

        virtual bool Thinks ( void ) const;
        virtual void Defend ( void );
        virtual handover_t Behave ( mxreaction_t reaction );
        virtual handover_t ReturnHome ( void );
        virtual handover_t StandFirm ( void );
        virtual handover_t TakeBackStronghold ( void );
        virtual handover_t AttackEnemy ( void );
        virtual handover_t HelpNeighbour ( void );
        virtual handover_t Retreat ( void );
        virtual handover_t GatherStrength ( void );
        virtual handover_t CounterThreat ( void );
        virtual handover_t LendService ( void );
        virtual void Wander ( void );
        virtual void Campaign ( void );
        virtual void PourOut ( void );
        virtual void Quest ( void );
        virtual void Recruit ( citadel_character* character );
        virtual void Besiege ( citadel_stronghold* keep, mxgridref target );
        virtual bool March ( mxgridref target, bool fight );
        virtual bool Guard ( void );
        virtual void Done ( mxquestnews_t news );
        virtual void Blocked ( void );
        virtual void Order ( mxquest_t quest, mxid target );

        bool Invaded ( u32 at, u32 of ) const;
        u32 Home ( const citadel_character* who ) const;
        mxdanger_t HomeDanger ( void ) const;
        std::vector<u32> NearPlaces ( void ) const;
        std::vector<u32> AdjacentPlaces ( void ) const;
        std::vector<u32> RealmPlaces ( mxrace_t realm ) const;
        std::vector<u32> NeighbouringRealmPlaces ( void ) const;
        std::vector<u32> AllPlaces ( void ) const;
        std::vector<citadel_character*> OwnersOf ( const std::vector<u32>& lands ) const;
        std::vector<citadel_character*> Neighbours ( void ) const;
        std::vector<citadel_character*> Recruits ( void ) const;

        bool Idle ( void ) const;
        bool AtStronghold ( void ) const;
        bool IsEnemy ( const mxitem* figure ) const;
        mxitem* EnemyIn ( const std::vector<u32>& lands ) const;
        u32 Strength ( const mxitem* figure ) const;
        bool Odds ( const mxitem* opponent, u32 a, u32 b ) const;
        citadel_stronghold* SafeKeep ( std::vector<u32> lands ) const;
        template<typename T> T Pick ( const std::vector<T>& candidates ) const;
        template<typename T> T FirstReachable ( std::vector<T>& candidates ) const;

    protected:
        static constexpr u32 IMPATIENCE = 3;    // nights an impatient lord of yours waits before he sets out alone
        static constexpr u32 HANDOVERS  = 13;   // how often a lord's behaviour may hand over in one day
        static constexpr s32 WANDERING  = 8;    // how far a wanderer strays on one journey

        citadel_character*              lord = nullptr;
        bool                            blocked = false;    // March met the enemy in the road
        bool                            surveyed = false;
        std::vector<place_t>            places;
        std::map<mxrace_t, mxdanger_t>  realms;
    };
}

#endif // _CITADEL_
#endif // _CITADEL_PROCESSOR_QUEST_H_INCLUDED_
