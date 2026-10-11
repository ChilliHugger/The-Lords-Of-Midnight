//
//  shieldbuilder.cpp
//  midnight
//
//  Created by Chris Wild on 09/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#include "../system/moonring.h"

#include "shieldbuilder.h"
#include "imagebuilder.h"
#include "../tme/tme_interface.h"
#include "../system/tmemanager.h"
#include "../ui/uihelper.h"

USING_NS_AX;
USING_NS_AX_UI;

shieldbuilder::shieldbuilder() 
{
}

shieldbuilder::~shieldbuilder()
{
}

bool shieldbuilder::build(mxid id, ImageView* parent)
{
    auto data = static_cast<char_data_t*>(TME_GetEntityUserData(id));
    if (data == nullptr) {
        return false;
    }

    return imagebuilder::build(StringExtensions::toLower(data->symbol), parent);
}
