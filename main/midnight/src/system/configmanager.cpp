//
//  configmanager.cpp
//  midnight
//
//  Created by Chris Wild on 07/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#include "configmanager.h"
#include "moonring.h"
#include "tmemanager.h"
#include "../library/chilli.h"

#ifndef MX_DEBUG
constexpr u32 _DEFAULT_DEBUG_UNDO_HISTORY_ = 1;
#else
constexpr u32 _DEFAULT_DEBUG_UNDO_HISTORY_ = 10;
#endif

configmanager::configmanager() :
    skip_adverts(false),
    skip_dedication(false),
    debug_map(false),
    keep_full_save_history(false),
    always_undo(false),
    undo_history(_DEFAULT_DEBUG_UNDO_HISTORY_)
{
}

configmanager::~configmanager()
{
}

/*
 <?xml version="1.0" encoding="ISO-8859-1" ?>
 <main>
   <variables>
     <var id="cheat_armies_noblock" value="true" />
     <var id="cheat_nasties_noblock" value="true" />
     <var id="cheat_commands_free" value="true" />
     <var id="cheat_movement_cheap" value="false" />
     <var id="cheat_movement_free" value="true" />

     <var id="debug_map" value="false" />
     <var id="cheat_always_win_fight" value="false" />
     <var id="keep_full_save_history" value="false" />
     <var id="cheat_always_undo" value="false" />
     <var id="undo_history" value="10" />
     <var id="start_on_panel" value="MODE_THINK" />
   </variables>
 </main>
 */

bool configmanager::LoadXmlConfig (const std::string& filename )
{
    using xml = chilli::lib::xml;
    
    UIDEBUG("configmanager::LoadXmlConfig - ENTER");
    UIDEBUG("Loading: %s", filename.c_str());
    
    std::unique_ptr<xml> config ( new xml() );

    if ( !config->Load ( filename.c_str() ) ) {
        return false;
    }
    
    auto main = config->Find("main");
    if ( main == nullptr ) {
        COMPLAIN ( "invalid config file: <main>" );
    }
    
    auto variables = main->Find("variables");
    if ( variables == nullptr ) {
        UIDEBUG ( "missing tag: <variables>" );
        return false;
    }
    
#define IS_VAR(x)       c_stricmp(x,name) == 0
#define BOOL_VALUE      t->ReadBool("value")
#define INT_VALUE       t->ReadInt("value")
#define STRING_VALUE    t->ReadStr("value")

    
    FOREACHELEMENT(variables,t) {
        if ( t->IsType("var") ) {
            
            auto name = t->ReadStr("id");
            
            UIDEBUG("VAR %s = '%s'", name, t->ReadStr("value"));
            
            if ( IS_VAR("cheat_armies_noblock") ) {
                tme::variables::sv_cheat_armies_noblock = BOOL_VALUE;
            }
            
            else if ( IS_VAR("cheat_nasties_noblock") ) {
                tme::variables::sv_cheat_nasties_noblock = BOOL_VALUE;
            }
            
            else if ( IS_VAR("cheat_commands_free") ) {
                tme::variables::sv_cheat_commands_free = BOOL_VALUE;
            }
            
            else if ( IS_VAR("cheat_movement_cheap") ) {
                tme::variables::sv_cheat_movement_cheap = BOOL_VALUE;
            }
            
            else if ( IS_VAR("cheat_movement_free") ) {
                tme::variables::sv_cheat_movement_free = BOOL_VALUE;
            }

            else if ( IS_VAR("skip_dedication") ) {
                skip_dedication = BOOL_VALUE;
            }
            
            else if ( IS_VAR("skip_adverts") ) {
                skip_adverts = BOOL_VALUE;
            }
            
            else if (IS_VAR("debug_map")) {
                debug_map = BOOL_VALUE;
            }
            
            else if ( IS_VAR("cheat_always_win_fight") ) {
                tme::variables::sv_cheat_always_win_fight = BOOL_VALUE;
            }
            
            else if ( IS_VAR("keep_full_save_history") ) {
                keep_full_save_history = BOOL_VALUE;
            }
            
            else if ( IS_VAR("cheat_always_undo") ) {
                always_undo = BOOL_VALUE;
            }
            
            else if ( IS_VAR("undo_history") ) {
                undo_history = INT_VALUE;
            }
            
            else if ( IS_VAR("start_on_panel") ) {
                start_on_panel = STRING_VALUE;
            }
            
        }
    }
    
    UIDEBUG("configmanager::LoadXmlConfig - EXIT");
    
    return true ;
}
