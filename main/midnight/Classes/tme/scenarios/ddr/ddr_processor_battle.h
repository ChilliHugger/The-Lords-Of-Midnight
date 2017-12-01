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
		
            virtual void War ( void );
            virtual void Battle ( mxcharacter* character);
            virtual void BattleVsCharacter ( mxcharacter* attacker );
            virtual void doBattle ( mxitem* attacker, s32 success);
            
            virtual void battleArmies ( mxitem* attacker, mxitem* defender );
            
            virtual void loseFight ( ddr_character* c, s32 success );

            s32 getArmySize( mxitem* item);
            void setArmySize(mxitem* item, s32 size);
            
            s32 getArmyStrength( mxitem* army );

            void Test(void);
            
        protected:
            entities  characters_here;
        
        };


    
    
	//}
}

#endif // _DDR_

#endif //_DDR_BATTLEPROCESSOR_H_INCLUDED_
