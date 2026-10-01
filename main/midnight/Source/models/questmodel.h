//
//  questmodel.h
//  citadel
//

#pragma once
#include "../tme/tme_interface.h"

#if defined(_CITADEL_)

enum class questview {
    quests,     // the twelve quests, for the lord you are looking through
    targets,    // whom or what the chosen one is for
    news,       // the lords of yours with something to tell you this morning
};

struct questmodel
{
    questview   view = questview::quests;
    mxquest_t   quest = QS_NONE;        // the quest waiting for its target
    bool        picking = false;        // the detailed map is choosing a place for it
};

#endif
