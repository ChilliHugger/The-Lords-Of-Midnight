//
//  uifocus.h
//  midnight
//
//  D-pad / arrow key focus for the UI. One uifocuscontroller belongs to each panel.
//
//  What can take focus is decided by marking the node (uifocusable::mark) - the button
//  factories in uihelper and the menu classes do this already, so most screens need
//  nothing more than a default focus. A marked node is only a candidate while it is
//  running, visible, enabled and on screen, so hiding or disabling an icon takes it out
//  of the navigation without any bookkeeping.
//
//  Where a press goes is the nearest candidate in that direction (uifocusnav.h). A screen
//  whose layout does not suit that can set links by layout id - either on the node or as a
//  table for the panel - and a link to something unavailable is followed on to what that
//  item links to. A panel may also override a move completely (uipanel::OnFocusMove).
//
//  Modal windows push a scope (their root node) so that only their own controls can be
//  reached, and pop it again when they close.
//

#pragma once

#include "../axmol_sdk.h"
#include "../library/inc/mxtypes.h"
#include "../frontend/layout_id.h"
#include "uifocusnav.h"

FORWARD_REFERENCE(uipanel);

namespace chilli {
    namespace ui {

    using FocusDirection = focus::direction;

    //
    // The colour of the focus follows the look of each game's UI. Text drawn on top of
    // it has to stay readable, so the two are chosen together.
    //
#if defined(_LOM_)
    const ax::Color3B FOCUS_COLOUR(0xff, 0xdd, 0x00);
    const ax::Color3B FOCUS_TEXT_COLOUR(0x00, 0x00, 0x00);
#elif defined(_DDR_)
    const ax::Color3B FOCUS_COLOUR(0x00, 0xdd, 0xdd);
    const ax::Color3B FOCUS_TEXT_COLOUR(0x00, 0x00, 0x00);
#else
    const ax::Color3B FOCUS_COLOUR(0xdd, 0x44, 0xdd);
    const ax::Color3B FOCUS_TEXT_COLOUR(0x00, 0x00, 0x00);
#endif

    // Which way a node shows it has focus
    enum class FocusStyle {
        outline,        // coloured rectangle around the node, for icons and buttons
        background,     // coloured block behind the node, for menu items
        tint,           // nothing is drawn: the item colours its own box through onFocus
    };

    // Explicit moves for a panel's table. A link of ID_NONE leaves that direction to the
    // nearest-candidate search.
    struct FocusLink {
        layoutid_t id;
        layoutid_t left;
        layoutid_t right;
        layoutid_t up;
        layoutid_t down;
    };

    //
    // Marks a node as able to take focus, carrying its per-node settings
    //
    class FocusItem : public ax::Component
    {
    public:
        static constexpr const char* NAME = "focusitem";

        static FocusItem* mark( ax::Node* node, FocusStyle style = FocusStyle::outline );
        static FocusItem* of( ax::Node* node );

        FocusItem* setLink( FocusDirection direction, layoutid_t id );

        layoutid_t link( FocusDirection direction ) const { return links[(int)direction]; }

    public:
        FocusStyle                  style{ FocusStyle::outline };

        // used instead of a click when the node is not a Widget or MenuItem
        std::function<void()>       onActivate;

        // for items that change their own look to suit the focus, e.g. the text of a menu
        // item turns dark on the coloured background
        std::function<void(bool)>   onFocus;

    protected:
        FocusItem() { _name = NAME; }

    private:
        layoutid_t                  links[4]{};
    };

    }
}

class uifocuscontroller
{
protected:
    using Node = ax::Node;
    using Widget = ax::ui::Widget;
    using Direction = chilli::ui::FocusDirection;
    using FocusLink = chilli::ui::FocusLink;
    using FocusStyle = chilli::ui::FocusStyle;
    using FocusItem = chilli::ui::FocusItem;

public:
    // Follows a move for the panel: return the node to focus, or nullptr to leave it to
    // the nearest-candidate search.
    using MoveCallback = std::function<Node*(Node* from, Direction direction)>;

    // Called when there was nothing to move to; return true if it was dealt with
    // (scrolling a page, turning a page...)
    using EdgeCallback = std::function<bool(Direction direction)>;

public:
    explicit uifocuscontroller( uipanel* owner ) : panel(owner) {}
    ~uifocuscontroller();

    // Starts listening for keys and controllers, and showing the focus. The panel does
    // this once it has finished arriving and stops as it starts to leave, so a screen
    // that is changing never handles input.
    void attach();
    void detach();

    bool isEnabled() const;

    Node* current() const { return focused; }
    bool focus( Node* node );
    bool focus( layoutid_t id );

    // whether node is the ancestor or one of its descendants
    bool isWithin( Node* node, Node* ancestor ) const;

    // The panel's first focus: the default if it is available, otherwise the item first
    // in reading order. Does nothing if something is already focused.
    void focusDefault();
    void setDefault( layoutid_t id ) { defaultId = id; }

    void setLinks( const FocusLink* links, size_t count );

    bool navigate( Direction direction );
    bool activate();

    // Re-checks that the focus is still on something that can have it, e.g. after a
    // button was hidden.
    void refresh();

    void pushScope( Node* root );
    void popScope( Node* root );

    MoveCallback    onMove;
    EdgeCallback    onEdge;

    // focus was given to or taken from a node
    std::function<void(Node* lost, Node* got)> onChanged;

protected:
    ax::Rect worldRect( Node* node ) const;
    chilli::ui::focus::rect focusRect( Node* node ) const;
    void setFocusNode( Node* node );
    bool isCandidate( Node* node ) const;
    bool isInScope( Node* node ) const;
    Node* scopeRoot() const;

    void collect( Node* node, std::vector<Node*>& result ) const;
    std::vector<Node*> candidates() const;

    Node* findLinked( Node* from, Direction direction ) const;
    Node* findNearest( Node* from, Direction direction ) const;
    layoutid_t linkFor( Node* node, Direction direction ) const;

    void showFocus();
    ax::Sprite* findPicture( Node* node ) const;
    void hideFocus();
    void updateFocus();
    void ensureVisible( Node* node );
    bool scroll( Direction direction );
    void press( Node* node );

    bool onKeyDown( ax::EventKeyboard::KeyCode key );
    bool onKeyUp( ax::EventKeyboard::KeyCode key );
    bool onControllerDown( int key );
    bool onControllerUp( int key );
    void goBack();
    bool isActivateKey( ax::EventKeyboard::KeyCode key ) const;
    bool getDirection( ax::EventKeyboard::KeyCode key, Direction& direction ) const;
    bool getDirection( int controllerKey, Direction& direction ) const;

protected:
    uipanel*                    panel{};
    Node*                       focused{};
    ax::Node*                   highlight{};
    ax::SpriteFrame*            highlightFrame{};       // the picture the outline was made from
    ax::Size                    highlightSize;
    FocusStyle                  highlightStyle{ FocusStyle::outline };
    layoutid_t                  defaultId{ ID_NONE };
    layoutid_t                  lastId{ ID_NONE };      // focus when the panel was left, for when it returns
    const FocusLink*            links{};
    size_t                      linkCount{};
    std::vector<Node*>          scopes;

    ax::EventListenerKeyboard*  keyboardListener{};
    ax::EventListenerController* controllerListener{};

    // an activation only counts if the press was seen too, otherwise releasing the key
    // that changed screen would activate something on the new screen
    bool                        activateArmed{};
};
