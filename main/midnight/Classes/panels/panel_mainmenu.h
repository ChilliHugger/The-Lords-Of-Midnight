#ifndef __panel_mainmenu_h__
#define __panel_mainmenu_h__

#include "uipanel.h"
#include "../frontend/layout_id.h"

class panel_mainmenu : public uipanel
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    CREATE_FUNC(panel_mainmenu);

protected:
    void createMenu();
    
    void OnMenuItem( Ref* element, ICON_ID id );
    void OnShowManual();
    void OnShowGuide();
    void OnHelpClose();
    void OnOptions();
    void OnCredits();
    void OnExit();
    void OnUpdate();
    void OnNewStory();
    void OnContinueStory();
    void OnEndStory();
};

#endif // __panel_mainmenu_h__
