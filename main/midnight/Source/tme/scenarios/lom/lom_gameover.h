#ifndef _LOM_PROCESSOR_GAMEOVER_H_INCLUDED_
#define _LOM_PROCESSOR_GAMEOVER_H_INCLUDED_

#include "../../baseinc/mxgameover.h"

namespace tme {

    class lom_gameover : public mxgameover
    {
    public:
        lom_gameover();
        virtual ~lom_gameover();

        virtual m_gameover_t Process( bool night ) override;
    };
}

#endif //_LOM_PROCESSOR_GAMEOVER_H_INCLUDED_
