#ifndef __panel_mainmenu_h__
#define __panel_mainmenu_h__

#include "../ui/uipanel.h"
#include "../frontend/layout_id.h"
#include "../frontend/version_check.h"

FORWARD_REFERENCE(menueventargs);
FORWARD_REFERENCE(uinotificationinterface);
FORWARD_REFERENCE(uitextmenu);

class panel_mainmenu : public uipanel
{
    using uinotificationinterface = chilli::ui::NotificationInterface;
    
public:
    
    virtual bool init() override;
    virtual ~panel_mainmenu() override;

    CREATE_FUNC(panel_mainmenu);
    
    void OnMenuNotification(_UNUSED_ const uinotificationinterface*, menueventargs* );
    virtual void OnNotification( Ref* element ) override;
    virtual bool OnKeyboardEvent( uikeyboardevent* event ) override;

    
protected:
    void OnShowManual();
    void OnShowGuide();
    void OnHelpClose();
    void OnOptions();
    void OnCredits();
    void OnExitGame();
    void OnNewStory();
    void OnContinueStory();
    void OnEndStory();
    void OnShowWelcomeMessage();
    void hideWelcomeMessage(f32 y);
    void showNovella();

    void deleteStory( storyid_t id );
    void refreshStories( void );
    
#if defined(_USE_VERSION_CHECK_)
    void OnUpdate();
    void getVersion();
#endif
    
private:
    uitextmenu* menu;
    std::string versionUrl;
    
    Button* guide;
    Button* story;
    
    
    
};

#endif // __panel_mainmenu_h__
