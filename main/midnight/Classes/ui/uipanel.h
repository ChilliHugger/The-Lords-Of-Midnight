//
//  DefaultScene.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef uipanel_h
#define uipanel_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../system/helpmanager.h"
#include "../frontend/panel_id.h"
#include "../ui/uielement.h"

using namespace chilli::types;

// forward references
FORWARD_REFERENCE( uipopup );
FORWARD_REFERENCE( uihelpwindow );
FORWARD_REFERENCE( moonring );
FORWARD_REFERENCE( uieventargs );
//FORWARD_REFERENCE( uielement );

class uipanel : public cocos2d::Scene
{
public:
    
    virtual bool init();
    virtual void OnNotification( Ref* element );
    virtual void setObject( mxid object ) {};
    virtual void OnShown( void );
    virtual void OnActivate( void );
    virtual void OnDeActivate( void );

    // Cocos2dX Scene Helpers
    virtual void onEnterTransitionDidFinish();
    virtual void onEnter();
    virtual void onExit();
    //
    
    moonring* GetMoonring() { return mr; }
    
    void Enable() { enabled = true; }
    void Disable() { enabled = false; }
    bool isEnabled() { return enabled; }
    bool isHelpVisible() { return help_visible != HELP_NONE; }
    
protected:
    
    Node* setBackgroundToHeight( LPCSTR background );
    Node* setBackgroundToWidth( LPCSTR background );
    Node* setBackground( cocos2d::Color3B color );
    
    void FadeExit( f32 delay = 0);
    void Exit( f32 delay = 0);
    
    void AreYouSure ( LPCSTR text, MXVoidCallback ok );
    void OpenPDF(LPCSTR pdf);
    void FillBackground();

    BOOL ShowHelpWindow ( helpid_t id, BOOL forceImportant = false, MXVoidCallback callback=nullptr  );
    void PopupHelpWindow ( helpid_t id, MXVoidCallback callback );
    virtual void HelpPending();
    
    
    void DebugNodes();
    void DebugNodesChildren(cocos2d::DrawNode* layer, Node* parent, cocos2d::Vec2 origin);
    
protected:
    uipopup*        popupWindow;
    moonring*       mr;
    
    helpid_t        help_pending_id;
    bool            help_pending;
    helpid_t        help_visible;
    uihelpwindow*   help_window;
    
    bool            enabled;
    
    WidgetClickCallback clickCallback;
    WidgetEventCallback eventCallback;
    
public:
    panelmode_t     currentmode;
};


#endif /* uipanel_h */
