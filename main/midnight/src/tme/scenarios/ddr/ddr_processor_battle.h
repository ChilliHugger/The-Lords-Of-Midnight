#ifndef _DDR_BATTLEPROCESSOR_H_INCLUDED_
#define _DDR_BATTLEPROCESSOR_H_INCLUDED_

#include "../../baseinc/info.h"
#include "../../baseinc/processor_battle.h"
#include "scenario_ddr_internal.h"

#if defined(_DDR_)

namespace tme {
    //namespace processors {

    class ddr_battle : public mxbattle
        {
        public:
            ddr_battle();
            virtual ~ddr_battle();
        
            virtual void Process ( void );
            virtual void Battle ( ddr_character* character);
            virtual void CharacterAttacks ( ddr_character* attacker );
            virtual void StrongholdAttacks( mxstronghold* attacker );
            virtual void doBattle ( mxitem* attacker, s32 attacker_hp);
            virtual void battleArmies ( mxitem* attacker, mxitem* defender );
            virtual void CharacterVsCharacter( ddr_character* attacker, s32 attacker_hp, ddr_character* defender);

            virtual ddr_character* SelectDefender(c_character& characters, mxrace_t loyalty);
            virtual s32 GetCharactersHereForBattle(c_character& characters, mxcharacter* character);

            virtual void PutCharacterInBattle( ddr_character* character );

            virtual void DefenderLoses(ddr_character* defender, s32 defender_hp, ddr_character* attacker );
            
            virtual s32 CalcCharacterAttackerHP(const ddr_character* attacker);
            virtual s32 CalcCharacterDefenderHP( const ddr_character* defender );
            virtual s32 CalcArmyAttackerHP( mxitem* attacker );
            virtual s32 CalcArmyDefenderHP( mxitem* defender );

            virtual s32 CalcDefenderLoses( mxitem *attacker, mxitem* defender );
            virtual void UpdateDefenderLoses(mxitem* defender, s32 loses);
            virtual void UpdateAttackerKills(mxitem* attacker, s32 kills);

            virtual void CharacterLosesArmy( ddr_character* character );
            virtual void StrongholdLosesArmy( mxstronghold* stronghold, mxcharacter* attacker );
            virtual void AttackerArmiesWinBattle(tme::mxitem *attacker);
            virtual void DefenderArmiesLosesBattle( mxitem *attacker, mxitem* defender );

            virtual void loseFight ( ddr_character* character, s32 success );
            virtual void CharacterKilledByCharacter(ddr_character* defender, ddr_character* attacker );

            virtual bool CharacterShouldLoseHorse(const ddr_character* character, s32 success);
            virtual bool CharacterShouldDie(const ddr_character* character);
            virtual bool CharacterIsProtected(const ddr_character* character);
            virtual bool CharacterHasBattleObject( const ddr_character* character );

            s32 getArmySize( mxitem* item);
            void setArmySize(mxitem* item, s32 size);
            
            s32 getArmyStrength( mxitem* army );
            
        protected:
            c_character  characters_here;
        
        };


    
    
    //}
}

#endif // _DDR_

#endif //_DDR_BATTLEPROCESSOR_H_INCLUDED_
