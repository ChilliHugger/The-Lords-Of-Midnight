//
//  OptionsScene.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef __PANEL_OPTIONS_H__
#define __PANEL_OPTIONS_H__

#include "../axmol_sdk.h"
#include "../ui/uipanel.h"
#include "../ui/uitextmenuitem.h"
#include "../ui/uitextmenu.h"

FORWARD_REFERENCE(menueventargs);
FORWARD_REFERENCE(uinotificationinterface);
FORWARD_REFERENCE(uioptionitem);
FORWARD_REFERENCE(MenuItemNode);

enum eOptionTypes {
    OPT_NONE=0,
    OPT_TOGGLE=1,
    OPT_NUMBER=2,
};

typedef struct {
    int             id;
    eOptionTypes    type;
    int             max;
    const char**    text;
    void*           var;
    bool            disabled;
    bool            hidden;
} option_t ;

#define SET_OPTION(x,z) \
    findOption(x)->var = &mr->settings->z

class panel_options : public uipanel
{
    using Node = ax::Node;
    using DrawNode = ax::DrawNode;
    using Menu = ax::Menu;
    using ScrollView = ax::ui::ScrollView;
    using uinotificationinterface = chilli::ui::NotificationInterface;
    template<class T> using Vector = ax::Vector<T>;
public:

    virtual bool init() override;
    
    CREATE_FUNC(panel_options);

protected:
    void CreateMainMenu();
    void SetValues ( void );
    void SetMenu ( int id );
    void SetSubMenu( uitextmenuitem items[], int elements);
    void clearRule(int id, bool condition);
    void checkDisabledRules();
    void OnMenuNotification( const uinotificationinterface*, menueventargs* );
    virtual bool OnKeyboardEvent( uikeyboardevent* event ) override;
    virtual void OnNotification( Ref* sender ) override;
    
#if defined(_OS_DESKTOP_)
    void changeDisplayMode();
#endif

#if defined(_FOCUS_ENABLED_)
    virtual std::vector<layoutid_t> getFocusControls() const override;
#endif

protected:
    DrawNode*               menu2_background;
    Vector<MenuItemNode*>   fields;
    ScrollView*             subMenuScrollView;
    uitextmenu*             mainMenu;
    ax::Map<int, uioptionitem*> optionControls;
    CF_SCREEN               initialScreenMode;
    CF_SCENARIO             initialScenario;
    TOGGLE                  rules[64];
};



#endif /* __PANEL_OPTIONS_H__ */
