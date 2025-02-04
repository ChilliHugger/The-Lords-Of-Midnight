#ifndef _LOM_BATTLEPROCESSOR_H_INCLUDED_
#define _LOM_BATTLEPROCESSOR_H_INCLUDED_

#include "../../baseinc/info.h"
#include "../../baseinc/processor_battle.h"
#include "scenario_lom_internal.h"

#if defined(_LOM_)
namespace tme {

    class lom_battle : public mxbattle
    {
    public:
        virtual void ProcessLocation ( mxgridref loc );

    protected:
        virtual u32 Fight ( u32 attacks, u32 success, c_army& foes );

        virtual void ProcessAllCharacters();
        virtual void ProcessAllArmies();

        virtual bool MakeFriendOrFoeList ( const mxcharacter* character=nullptr );

        virtual bool SelectOpponent(c_army& armies);

        virtual void CheckVictors ();
        virtual void CalculateBattleStatus();
        virtual void UpdateArmies();
        virtual void UpdateCharacters();

        virtual void UpdateStrongholdArmy(mxarmy* army);
        virtual void UpdateRegimentArmy(mxarmy* army);
        virtual void UpdateCharacterArmy(mxarmy* army);

        virtual void CharacterLosesEnergy( mxcharacter* character );
        virtual void CharacterLosesBattle( mxcharacter* character );
        virtual void CharacterWinsBattle( mxcharacter* character );
        virtual void CharacterContinuesBattle( mxcharacter* character );

        virtual bool ChanceOfEngagement(u32 success);
        virtual bool ChanceOfSuccessfullKill(u32 success);
        
    protected:
        std::unique_ptr<mxlocinfo> info;
        
        mxgridref           location;
        c_army              foes;
        c_army              friends;
        mxbattlestatus_t    status;
    };

}
#endif // _LOM_

#endif //_LOM_BATTLEPROCESSOR_H_INCLUDED_
