//
//  ProjectConfig.cpp
//  midnight
//
//  Created by Chris Wild on 14/06/2018.
//

#include "projectconfig.h"
#include "moonring.h"
#include "../library/chilli.h"
#include "progressmonitor.h"
#include "tmemanager.h"

#include "../landscaping/LandscapeColour.h"

projectconfig::projectconfig()
{
}

projectconfig::~projectconfig()
{
}

bool projectconfig::LoadXmlConfig ( LPCSTR scenario, progressmonitor* monitor )
{
    //char path[MAX_PATH];
    
    UIDEBUG("ProjectConfig::LoadXmlConfig - ENTER");
    
    auto config = new chilli::lib::xml();
    
    
    //sprintf( path, "%s/%s", FrontendDirectory(), scenario );
    
    monitor->Update("Loading Config",0);
    
    if ( !config->Load ( scenario ) ) {
        SAFEDELETE(config);
        COMPLAIN ( "Cannot load frontend config" );
    }
    
    monitor->Update("Loaded Config",1);
    
    auto base = config->Find("main");
    if ( base == NULL ) {
        SAFEDELETE(config);
        COMPLAIN ( "invalid config file: <main>" );
    }

    chilli::lib::xml::node* e = NULL ;
//
//    if ( pass == 1 ) {
//        // scenario
//        UpdateSplash("Loading scenario", progress++);
//        if ( (e = base->Find( "scenario")) ) {
//            // id
//            scenarioname = e->ReadStr("id");
//            scenariodir = e->ReadStr("directory");
//
//            // create a blank
//            if ( !TME_Init() ) {
//                COMPLAIN ( "Cannot TME_Init()" );
//            }
//
//        }
//    }
//
//    if ( pass == 1 ) {
//        // terrain
//        UIDEBUG( "Frontend: Loading TERRAIN..." );
//        if ( (e = base->Find( "terrain")) ) {
//            FOREACHELEMENT(e,t) {
//                if ( t->IsType("terrain") ) {
//                    terrain_data_t* d = new terrain_data_t ;
//                    d->symbol = t->ReadStr("id");
//                    d->file = t->ReadStr("file");
//                    UIDEBUG( "Frontend: Loading TERRAIN... %s", (LPCSTR)d->symbol );
//                    terrain.Add(d);
//                    d->graphics = new imagearray(d->symbol,MAX_TERRAIN_SCALES) ;
//                    //for ( int ii=0; ii<MAX_TERRAIN_SCALES; ii++ ) {
//                    if ( strlen(d->file)) {
//                        int ii=0;
//                        sprintf(path,"%s%d.png", (LPCSTR)d->file, ii );
//                        d->graphics->LoadImage(ii,path);
//                        d->graphics->Image(ii)->setClamping(true);
//                        UpdateSplash("Loading Terrain", progress++);
//                        //}
//                    }
//                    d->mapimage = LOAD_CELLIMAGE( t->ReadStr("mapfile"), point(0,0), size(SRC(64),SRC(64)), size(4,4) );
//                    d->mapcell = t->ReadInt("mapcell");
//
//                }
//                UpdateSplash("Loading Terrain", progress++);
//            }
//        }
//    }
//
//
//    if ( pass == 1 ) {
//        // mouse
//        UIDEBUG( "Frontend: Loading MOUSE..." );
//        UpdateSplash("Loading mouse icons", progress++);
//        if ( (e = base->Find( "mouse")) ) {
//            // imagelist "gfx"
//
//            UIDEBUG( "found mouse tag" );
//            lib::xml::node* e1 = e->Find("imagelist","gfx");
//            if ( e1 ) {
//                UIDEBUG( "found mouse gfx imagelist" );
//                images.mouse.Load ( e1, ImageListLoadCallback );
//            }
//            /*
//             lib::xml::node* e2 = e->Find("hotspots");
//             if ( e2 ) {
//             int ii=0;
//             //UIDEBUG( "found mouse hotspots" );
//             FOREACHELEMENT(e2,p) {
//             if ( p->IsType("point") ) {
//             p->ReadPoint("value", mouse_hotspots[ii]);
//             ii++;
//             }
//             }
//             }
//             */
//
//
//        }
//    }
//
//
//
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
        if ( (e = base->Find( "characters")) ) {
            FOREACHELEMENT(e,c) {
                if ( c->IsType("character") ) {
                    char_data_t* d = new char_data_t ;
                    d->symbol = c->ReadStr("id");
                    UIDEBUG( "Frontend: Loading CHARACTERS... %s", (LPCSTR)d->symbol );
                    
                    d->face = c->ReadStr("face") ;
                    //d->face = LOAD_IMAGE( c->ReadStr("face") );
                    //d->shield = LOAD_IMAGE( c->ReadStr("shield") );
                    //d->body = LOAD_IMAGE( c->ReadStr("body") );
                    //d->horse = LOAD_IMAGE( c->ReadStr("horse") );
                    d->shortcut = c->ReadStr("shortcut");
                    d->shortcut2 = c->ReadStr("shortcut2");
                    mr->tme->characters.Add(d);
                }
                monitor->Update("Loading Characters", 1);
            }
        }
//    }
//
//    if ( pass == 1 ) {
//        // objects
//        UIDEBUG( "Frontend: Loading OBJECTS..." );
//        if ( (e = base->Find( "objects") ) ) {
//            FOREACHELEMENT(e,o) {
//                if ( o->IsType("object") ) {
//                    obj_data_t* d = new obj_data_t ;
//                    d->symbol = o->ReadStr("id");
//                    UIDEBUG( "Frontend: Loading OBJECTS... %s", (LPCSTR)d->symbol );
//                    //d->background = LOAD_IMAGE( o->ReadStr("background") );
//                    d->i_big = LOAD_IMAGE( o->ReadStr("big") );
//                    //d->i_small = LOAD_IMAGE( o->ReadStr("small") );
//                    objects.Add(d);
//                }
//                UpdateSplash("Loading Objects", progress++);
//            }
//        }
//    }
//
//    if ( pass == 1 ) {
//        // race
//        UIDEBUG( "Frontend: Loading RACES..." );
//        if ( (e = base->Find( "races")) ) {
//            FOREACHELEMENT(e,r) {
//                if ( r->IsType("race") ) {
//                    race_data_t* d = new race_data_t ;
//                    d->symbol = r->ReadStr("id");
//                    UIDEBUG( "Frontend: Loading RACES... %s", (LPCSTR)d->symbol );
//                    d->face = LOAD_IMAGE( r->ReadStr("face") );
//                    //d->shield = LOAD_IMAGE( r->ReadStr("shield") );
//                    d->body = LOAD_IMAGE( r->ReadStr("body") );
//                    d->horse = LOAD_IMAGE( r->ReadStr("horse") );
//                    races.Add(d);
//                }
//                UpdateSplash("Loading Races", progress++);
//            }
//        }
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
    
    
    LandscapeColour::OnXmlInit( base->Find("uilandscape") );
    
    
    SAFEDELETE(config);
    
    UIDEBUG("Frontend: Config Loaded");
    
    return TRUE ;
}
