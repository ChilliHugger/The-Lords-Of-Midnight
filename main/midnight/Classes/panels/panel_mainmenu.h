#ifndef __panel_mainmenu_h__
#define __panel_mainmenu_h__

#include "../ui/uipanel.h"
#include "../frontend/layout_id.h"

FORWARD_REFERENCE(menueventargs);
FORWARD_REFERENCE(uinotificationinterface);
FORWARD_REFERENCE(uitextmenu);

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

    void deleteStory( storyid_t id );
    void refreshStories( void );
    
private:
    uitextmenu* menu;
};

#endif // __panel_mainmenu_h__
