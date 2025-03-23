//
//  uifocuscontroller.cpp
//  midnight
//
//  Created by Chris Wild on 23/03/2025.
//

#include "uifocuscontroller.h"
#include "uitextmenu.h"
#include "uihelper.h"
#include "../frontend/language.h"

USING_NS_AX;
USING_NS_AX_UI;

uifocuscontroller::uifocuscontroller()
    : eventListenerController(nullptr)
    , eventListenerKeyboard(nullptr)
    , parent(nullptr)
    , focusNode(nullptr)
    , focusIndex(-1)
    , focusId(ID_NONE)
    , callback(nullptr)
{
}

void uifocuscontroller::add(Node* node)
{
    if (eventListenerController != nullptr || eventListenerKeyboard != nullptr) {
        return;
    }
    
    parent = node;
    
#if defined(_OS_TVOS_)
    eventListenerController = EventListenerController::create();
    eventListenerController->onKeyUp = [this](Controller* controller, int keyCode, Event* event) {
        if (keyCode == Controller::Key::BUTTON_DPAD_LEFT) {
            findPreviousFocus();
        }
        if (keyCode == Controller::Key::BUTTON_DPAD_RIGHT) {
            findNextFocus();
        }
        if (keyCode == Controller::Key::BUTTON_X || keyCode == Controller::Key::BUTTON_A) {
            selectFocusedItem();
        }
    };
    parent->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListenerController,parent);
#endif

#if defined(_OS_DESKTOP_)
    eventListenerKeyboard = EventListenerKeyboard::create();
    eventListenerKeyboard->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
            findPreviousFocus();
            event->stopPropagation();
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
            findNextFocus();
            event->stopPropagation();
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
            selectFocusedItem();
            event->stopPropagation();
        }
    };
    parent->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListenerKeyboard,parent);
#endif

}

void uifocuscontroller::remove()
{
    if (focusNode != nullptr) {
        parent->removeChild(focusNode);
        focusNode = nullptr;
    }

    if (eventListenerKeyboard != nullptr) {
        parent->getEventDispatcher()->removeEventListener(eventListenerKeyboard);
    }
  
    if (eventListenerController != nullptr) {
        parent->getEventDispatcher()->removeEventListener(eventListenerController);
    }
  
  
    parent = nullptr;
}

void uifocuscontroller::setFocus(layoutid_t id)
{
    RETURN_IF_NULL(parent);

    auto lostFocus = focusNode;

    auto child = uihelper::getChildByTagRecursively<Node*>(id,parent);

    if (child != nullptr) {
        focusNode = child;
        focusId = id;
        focusIndex = getIndexPosition(id);
    } else {
        focusIndex = -1;
        focusId = ID_NONE;
        focusNode = nullptr;
    }

    auto gotFocus = focusNode;
    
    if (lostFocus != nullptr) {
        auto widget = dynamic_cast<Widget*>(lostFocus);
        if (widget != nullptr) {
            widget->onFocusChanged(widget, nullptr);
        } else {
            auto menuItem = dynamic_cast<MyMenuItemLabel*>(lostFocus);
            if (menuItem != nullptr) {
                menuItem->onFocusChanged(menuItem, nullptr);
            }
        }
    }
    
    if (gotFocus != nullptr) {
        auto widget = dynamic_cast<Widget*>(gotFocus);
        if (widget != nullptr) {
            widget->onFocusChanged(nullptr, widget);
        } else {
            auto menuItem = dynamic_cast<MyMenuItemLabel*>(gotFocus);
            if (menuItem != nullptr) {
                menuItem->onFocusChanged(nullptr, menuItem);
            }
        }
    }
}

bool checkIsEnabled(Node* node) {
    if (node == nullptr)
        return false;
        
    if (!node->isVisible())
        return false;
        
    auto widget = dynamic_cast<Widget*>(node);
    if (widget != nullptr) {
        return widget->isEnabled();
    }
    
    auto menuItem = dynamic_cast<MyMenuItemLabel*>(node);
    if (menuItem != nullptr) {
        return menuItem->isEnabled();
    }
    
    return false;
}

void uifocuscontroller::setControls(std::vector<layoutid_t>& controls)
{
    RETURN_IF_NULL(parent);

    this->controls = controls;
    if (controls.size() > 0) {
        setFocus(controls.at(0));
    }
}

int uifocuscontroller::getIndexPosition(layoutid_t id)
{
    return (int)distance(controls.begin(), find(controls.begin(), controls.end(), id));
}

void uifocuscontroller::findNextFocus()
{
    if (controls.size() == 0)
        return;
        
    auto newFocus = focusIndex;
        
    for(;;) {
        if (newFocus >= controls.size()-1)
            newFocus = -1;
        
        newFocus++;
        
        BREAK_IF (newFocus == focusIndex);
        
        auto id = controls.at(newFocus);
        auto child = uihelper::getChildByTagRecursively<Node*>(id,parent);
        if (checkIsEnabled(child)) {
            setFocus(id);
            break;
        }
    }
}

void uifocuscontroller::findPreviousFocus()
{
    if (controls.size() == 0)
        return;
    
    auto newFocus = focusIndex;

    for(;;) {
        if (newFocus <= 0)
            newFocus = (int)controls.size();
        
        newFocus--;
        
        BREAK_IF (newFocus == focusIndex);
        
        auto id = controls.at(newFocus);
        auto child = uihelper::getChildByTagRecursively<Node*>(id,parent);
        if (checkIsEnabled(child)) {
            setFocus(id);
            break;
        }
    }
}

void uifocuscontroller::setCallback(chilli::ui::WidgetClickCallback callback)
{
    RETURN_IF_NULL(parent);

    this->callback = callback;
}

void uifocuscontroller::selectFocusedItem()
{
    if (callback != nullptr) {
        auto child = uihelper::getChildByTagRecursively<Node*>(focusId,parent);
        if (child != nullptr) {
            callback(child);
        }
    }
}
