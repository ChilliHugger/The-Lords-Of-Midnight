//
//  uichoicepopup.h
//  midnight
//
//  A window offering a short list of actions, for the things that are done by dragging
//  with a mouse or a finger. Choosing one closes the window and runs it. There is always
//  a cancel button, which Escape also presses.
//

#pragma once

#include "../axmol_sdk.h"
#include "../library/inc/mxtypes.h"
#include "uielement.h"
#include "uishortcutkeys.h"

FORWARD_REFERENCE(uipanel);

using namespace chilli::types;

struct uichoice
{
    std::string             text;
    std::function<void()>   action;
};

class uichoicepopup :
    public chilli::ui::Element,
    public uishortcutkeys
{
protected:
    using Layout = ax::ui::Layout;

private:
    uichoicepopup() = default;
    ~uichoicepopup();

public:
    static uichoicepopup* create( uipanel* parent, const std::string& title, const std::vector<uichoice>& choices );

    void Show();
    void Close();

protected:
    bool initWithParent( uipanel* parent, const std::string& title, const std::vector<uichoice>& choices );
    void addTouchListener();
    void choose( size_t index );

protected:
    uipanel*                parent{};
    Layout*                 layout{};
    std::vector<uichoice>   choices;
};
