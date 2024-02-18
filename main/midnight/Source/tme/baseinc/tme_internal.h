/*
 *  tme_internal.h
 *  tme
 *
 *  Created by Chris Wild on 02/02/2011.
 *  Copyright 2011 Chilli Hugger. All rights reserved.
 *
 */

#ifndef _TMEINTERNAL_H_INCLUDED_
#define _TMEINTERNAL_H_INCLUDED_



#include "info.h"
#include "mxengine.h"
#include "mxinterface.h"
#include "midnight.h"
#include "lomxtypes.h"
#include "../scenarios/default/default_scenario.h"
#include "../scenarios/default/default_scenario_internal.h"
#include "../baseinc/mxgameover.h"

namespace tme {
    MXINLINE archive& operator<<( archive& ar, std::string& s )  { return StringExtensions::SerializeString(ar, s); }
    MXINLINE archive& operator>>( archive& ar, std::string& s )  { return StringExtensions::SerializeString(ar, s); }
}

#endif // _TMEINTERNAL_H_INCLUDED_
