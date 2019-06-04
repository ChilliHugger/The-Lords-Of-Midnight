//
//  tmemanager.cpp
//  midnight
//
//  Created by Chris Wild on 15/12/2017.
//
//

#include "../cocos.h"
#include "tmemanager.h"
#include <string>

tmemanager::tmemanager()
{
}

void tmemanager::ResolveTMEData ()
{
    for ( u32 ii=0; ii<characters.Count(); ii++ ) {
        char_data_t* d = static_cast<char_data_t*>(characters[ii]);
        d->id  = TME_LinkData(d->symbol.c_str(),d);
    }
    
    // lets check that all characters have user data
    c_mxid all;
    TME_GetAllCharacters(all);
    
    character c;
    for ( auto id: all) {
        TME_GetCharacter(c, id);
        if ( c.userdata == nullptr ) {
            auto u = new char_data_t;
            characters.Add(u);
            u->symbol = TME_GetSymbol(c.id);
            u->id = TME_LinkData(u->symbol.c_str(),u);
        }
    }
    
    for ( u32 ii=0; ii<races.Count(); ii++ ) {
        auto d = static_cast<race_data_t*>(races[ii]);
        d->id  = TME_LinkData(d->symbol.c_str(),d);
    }
    
    for ( u32 ii=0; ii<objects.Count(); ii++ ) {
        auto d = static_cast<obj_data_t*>(objects[ii]);
        d->id  = TME_LinkData(d->symbol.c_str(),d);
    }
    
    for ( u32 ii=0; ii<terrain.Count(); ii++ ) {
        auto d = static_cast<terrain_data_t*>(terrain[ii]);
        d->id  = TME_LinkData(d->symbol.c_str(),d);
    }

    TME_RefreshCurrentCharacter ();
    
}

void tmemanager::UnResolveTMEData()
{
    // destroy characters
    for ( u32 ii=0; ii<characters.Count(); ii++ ) {
        char_data_t* d = static_cast<char_data_t*>(characters[ii]);
        SAFEDELETE    ( d );
    }
    characters.Destroy();
    
    // destroy objects
    for ( u32 ii=0; ii<objects.Count(); ii++ ) {
        obj_data_t* d = static_cast<obj_data_t*>(objects[ii]);
        SAFEDELETE    ( d );
    }
    objects.Destroy();
    
    // destroy races
    for ( u32 ii=0; ii<races.Count(); ii++ ) {
        race_data_t* d = static_cast<race_data_t*>(races[ii]);
        SAFEDELETE    ( d );
    }
    races.Destroy();
    
    // destroy terrain
    for ( u32 ii=0; ii<terrain.Count(); ii++ ) {
        terrain_data_t* d = static_cast<terrain_data_t*>(terrain[ii]);
        SAFEDELETE ( d );
    }
    terrain.Destroy();
}

void tmemanager::resetTMEData()
{
    // clear the lord select locations
        for ( u32 ii=0; ii<characters.Count(); ii++ ) {
            auto d = static_cast<char_data_t*>(characters[ii]);
            d->select_loc = point::ZERO;
            d->select_page = InvalidPage;
        }
    
}

tme_item::tme_item()
{
}

tme_item::~tme_item()
{
}

char_data_t::char_data_t() :
    select_page(InvalidPage),
    select_loc(point::ZERO)
{
    //face = NULL ;
    //shield = NULL ;
    //body = NULL ;
    //horse = NULL ;
}

char_data_t::~char_data_t()
{
    //IMAGE_RELEASE(face);
    //IMAGE_RELEASE(shield);
    //IMAGE_RELEASE(body);
    //IMAGE_RELEASE(horse);
}

obj_data_t::obj_data_t()
{
    //background = NULL ;
    //i_big = NULL ;
    //i_small = NULL ;
}

obj_data_t::~obj_data_t()
{
    //IMAGE_RELEASE ( background );
    //IMAGE_RELEASE ( i_big );
    //IMAGE_RELEASE ( i_small );
}

race_data_t::race_data_t()
{
    //face = NULL ;
    //shield = NULL ;
    //body = NULL ;
    //horse = NULL ;
}
race_data_t::~race_data_t()
{
    //IMAGE_RELEASE(face);
    //IMAGE_RELEASE(shield);
    //IMAGE_RELEASE(body);
    //IMAGE_RELEASE(horse);
}


terrain_data_t::terrain_data_t()
{
    //graphics = NULL ;
    //mapimage = NULL ;
}

terrain_data_t::~terrain_data_t()
{
    //SAFEDELETE(graphics);
    //IMAGE_RELEASE(mapimage);
}





//
//
//
//
static std::string empty_string;

std::string& GetCharacterImage(mxid id)
{
    character c;
    TME_GetCharacter(c,id);
    return GetCharacterImage(c);
}

std::string& GetCharacterImage(const character& c)
{
    return GetRaceImage(MAKE_ID(IDT_RACEINFO,c.race),Character_IsRiding(c));
}

std::string& GetCharacterFace(mxid id)
{
    character c;
    TME_GetCharacter(c,id);
    return GetCharacterFace(c);
}

std::string& GetCharacterFace(const character& c)
{
    static std::string dead = "f_dead!";
    
    char_data_t* d = static_cast<char_data_t*>(c.userdata) ;
    if ( Character_IsDead(c) )
        return LOAD_IMAGE( dead );
    std::string& image = d ? LOAD_IMAGE(d->face) : empty_string ;
    if ( image.empty() )
        return GetRaceFace(MAKE_ID(IDT_RACEINFO,c.race));
    return image;
}

std::string& GetCharacterShield(mxid id)
{
    character c;
    TME_GetCharacter(c,id);
    return GetCharacterShield(c);
}

std::string& GetCharacterShield(const character& c)
{
    char_data_t* d = static_cast<char_data_t*>(c.userdata) ;
    return d ? LOAD_IMAGE(d->shield) : empty_string ;
    // TODO if no shield then pickup liege's shield
}


std::string& GetObjectBig(mxid id)
{
    object o;
    TME_GetObject(o,id);
    return GetObjectBig(o);
}

std::string& GetObjectBig(const object& o)
{
    obj_data_t* d = static_cast<obj_data_t*>(o.userdata);
    return d ? LOAD_IMAGE(d->i_big) : empty_string ;
}


std::string& GetObjectSmall(mxid id)
{
    object o;
    TME_GetObject(o,id);
    return GetObjectSmall(o);
}

std::string& GetObjectSmall(const object& o)
{
    obj_data_t* d = static_cast<obj_data_t*>(o.userdata);
    return d ? LOAD_IMAGE(d->i_small) : empty_string ;
}


std::string& GetObjectBackground(mxid id)
{
    object o;
    TME_GetObject(o,id);
    return GetObjectBackground(o);
}

std::string& GetObjectBackground(const object& o)
{
    obj_data_t* d = static_cast<obj_data_t*>(o.userdata);
    return d ? LOAD_IMAGE(d->background) : empty_string ;
}

std::string& GetRaceImage(mxid id,BOOL horse)
{
    raceinfo r;
    TME_GetRaceInfo(r,id);
    return GetRaceImage(r,horse);
}

std::string& GetRaceImage(const raceinfo& r,BOOL horse)
{
    race_data_t* d = static_cast<race_data_t*>(r.userdata) ;
    if ( d == nullptr ) return empty_string ;
    return LOAD_IMAGE( horse ? d->horse : d->body ) ;
}

std::string& GetRaceFace(mxid id)
{
    raceinfo r;
    TME_GetRaceInfo(r,id);
    return GetRaceFace(r);
}

std::string& GetRaceFace(const raceinfo& r)
{
    race_data_t* d = static_cast<race_data_t*>(r.userdata) ;
    return d ? LOAD_IMAGE(d->face) : empty_string ;
}

std::string& LOAD_IMAGE( std::string& filename )
{
    if ( !filename.empty() )
        cocos2d::Director::getInstance()->getTextureCache()->addImage(filename);
    return filename;
}
