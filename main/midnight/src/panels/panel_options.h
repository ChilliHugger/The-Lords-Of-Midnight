//
//  OptionsScene.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef __PANEL_OPTIONS_H__
#define __PANEL_OPTIONS_H__

#include "../cocos.h"
#include "../ui/uipanel.h"
#include "../ui/uitextmenuitem.h"
#include "../ui/uitextmenu.h"

FORWARD_REFERENCE(menueventargs);
FORWARD_REFERENCE(uinotificationinterface);
FORWARD_REFERENCE(uioptionitem);

enum eOptionTypes {
    OPT_NONE=0,
    OPT_BOOL=1,
    OPT_NUMBER=2,
};

typedef struct {
    int             id;
    eOptionTypes    type;
    int             max;
    const char**    text;
    void*           var;
    bool            disabled;    
} option_t ;

#define SET_OPTION(x,z) \
    findOption(x)->var = &mr->settings->z;

class panel_options : public uipanel
{
    using Node = cocos2d::Node;
    using DrawNode = cocos2d::DrawNode;
    using Menu = cocos2d::Menu;
    using uinotificationinterface = chilli::ui::NotificationInterface;
    template<class T> using Vector = cocos2d::Vector<T>;
public:

    virtual bool init() override;
    
    CREATE_FUNC(panel_options);

protected:
    void CreateMainMenu();
    void SetValues ( void );
    void SetMenu ( int id );
    void SetMenu( uitextmenuitem items[], int elements);
    
    void OnMenuNotification( const uinotificationinterface*, menueventargs* );
    virtual bool OnKeyboardEvent( uikeyboardevent* event ) override;
    virtual void OnNotification( Ref* sender ) override;
    
#if defined(_OS_DESKTOP_)
    void changeDisplayMode();
#endif

protected:
    DrawNode*               menu2_background;
    Vector<Node*>           fields;
    Menu*                   subMenu;
    uitextmenu*             mainMenu;
    cocos2d::Map<int, uioptionitem*> optionControls;
    CONFIG_SCREEN_MODE      initialScreenMode;
};



#endif /* __PANEL_OPTIONS_H__ */
