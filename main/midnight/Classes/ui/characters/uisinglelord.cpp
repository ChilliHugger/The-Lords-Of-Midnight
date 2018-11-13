//
//  uisinglelord.cpp
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//

#include "uisinglelord.h"
#include "../uihelper.h"
#include "../../tme_interface.h"
#include "../../system/tmemanager.h"


uisinglelord::uisinglelord()
{
}

bool uisinglelord::init()
{
    return uilordselect::init();
}

uisinglelord* uisinglelord::createWithLord ( mxid characterid )
{
    auto select = uisinglelord::create();
    select->setLord( characterid );
    return select;
}

bool uisinglelord::setLord( mxid characterid )
{
    char_data_t* data = (char_data_t*)TME_GetEntityUserData( characterid );
    
    character c;
    TME_GetCharacter(c, characterid );
    
#ifdef _DDR_
    mximage* face = NULL ;
    
    if ( data )
        face = data->face ;
    if ( face == NULL ) {
        race_data_t* r_data = (race_data_t*) TME_GetEntityUserData( c.race );
        if ( r_data )
            face = r_data->face;
    }
    //uibutton* child =  new uibutton( new uiface( point(0,0), face, gl->images.fontS, c.shortname ) );
    // TODO: DDR Face with name
#else
    
    //cocos2d::Director::getInstance()->getTextureCache()->addImage((LPCSTR)data->face);
    //auto face = uihelper::CreateImageButton((LPCSTR)data->face, ID_CHAR1+ii, callback);
    //face->setUserData(data);
    auto image = Sprite::create((LPCSTR)data->face);
    addChild(image);
    //addItem(face, CHOOSE_CHAR+ii);
#endif
    
    return true;
}
