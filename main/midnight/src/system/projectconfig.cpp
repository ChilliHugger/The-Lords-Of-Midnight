//
//  ProjectConfig.cpp
//  midnight
//
//  Created by Chris Wild on 14/06/2018.
//

#include "../landscaping/LandscapeColour.h"
#include "projectconfig.h"
#include "moonring.h"
#include "progressmonitor.h"
#include "tmemanager.h"
#include "../library/chilli.h"

projectconfig::projectconfig()
{
}

projectconfig::~projectconfig()
{
}

bool projectconfig::LoadXmlConfig ( LPCSTR scenario, progressmonitor* monitor )
{
    using xml = chilli::lib::xml;
    
    UIDEBUG("Frontend: Scenario Config Loading");
    
    auto config = new chilli::lib::xml();
    
    monitor->Update("Loading Scenario Config",0);
    
    if ( !config->Load ( scenario ) ) {
        SAFEDELETE(config);
        COMPLAIN ( "Cannot load scenario config" );
    }
    
    monitor->Update("Loaded Scenario Config",1);
    
    auto base = config->Find("main");
    if ( base == NULL ) {
        SAFEDELETE(config);
        COMPLAIN ( "invalid config file: <main>" );
    }

    XmlNode* e = nullptr ;

//    if ( pass == 1 ) {
//        // terrain
        UIDEBUG( "Frontend: Loading TERRAIN..." );
        if ( (e = xml::Find( base,"terrain")) ) {
            FOREACHELEMENT(e,t) {
                if ( xml::IsType(t,"terrain") ) {
                    terrain_data_t* d = new terrain_data_t ;
                    d->symbol = xml::ReadStr(t,"id");
                    d->file = xml::ReadStr(t,"file");
                    UIDEBUG( "Frontend: Loading TERRAIN... %s", d->symbol.c_str() );
                    mr->tme->terrain.Add(d);
                    
                    d->mapdensity = xml::ReadBool(t,"mapdensity",false);
                    d->mapcell = xml::ReadInt(t,"mapcell",-1) + 1;
                }
                monitor->Update("Loading Terrain", 1);
            }
        }
//    }

#if defined(_MOUSE_ENABLED_)
        UIDEBUG( "Frontend: Loading MOUSE..." );
        monitor->Update("Loading mouse icons",  0);
        auto zero = point(0,0);
        if ( (e = xml::Find( base, "cursors")) ) {
             FOREACHELEMENT(e,t) {
                if ( xml::IsType(t,"cursor") ) {
                    auto d = new mouse_data_t ;
                    d->file = xml::ReadStr(t,"image");
                    auto a = xml::ReadPoint(t,"anchor", zero);
                    d->anchor = cocos2d::Vec2(a.x, a.y);
                    mr->mouseData.push_back(d);
                }
                monitor->Update("Loading mouse icons", 1);
            }
        }
#endif

//    if ( pass == 1 ) {
//        // mouse
//        UIDEBUG( "Frontend: Loading COMPASS..." );
//        UpdateSplash("Loading compass", progress++);
//        if ( (e = base->Find( "compass")) ) {
//            lib::xml::node* e1 = e->Find("imagelist","gfx");
//            if ( e1 ) {
//                UIDEBUG( "found compass gfx imagelist" );
//                images.compass.Load ( e1, ImageListLoadCallback );
//            }
//        }
//    }
//
//
//    if ( pass == 1 ) {
//        // general
//        UIDEBUG( "Frontend: Loading GENERAL..." );
//        UpdateSplash("Loading General icons", progress++);
//        if ( (e = base->Find( "general")) ) {
//            //images.background = LOAD_IMAGE( e->ReadElement("image","background","file") );
//            //images.bk_army  = LOAD_IMAGE( e->ReadElement("image","bk_army","file") );
//            images.chardead  = LOAD_IMAGE( e->ReadElement("image","chardead","file") );
//            //p_look.compass  = LOAD_IMAGE( e->ReadElement("image","compass","file") );
//            //p_look.floor  = LOAD_IMAGE( e->ReadElement("image","floor","file") );
//            //p_map.map_tokens.Load( e->Find("imagelist","map_tokens") );
//
//            images.icons.Load( e->Find("imagelist","icons"), ImageListLoadCallback);
//            images.lord_status.Load( e->Find("imagelist","lord_status"), ImageListLoadCallback);
//        }
//    }
//
//
//    if ( pass == 1 ) {
//        // characters
        UIDEBUG( "Frontend: Loading CHARACTERS..." );
        monitor->Update("Loading Characters", 0);
        if ( (e = xml::Find(base, "characters")) ) {
            FOREACHELEMENT(e,c) {
                if ( xml::IsType(c,"character") ) {
                    char_data_t* d = new char_data_t ;
                    d->symbol = xml::ReadStr(c,"id");
                    UIDEBUG( "Frontend: Loading CHARACTERS... %s", d->symbol.c_str() );
                    
                    d->face = xml::ReadStr(c,"face","") ;
                    d->shield = xml::ReadStr(c,"shield","") ;
                    d->shortcut_old = xml::ReadChar(c,"shortcut",0);
                    d->shortcut_new = xml::ReadChar(c,"shortcut2",0);
                    mr->tme->characters.Add(d);
                }
                monitor->Update("Loading Characters", 1);
            }
        }
//    }
//
//    if ( pass == 1 ) {
        // objects
        UIDEBUG( "Frontend: Loading OBJECTS..." );
        if ( (e = xml::Find( base,"objects") ) ) {
            FOREACHELEMENT(e,o) {
                if ( xml::IsType(o,"object") ) {
                    obj_data_t* d = new obj_data_t ;
                    d->symbol = xml::ReadStr(o,"id");
                    UIDEBUG( "Frontend: Loading OBJECTS... %s",d->symbol.c_str() );
                    d->i_big = xml::ReadStr(o,"big","") ;
                    d->mapcell = xml::ReadInt(o,"mapcell",0);
                    mr->tme->objects.Add(d);
                }
                monitor->Update("Loading Objects", 1);
            }
        }
//    }
//
//    if ( pass == 1 ) {
        // race
        UIDEBUG( "Frontend: Loading RACES..." );
        if ( (e = xml::Find(base, "races")) ) {
            FOREACHELEMENT(e,r) {
                if ( xml::IsType(r,"race") ) {
                    race_data_t* d = new race_data_t ;
                    d->symbol = xml::ReadStr(r,"id","");
                    UIDEBUG( "Frontend: Loading RACES... %s",d->symbol.c_str() );
                    d->face = xml::ReadStr(r,"face","") ;
                    d->body = xml::ReadStr(r,"body","") ;
                    d->horse = xml::ReadStr(r,"horse","") ;
                    mr->tme->races.Add(d);
                }
                monitor->Update("Loading Races", 1);
            }
        }
//    }
//
//    if ( pass == 1 ) {
//        UIDEBUG("Frontend: Loading SKY and FLOOR");
//        UpdateSplash("Loading Sky & floor", progress++);
//
//        images.floor = LOAD_IMAGE( "terrain/floor.png" );
//        images.sky = LOAD_IMAGE( "terrain/sky_day.png" );
//        images.blank_terrain = LOAD_CELLIMAGE( "terrain/map_blank.png", point(0,0), size(SRC(64),SRC(64)), size(4,4) ) ;
//        images.graffiti = LOAD_IMAGE("terrain/graffiti.png");
//        images.map = LOAD_IMAGE("screens/misc/overview_map.png",TRUE);
//    }
//
    
    monitor->Update("Init Panels", 0);
    
//    if ( pass == 1 ) {
//        UpdateSplash("Init Panels", progress++);
//
//        UIDEBUG("Frontend: Initialise Panels");
//
//        p_look->OnXmlInit(base->Find("panel_look"));
//
//        p_map->OnXmlInit(base->Find("panel_map"));
//        //p_night->OnXmlInit(base->Find("panel_night"));
//        p_select->OnXmlInit(base->Find("panel_select"));
//        //p_think->OnXmlInit(base->Find("panel_think"));
//
//        uilandscape::OnXmlInit(base->Find("uilandscape"));
//    }
    
    
    LandscapeColour::OnXmlInit( xml::Find(base,"uilandscape") );
    
    
    SAFEDELETE(config);
    
    UIDEBUG("Frontend: Scenario Config Loaded");
    
    return TRUE ;
}


