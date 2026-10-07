#ifndef _CITADEL_BATTLEPROCESSOR_H_INCLUDED_
#define _CITADEL_BATTLEPROCESSOR_H_INCLUDED_

#include "../lom/lom_processor_battle.h"
#include "scenario_citadel_internal.h"

#include <memory>
#include <vector>

#if defined(_CITADEL_)
namespace tme {

    //
    // The Citadel fights Lords of Midnight's battle, but not everyone the night finds is at war.
    //
    class citadel_battle : public lom_battle
    {
    public:
        virtual void Duel ( mxcharacter* attacker, mxcharacter* defender );
        virtual void Attack ( mxcharacter* lord );
        virtual bool Guard ( mxcharacter* lord );

    protected:
        // one lord and his men, in a duel
        struct side_t {
            mxcharacter*                            lord;
            std::vector<std::unique_ptr<mxarmy>>    armies;
            c_army                                  standing;
        };

        virtual bool HasDefenders() const override;
        virtual bool TakesPart ( const mxarmy* army ) const override;
        virtual bool TakesPart ( const mxcharacter* character ) const override;

        virtual void PrepareArmies() override;
        virtual void UpdateStrongholdArmy ( mxarmy* army ) override;
        virtual mxcharacter* Liberator() const override;

        virtual void CharacterLosesEnergy ( mxcharacter* character ) override;

        virtual side_t Muster ( mxcharacter* lord );
        virtual void Enlist ( side_t& side, u32 total, mxunit_t type, s32 success );
        virtual void Strike ( side_t& from, side_t& at );
    };

    inline citadel_battle* CitadelBattle ()
    {
        return static_cast<citadel_battle*>(mx->battle);
    }

}
#endif // _CITADEL_

#endif //_CITADEL_BATTLEPROCESSOR_H_INCLUDED_
