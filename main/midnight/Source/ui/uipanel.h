//
//  DefaultScene.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef uipanel_h
#define uipanel_h

#include "../axmol_sdk.h"

#include "../system/helpmanager.h"
#include "../system/resolutionmanager.h"
#include "../frontend/panel_id.h"
#include "../ui/uielement.h"
#include "../ui/uishortcutkeys.h"
#include "../system/moonring.h"

using namespace chilli::types;

// forward references
FORWARD_REFERENCE( uipopup );
FORWARD_REFERENCE( uihelpwindow );
FORWARD_REFERENCE( moonring );
FORWARD_REFERENCE( uieventargs );



class ResumeEventListenerMouse : public ax::EventListenerMouse
{
public:
    static ResumeEventListenerMouse* create()
    {
        auto ret = new (std::nothrow) ResumeEventListenerMouse();
        if (ret && ret->init())
        {
            ret->autorelease();
        }
        else
        {
            AX_SAFE_DELETE(ret);
        }
        return ret;
    }
    
    void Resume() {
        setPaused(false);
    }
};


class uipanel :
    public ax::Scene,
    public uishortcutkeys
{
protected:
    using Color3B = ax::Color3B;
    using DrawNode = ax::DrawNode;
    using Button = ax::ui::Button;
    using Layer = ax::Layer;
    using Node = ax::Node;
    using Vec2 = ax::Vec2;
    using Sprite = ax::Sprite;
    using WidgetClickCallback = chilli::ui::WidgetClickCallback;
    using WidgetEventCallback = chilli::ui::WidgetEventCallback;
public:
    
    virtual bool init();
    virtual void setObject( mxid object ) {};
    
    virtual void OnNotification( Ref* element );
    virtual void OnShown( void );
    virtual void OnActivate( void );
    virtual void OnDeActivate( void );
    virtual bool OnKeyboardEvent( uikeyboardevent* event );
  
#if defined(_MOUSE_ENABLED_)
    virtual bool OnMouseMove( Vec2 pos );
#endif

    // Axmol Scene Helpers
    virtual void onEnter();
    virtual void onExit();
    //
    
    moonring* GetMoonring() { return mr; }
    f32 phoneScale() const;
    bool isPhoneScaleEnabled() const;
    padding getSafeArea() const;
    
    void Enable() { setEnabled(true); }
    void Disable() { setEnabled(false); }
    bool isEnabled() { return enabled; }
    bool isHelpVisible() { return help_visible != HELP_NONE; }
    void showHelpPending();

    void pauseEvents();
    void resumeEvents();    
    void AreYouSure ( LPCSTR text, MXVoidCallback ok, MXVoidCallback notok = nullptr );
    
protected:
    uipanel();
    
    Node* setBackgroundToHeight( LPCSTR background, bool checkWidth = true );
    Node* setBackgroundToWidth( LPCSTR background );
    Node* setBackground( Color3B color );
    Node* setBackgroundCentered( LPCSTR background );

    void FadeExit( f32 delay = 0);
    void Exit( f32 delay = 0);
    
    void OpenPDF(LPCSTR pdf);
    void FillBackground();

    bool showHelpWindow ( helpid_t id, BOOL forceImportant = false, MXVoidCallback callback=nullptr  );
    void popupHelpWindow ( helpid_t id, MXVoidCallback callback );
    virtual void helpPending();
    
    
    void DebugNodes();
    void DebugNodesChildren(DrawNode* layer, Node* parent, Vec2 origin);
    
    void setEnabled(bool enabled);
    
    void addKeyboardListener();
    
#if defined(_MOUSE_ENABLED_)
    void addMouseListener();
    void setCursor(MOUSE_CURSOR cursor);
    Node* childFromPoint( Vec2 pos );
    Node* childFromPoint( Node* node, Vec2 pos );
    void ResumeMouseListener();
#endif
    
protected:
    uipopup*            popupWindow;
    moonring*           mr;
    
    helpid_t            help_pending_id;
    bool                help_pending;
    helpid_t            help_visible;
    uihelpwindow*       help_window;
    Button*             i_help;
    
    bool                enabled;
    
    WidgetClickCallback clickCallback;
    WidgetEventCallback eventCallback;
    
    Layer*              safeArea;
    
#if defined(_MOUSE_ENABLED_)
    Sprite*             imgCursor;
    ResumeEventListenerMouse* mouseEventListener;
    
public:
    static Vec2     cursorPosition;
    MOUSE_CURSOR    currentCursor;
    Vec2            cursorAnchor;
#endif

public:
    panelmode_t     currentmode;
};

#endif /* uipanel_h */
