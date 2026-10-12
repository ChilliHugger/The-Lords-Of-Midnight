//
//  uigroupedlord.cpp
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//
#include "../uihelper.h"
#include "uigroupedlord.h"
#include "uisinglelord.h"
#include "../../tme/tme_interface.h"
#include "../../system/tmemanager.h"
#include "../../system/moonring.h"
#include "../../system/resolutionmanager.h"

USING_NS_AX;

uigroupedlord::uigroupedlord() :
    buttonNode(nullptr)
{
    lordtype = lordwidget::grouped;
}

bool uigroupedlord::init()
{
    if ( uilordselect::init() ) {
        
        setContentSize(Size(DISTANCE_INNER,DISTANCE_INNER));
        
        setTouchEnabled(true);
        enableDrag();
        enableDrop();
        
        return true;
    }
    return false;
}

uigroupedlord* uigroupedlord::createWithLord ( mxid characterid )
{
    auto select = uigroupedlord::create();
    select->setLord( characterid );
    return select;
}

bool uigroupedlord::setLord(mxid characterid)
{
    uilordselect::setLord(characterid);
    
    character c;
    TME_GetCharacter(c, characterid );
    
    renderLordImage("lord_grouped", faceName(c), {}, SHIELD_OFFSET_Y);
    setLordTitle(c.shortname, false);
    buttonNode = lordImage;
    
    auto userdata = static_cast<char_data_t*>(getUserData());
    userdata->select_loc = point::ZERO;
    userdata->select_page = InvalidPage;
    
    refreshStatus();
    
    return true;
}

void uigroupedlord::setTitleAlignment(TextHAlignment align)
{
}

void uigroupedlord::onPressStateChangedToNormal()
{
    if ( buttonNode ) {
        buttonNode->setScale(scale_normal);
        buttonNode->setOpacity(ALPHA(alpha_normal));
    }
}

void uigroupedlord::onPressStateChangedToPressed()
{
    if ( buttonNode ) {
        buttonNode->setScale(scale_3qtr);
        buttonNode->setOpacity(ALPHA(alpha_normal));
    }
}

bool uigroupedlord::hitTest(const Vec2 &pt, const Camera* camera, Vec3 *p) const
{
    return ax::ui::Widget::hitTest(pt, camera, p)
        && MouseOverHotspot(pt, MOUSE_OVER_HINT_DROP) == MOUSE_OVER_FACE ;
}
