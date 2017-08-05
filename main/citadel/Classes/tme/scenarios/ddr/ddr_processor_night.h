#ifndef _DDR_NIGHTPROCESSOR_H_INCLUDED_
#define _DDR_NIGHTPROCESSOR_H_INCLUDED_

#include "../../baseinc/processor_night.h"


namespace tme {
	//namespace processors {

    class ddr_night : public mxnight
		{
        public:
            enum WINFLAGS {
                wf_luxor_home   = MXBIT(4),
                wf_morkin_home  = MXBIT(3),
                wf_tarithel_home= MXBIT(2),
                wf_rorthron_home= MXBIT(1),
                wf_shareth_dead = MXBIT(0),
                wf_all=wf_luxor_home|wf_morkin_home|wf_tarithel_home|wf_rorthron_home|wf_shareth_dead,
            };
            
            enum WINMODE {
                WIN_NONE=0,
                WIN_WORTHY=1,
                WIN_NOBLE=2,
                WIN_OVERWHELMING=3,
            };

#define WIN_MAX_TARGETS 6
            
            
		public:
			ddr_night();
			virtual ~ddr_night();

			virtual void Process(void);
			virtual m_gameover_t CheckWinLoseConditions ( BOOL night );
            virtual void MoveMidwinter ( void );
            virtual void checkImportantCharactersDead(void);
            
            virtual void describeVictory ( void );

            
#ifdef _TEST_WINLOSE_CONDITIONS_
            void testWinLoseConditions ( void );
#endif
            
            s32         victoryTargets;
            flags8      victoryFlags;
            WINMODE     victoryMode;

		};
	//}
}

#endif //_DDR_NIGHTPROCESSOR_H_INCLUDED_
