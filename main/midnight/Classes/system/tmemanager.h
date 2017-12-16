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
    //mximage*    face;
    //mximage*    shield;
    //mximage*    body;
    //mximage*    horse;
    
    // select screen
    point       select_loc;
    c_str      shortcut;
    c_str      shortcut2;
    
    
} ;

class obj_data_t : public tme_item
{
public:
    obj_data_t();
    virtual ~obj_data_t();
    
public:
    //mximage*    background;
    //mximage*    i_big;
    //mximage*    i_small;
} ;

class race_data_t : public char_data_t
{
public:
    race_data_t();
    virtual ~race_data_t();
    
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
    int             mapcell;
} ;



class tmemanager 
{
public:
	tmemanager();

    void ResolveTMEData();
    void UnResolveTMEData();
    
public:

private:
    c_ptr               characters;
    c_ptr               races;
    c_ptr               objects;
    c_ptr               terrain;
};
