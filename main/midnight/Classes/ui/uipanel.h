//
//  DefaultScene.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef uipanel_h
#define uipanel_h

#include "cocos2d.h"
#include "../system/helpmanager.h"

using namespace chilli::types;

// forward references
FORWARD_REFERENCE( uipopup );
FORWARD_REFERENCE( uihelpwindow );
FORWARD_REFERENCE( moonring );
FORWARD_REFERENCE( uieventargs );
FORWARD_REFERENCE( uielement );

class uipanel : public cocos2d::Scene
{
public:
    
    virtual bool init();
    virtual void OnNotification( Ref* element );

    moonring* GetMoonring() { return mr; }
    
protected:
    
    Node* SetBackground( LPCSTR background );
    Node* SetBackground( cocos2d::Color3B color );
    
    void FadeExit( f32 delay = 0);
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
    BOOL            help_pending;
    int             help_visible;
    uihelpwindow*   help_window;
};


#endif /* uipanel_h */
