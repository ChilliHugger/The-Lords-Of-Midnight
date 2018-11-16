//
//  tmemanager.cpp
//  midnight
//
//  Created by Chris Wild on 15/12/2017.
//
//

#include "cocos2d.h"
#include "tmemanager.h"



tmemanager::tmemanager()
{
}

void tmemanager::ResolveTMEData ()
{
    for ( u32 ii=0; ii<characters.Count(); ii++ ) {
        char_data_t* d = (char_data_t*)characters[ii];
        d->id  = TME_LinkData(d->symbol,d);
    }
    
    // lets check that all characters have user data
    c_mxid all;
    TME_GetAllCharacters(all);
    
    character c;
    for ( u32 ii=0; ii<all.Count(); ii++ ) {
        TME_GetCharacter(c, all[ii]);
        if ( c.userdata == NULL ) {
            auto u = new char_data_t;
            characters.Add(u);
            u->symbol = TME_GetSymbol(c.id);
            u->id = TME_LinkData(u->symbol,u);
        }
    }
    
    for ( u32 ii=0; ii<races.Count(); ii++ ) {
        auto d = (race_data_t*)races[ii];
        d->id  = TME_LinkData(d->symbol,d);
    }
    
    for ( u32 ii=0; ii<objects.Count(); ii++ ) {
        auto d = (obj_data_t*)objects[ii];
        d->id  = TME_LinkData(d->symbol,d);
    }
    
    for ( u32 ii=0; ii<terrain.Count(); ii++ ) {
        auto d = (terrain_data_t*)terrain[ii];
        d->id  = TME_LinkData(d->symbol,d);
    }
    
    // clear the lord select locations
    for ( u32 ii=0; ii<characters.Count(); ii++ ) {
        auto d = (char_data_t*)characters[ii];
        d->select_loc = point(0,0);
    }
    
    TME_RefreshCurrentCharacter ();
    
}

void tmemanager::UnResolveTMEData()
{
    // destroy characters
    for ( u32 ii=0; ii<characters.Count(); ii++ ) {
        char_data_t* d = (char_data_t*)characters[ii];
        SAFEDELETE    ( d );
    }
    characters.Destroy();
    
    // destroy objects
    for ( u32 ii=0; ii<objects.Count(); ii++ ) {
        obj_data_t* d = (obj_data_t*)objects[ii];
        SAFEDELETE    ( d );
    }
    objects.Destroy();
    
    // destroy races
    for ( u32 ii=0; ii<races.Count(); ii++ ) {
        race_data_t* d = (race_data_t*)races[ii];
        SAFEDELETE    ( d );
    }
    races.Destroy();
    
    // destroy terrain
    for ( u32 ii=0; ii<terrain.Count(); ii++ ) {
        terrain_data_t* d = (terrain_data_t*)terrain[ii];
        SAFEDELETE ( d );
    }
    terrain.Destroy();
}


tme_item::tme_item()
{
}

tme_item::~tme_item()
{
}

char_data_t::char_data_t()
{
    //face = NULL ;
    //shield = NULL ;
    //body = NULL ;
    //horse = NULL ;
    select_loc =point(0,0);
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

LPCSTR GetCharacterImage(mxid id)
{
    character c;
    TME_GetCharacter(c,id);
    return GetCharacterImage(c);
}

LPCSTR GetCharacterImage(const character& c)
{
    return GetRaceImage(MAKE_ID(IDT_RACEINFO,c.race),Character_IsRiding(c));
}

LPCSTR GetCharacterFace(mxid id)
{
    character c;
    TME_GetCharacter(c,id);
    return GetCharacterFace(c);
}

LPCSTR GetCharacterFace(const character& c)
{
    char_data_t* d = (char_data_t*)c.userdata ;
    if ( Character_IsDead(c) )
        return LOAD_IMAGE("f_dead!");
    LPCSTR image = d ? LOAD_IMAGE(d->face) : "" ;
    if ( strlen(image) == 0 )
        return GetRaceFace(MAKE_ID(IDT_RACEINFO,c.race));
    return image;
}

LPCSTR GetCharacterShield(mxid id)
{
    character c;
    TME_GetCharacter(c,id);
    return GetCharacterShield(c);
}

LPCSTR GetCharacterShield(const character& c)
{
    char_data_t* d = (char_data_t*)c.userdata ;
    return d ? LOAD_IMAGE(d->shield) : "" ;
    // TODO if no shield then pickup liege's shield
}


LPCSTR GetObjectBig(mxid id)
{
    anobject o;
    TME_GetObject(o,id);
    return GetObjectBig(o);
}

LPCSTR GetObjectBig(const anobject& o)
{
    obj_data_t* d = (obj_data_t*)o.userdata;
    return d ? LOAD_IMAGE(d->i_big) : "" ;
}


LPCSTR GetObjectSmall(mxid id)
{
    anobject o;
    TME_GetObject(o,id);
    return GetObjectSmall(o);
}

LPCSTR GetObjectSmall(const anobject& o)
{
    obj_data_t* d = (obj_data_t*)o.userdata;
    return d ? LOAD_IMAGE(d->i_small) : "" ;
}


LPCSTR GetObjectBackground(mxid id)
{
    anobject o;
    TME_GetObject(o,id);
    return GetObjectBackground(o);
}

LPCSTR GetObjectBackground(const anobject& o)
{
    obj_data_t* d = (obj_data_t*)o.userdata;
    return d ? LOAD_IMAGE(d->background) : "" ;
}

LPCSTR GetRaceImage(mxid id,BOOL horse)
{
    raceinfo r;
    TME_GetRaceInfo(r,id);
    return GetRaceImage(r,horse);
}

LPCSTR GetRaceImage(const raceinfo& r,BOOL horse)
{
    race_data_t* d = (race_data_t*)r.userdata ;
    if ( d == NULL ) return "" ;
    return LOAD_IMAGE( horse ? d->horse : d->body ) ;
}

LPCSTR GetRaceFace(mxid id)
{
    raceinfo r;
    TME_GetRaceInfo(r,id);
    return GetRaceFace(r);
}

LPCSTR GetRaceFace(const raceinfo& r)
{
    race_data_t* d = (race_data_t*)r.userdata ;
    return d ? LOAD_IMAGE(d->face) : "" ;
}

LPCSTR LOAD_IMAGE( LPCSTR filename )
{
    cocos2d::Director::getInstance()->getTextureCache()->addImage(filename);
    return filename;
}
