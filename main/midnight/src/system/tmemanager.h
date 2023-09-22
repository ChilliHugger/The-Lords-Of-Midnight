//
//  tmemanager.h
//  midnight
//
//  Created by Chris Wild on 15/12/2017.
//
//

#pragma once

#include "../tme/tme_interface.h"

using chilli::collections::c_ptr;

FORWARD_REFERENCE(imagearray);
FORWARD_REFERENCE(mxcellimage);

class tme_item
{
public:
    tme_item();
    virtual ~tme_item();
    
public:
    mxid        id;
    std::string symbol;
} ;

typedef s32 page_t;

class char_data_t : public tme_item
{
public:
    char_data_t();
    virtual ~char_data_t();
    
public:
    std::string     face;
    std::string     shield;
    
    // select screen
    point           select_loc;
    page_t          select_page;
    std::string     shortcut_old;
    std::string     shortcut_new;
    
    
} ;

class obj_data_t : public tme_item
{
public:
    obj_data_t();
    virtual ~obj_data_t();
    
public:
    std::string i_big;
    int         mapcell;
} ;

class race_data_t : public tme_item
{
public:
    race_data_t();
    virtual ~race_data_t();
    
public:
    std::string   body;
    std::string   horse;
    std::string   face;
} ;

class terrain_data_t : public tme_item
{
public:
    terrain_data_t();
    virtual ~terrain_data_t();
    
public:
    std::string file;
    int         mapcell;
    bool        mapdensity;
} ;



class tmemanager 
{
public:
    tmemanager();

    void ResolveTMEData();
    void UnResolveTMEData();
    void resetTMEData();
    
public:
    c_ptr               characters;
    c_ptr               races;
    c_ptr               objects;
    c_ptr               terrain;
};



std::string& GetCharacterImage(mxid id);
std::string& GetCharacterImage(const character& c);
std::string& GetCharacterFace(mxid id);
std::string& GetCharacterFace(const character& c);
std::string& GetCharacterShield(mxid id);
std::string& GetCharacterShield(const character& c);
std::string& GetThingBig(mxid id);
std::string& GetRaceImage(mxid id,BOOL horse);
std::string& GetRaceImage(const raceinfo& r,BOOL horse);
std::string& GetRaceFace(mxid id);
std::string& GetRaceFace(const raceinfo& r);
std::string& LOAD_IMAGE( std::string& filename );

const page_t InvalidPage = 0;


