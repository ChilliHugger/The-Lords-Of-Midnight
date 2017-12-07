#ifndef __panel_mainmenu_h__
#define __panel_mainmenu_h__

#include "../ui/uipanel.h"
#include "../frontend/layout_id.h"

class panel_mainmenu : public uipanel
{
public:
    
    virtual bool init();
    
    CREATE_FUNC(panel_mainmenu);
    
    virtual void OnNotification( Ref* element, u32 tag );

protected:
    void createMenu();
    
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
