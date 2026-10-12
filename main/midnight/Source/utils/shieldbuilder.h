//
//  shieldbuilder.h
//  midnight
//
//  Created by Chris Wild on 15/03/2025.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#ifndef shieldbuilder_h
#define shieldbuilder_h

#include "../axmol_sdk.h"
#include "../library/inc/mxtypes.h"
#include "../tme/tme_interface.h"
#include "../ui/uihelper.h"

USING_NS_TME;

// Builds a lord's shield from its description in shields.xml (registered in moonring's imageDefinitions)
class shieldbuilder
{
public:
    using ImageView = ax::ui::ImageView;
public:
    shieldbuilder();
    virtual ~shieldbuilder();
    
    bool build(mxid id, ImageView* parent);
};

#endif /* shieldbuilder_h */
