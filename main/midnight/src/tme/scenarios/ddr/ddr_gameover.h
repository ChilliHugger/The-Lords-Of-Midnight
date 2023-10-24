#ifndef _DDR_GAMEOVER_H_INCLUDED_
#define _DDR_GAMEOVER_H_INCLUDED_

#include "../../baseinc/mxgameover.h"

namespace tme {
    constexpr int WIN_MAX_TARGETS = 6;
 
    class ddr_gameover : public mxgameover
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

    public:
        ddr_gameover();
        virtual ~ddr_gameover();

        virtual m_gameover_t Process( bool night ) override;
        virtual void checkImportantCharactersDead(void);
        
    private:
        virtual void describeVictory ( WINMODE mode, flags8 flags, s32 targets );
    };
}

#endif //_DDR_GAMEOVER_H_INCLUDED_
