#ifndef __panel_mainmenu_h__
#define __panel_mainmenu_h__

#include "../ui/uipanel.h"
#include "../frontend/layout_id.h"

FORWARD_REFERENCE(menueventargs);
FORWARD_REFERENCE(uinotificationinterface);

class panel_mainmenu : public uipanel
{
public:
    
    virtual bool init();
    
    CREATE_FUNC(panel_mainmenu);
    
    void OnMenuNotification( uinotificationinterface*, menueventargs* );
    virtual void OnNotification( Ref* element );
    
protected:
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
