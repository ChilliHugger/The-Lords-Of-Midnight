/*
 * FILE:    mxgameover.cpp
 * 
 * PROJECT: MidnightEngine
 * 
 * CREATED: 
 * 
 * AUTHOR:  Chris Wild
 * 
 * Copyright 2023 Chilli Hugger. All rights reserved.
 * 
 * PURPOSE: 
 * 
 * 
 */

#include "../baseinc/tme_internal.h"
#include "../baseinc/mxgameover.h"

namespace tme {

    mxgameover::mxgameover()
    {
    }

    mxgameover::~mxgameover()
    {
    }
    
    m_gameover_t mxgameover::Process( bool night )
    {
        return m_gameover_t::MG_NONE;
    }
    
}
// namespace tme
