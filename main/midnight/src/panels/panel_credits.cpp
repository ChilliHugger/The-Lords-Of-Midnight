//
//  CreditsScene.cpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#include "../cocos.h"
#include "../extensions/CustomDirector.h"
#include "../platform/Extensions.h"

#include "panel_credits.h"
#include "panel_mainmenu.h"

#include "../system/resolutionmanager.h"
#include "../system/panelmanager.h"
#include "../ui/uihelper.h"

USING_NS_CC;

bool panel_credits::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    setBackgroundToHeight("credits.png");

    std::string version = "Version: "
        + Application::getInstance()->getVersion()
        + " (" + chilli::extensions::getBuildNo() + ")";


    auto label = Label::createWithTTF(uihelper::font_config_debug, version);
    label->getFontAtlas()->setAntiAliasTexParameters();
    label->setTextColor(Color4B::BLACK);
    label->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(safeArea, label, RES(32), RES(32));

    Exit(5.0);
    
    return true;
}


