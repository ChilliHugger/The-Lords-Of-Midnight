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
    //uitextmenu*     menu;
} option_t ;

#define SET_OPTION(x,z) \
    options[x].var = &mr->settings.z;
    //options[x].menu=y;

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
    void OnNotification(cocos2d::Ref *element, u32 tag );
    
    cocos2d::Node* menu2_background;
    cocos2d::Menu* menu2;
    cocos2d::Vector<Node*> fields;
};



#endif /* __PANEL_OPTIONS_H__ */
