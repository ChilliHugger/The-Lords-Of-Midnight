#ifndef _GAMEOVER_H_INCLUDED_
#define _GAMEOVER_H_INCLUDED_

#include "lomxtypes.h"

namespace tme {

    class mxgameover
    {
    public:
        mxgameover();
        virtual ~mxgameover();

        virtual m_gameover_t Process ( bool night );
    };
}

#endif //_GAMEOVER_H_INCLUDED_
