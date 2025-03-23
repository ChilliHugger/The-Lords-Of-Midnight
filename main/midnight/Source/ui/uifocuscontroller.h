//
//  uifocuscontroller.h
//  midnight
//
//  Created by Chris Wild on 23/03/2025.
//

#ifndef uifocuscontroller_h
#define uifocuscontroller_h

#include "../axmol_sdk.h"
#include "../library/inc/mxtypes.h"
#include "uielement.h"
#include "../frontend/layout_id.h"

using namespace chilli::types;

class uifocuscontroller {
protected:
    using Node = ax::Node;
    using EventListenerController = ax::EventListenerController;
    using EventListenerKeyboard = ax::EventListenerKeyboard;
    using WidgetClickCallback = chilli::ui::WidgetClickCallback;
private:
    int getIndexPosition(layoutid_t id);
    void findPreviousFocus();
    void findNextFocus();
    void selectFocusedItem();

private:
    EventListenerController*    eventListenerController;
    EventListenerKeyboard*      eventListenerKeyboard;
    Node*                       focusNode;
    Node*                       parent;
    std::vector<layoutid_t>     controls;
    int                         focusIndex;
    layoutid_t                  focusId;
    WidgetClickCallback         callback;

public:
    uifocuscontroller();

    void add(Node* node);
    void remove();
    void setFocus(layoutid_t id);
    void setControls(std::vector<layoutid_t>& controls);
    void setCallback(WidgetClickCallback callback);
};

#endif /* uifocuscontroller_h */
