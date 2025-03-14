//
//  uihelpwindow.h
//  midnight
//
//  Created by Chris Wild on 11/12/2017.
//
//

#pragma once

#include "../axmol_sdk.h"
#include "../library/inc/mxtypes.h"
#include "uielement.h"
#include "../system/keyboardmanager.h"
#include "../system/helpmanager.h"
#include "../frontend/layout_id.h"
#include "../frontend/keyboard_id.h"
#include "../ui/uishortcutkeys.h"

using namespace chilli::types;

FORWARD_REFERENCE(moonring);
FORWARD_REFERENCE(uipanel);

class uicommandwindow :
    public chilli::ui::Element,
    public uishortcutkeys
{
    using Widget = ax::ui::Widget;
    using Layout = ax::ui::Layout;
    using Node = ax::Node;
    template<class T> using Vector = ax::Vector<T>;
private:
    uicommandwindow();
    ~uicommandwindow();
    
public:
    
    static uicommandwindow* create( uipanel* parent );
    
    void clearItems();
    void show( MXVoidCallback callback );
    void close();
    void updateElements();
    
protected:
    bool initWithParent( uipanel* parent );
    void OnClose();
    void initialiseCommands();
    void addItem( Widget* item, u32 index );
    void enableItem( layoutid_t id, bool enable );
    void showItem( layoutid_t id, bool show );
    Widget* findItemById( layoutid_t id );

    void addTouchListener();
    void addKeyboardListener();

#if defined(_DDR_)
    void setupGiveText();
    void setupTakeText();
    void setupUseText();
#endif
    
    std::string getCharacterName(bool enabled, mxid characterId);
    std::string getObjectName(bool enabled, mxid objectId);
    void setCommandTopText(layoutid_t id, const std::string& top);
    void setCommandBottomText(layoutid_t id, const std::string& bottom);
    void addCommandText(Node* node);
    void setCommandText(layoutid_t id, int tag, const std::string& value);
    void setupApproachText();
    
protected:
    moonring*       mr;
    uipanel*        parent;
    helpid_t        id;
    MXVoidCallback  closeCallback;
    Layout*         layout;
    size            grid;
    size            padding;
    Vector<Widget*> elements;
};

constexpr int TAG_COMMAND_TEXT_TOP = 1;
constexpr int TAG_COMMAND_TEXT_BOTTOM = 2;

