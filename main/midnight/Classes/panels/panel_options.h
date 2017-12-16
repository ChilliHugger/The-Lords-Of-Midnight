//
//  OptionsScene.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef __PANEL_OPTIONS_H__
#define __PANEL_OPTIONS_H__

#include "uipanel.h"
#include "../ui/uitextmenuitem.h"

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
} option_t ;

#define SET_OPTION(x,z) \
    options[x].var = &mr->config->z;

class panel_options : public uipanel
{
public:
    //static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    CREATE_FUNC(panel_options);

protected:
    void CreateMenu1();
    void SetValues ( void );
    void SetMenu ( int id );
    void SetMenu( uitextmenuitem items[], int elements);
    
    void OnMenuNotification( uinotificationinterface*, menueventargs* );
    
    cocos2d::DrawNode* menu2_background;
    cocos2d::Vector<Node*> fields;
    cocos2d::Menu* menu2;
    
    cocos2d::Map<int, uioptionitem*>    optionControls;
    
};



#endif /* __PANEL_OPTIONS_H__ */
