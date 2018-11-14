//
//  tmemanager.h
//  midnight
//
//  Created by Chris Wild on 15/12/2017.
//
//

#pragma once

#include "../tme_interface.h"

typedef cocos2d::Node mximage;

FORWARD_REFERENCE(imagearray);
FORWARD_REFERENCE(mxcellimage);

class tme_item
{
public:
    tme_item();
    virtual ~tme_item();
    
public:
    mxid        id;
    c_str    symbol;
} ;

class char_data_t : public tme_item
{
public:
    char_data_t();
    virtual ~char_data_t();
    
public:
    c_str       face;
    c_str       shield;
    
    // select screen
    point      select_loc;
    c_str      shortcut;
    c_str      shortcut2;
    
    
} ;

class obj_data_t : public tme_item
{
public:
    obj_data_t();
    virtual ~obj_data_t();
    
public:
    c_str    background;
    c_str    i_big;
    c_str    i_small;
} ;

class race_data_t : public tme_item
{
public:
    race_data_t();
    virtual ~race_data_t();
    
public:
    c_str    body;
    c_str    horse;
    c_str   face;
} ;

class terrain_data_t : public tme_item
{
public:
    terrain_data_t();
    virtual ~terrain_data_t();
    
public:
    c_str           file;
    //imagearray*     graphics;
    //mxcellimage*    mapimage;
    c_str           mapimage;
    int             mapcell;
} ;



class tmemanager 
{
public:
	tmemanager();

    void ResolveTMEData();
    void UnResolveTMEData();
    
public:
    c_ptr               characters;
    c_ptr               races;
    c_ptr               objects;
    c_ptr               terrain;
};



LPCSTR GetCharacterImage(mxid id);
LPCSTR GetCharacterImage(const character& c);
LPCSTR GetCharacterFace(mxid id);
LPCSTR GetCharacterFace(const character& c);
LPCSTR GetCharacterShield(mxid id);
LPCSTR GetCharacterShield(const character& c);
LPCSTR GetObjectBig(mxid id);
LPCSTR GetObjectBig(const anobject& o);
LPCSTR GetObjectSmall(mxid id);
LPCSTR GetObjectSmall(const anobject& o);
LPCSTR GetObjectBackground(mxid id);
LPCSTR GetObjectBackground(const anobject& o);
LPCSTR GetRaceImage(mxid id,BOOL horse);
LPCSTR GetRaceImage(const raceinfo& r,BOOL horse);
LPCSTR GetRaceFace(mxid id);
LPCSTR GetRaceFace(const raceinfo& r);
LPCSTR LOAD_IMAGE( LPCSTR filename );
