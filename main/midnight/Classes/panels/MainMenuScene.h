#ifndef __MAINMENU_SCENE_H__
#define __MAINMENU_SCENE_H__

#include "DefaultScene.h"
#include "../frontend/layout_id.h"

class MainMenu : public DefaultScene
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    CREATE_FUNC(MainMenu);

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

#endif // __MAINMENU_SCENE_H__
