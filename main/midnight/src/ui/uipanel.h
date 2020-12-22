//
//  DefaultScene.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef uipanel_h
#define uipanel_h

#include "../cocos.h"

#include "../system/helpmanager.h"
#include "../frontend/panel_id.h"
#include "../ui/uielement.h"
#include "../ui/uishortcutkeys.h"

using namespace chilli::types;

// forward references
FORWARD_REFERENCE( uipopup );
FORWARD_REFERENCE( uihelpwindow );
FORWARD_REFERENCE( moonring );
FORWARD_REFERENCE( uieventargs );
//FORWARD_REFERENCE( uielement );

class uipanel :
    public cocos2d::Scene,
    public uishortcutkeys
{
protected:
    using Color3B = cocos2d::Color3B;
    using DrawNode = cocos2d::DrawNode;
    using Button = cocos2d::ui::Button;
    using Layer = cocos2d::Layer;
    using Node = cocos2d::Node;
    using Vec2 = cocos2d::Vec2;
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

    // Cocos2dX Scene Helpers
    virtual void onEnterTransitionDidFinish();
    virtual void onEnter();
    virtual void onExit();
    //
    
    moonring* GetMoonring() { return mr; }
    
    void Enable() { setEnabled(true); }
    void Disable() { setEnabled(false); }
    bool isEnabled() { return enabled; }
    bool isHelpVisible() { return help_visible != HELP_NONE; }
    void showHelpPending();

    
protected:
    uipanel();
    
    Node* setBackgroundToHeight( LPCSTR background, bool checkWidth = true );
    Node* setBackgroundToWidth( LPCSTR background );
    Node* setBackground( Color3B color );
    Node* setBackgroundCentered( LPCSTR background );

    
    void FadeExit( f32 delay = 0);
    void Exit( f32 delay = 0);
    
    void AreYouSure ( LPCSTR text, MXVoidCallback ok );
    void OpenPDF(LPCSTR pdf);
    void FillBackground();

    bool showHelpWindow ( helpid_t id, BOOL forceImportant = false, MXVoidCallback callback=nullptr  );
    void popupHelpWindow ( helpid_t id, MXVoidCallback callback );
    virtual void helpPending();
    
    
    void DebugNodes();
    void DebugNodesChildren(DrawNode* layer, Node* parent, Vec2 origin);
    
    void setEnabled(bool enabled);
    
    void addKeyboardListener();
    
protected:
    uipopup*        popupWindow;
    moonring*       mr;
    
    helpid_t        help_pending_id;
    bool            help_pending;
    helpid_t        help_visible;
    uihelpwindow*   help_window;
    Button*         i_help;
    
    bool            enabled;
    
    WidgetClickCallback clickCallback;
    WidgetEventCallback eventCallback;
    
    Layer*         safeArea;
    
    
public:
    panelmode_t     currentmode;
};


#endif /* uipanel_h */
