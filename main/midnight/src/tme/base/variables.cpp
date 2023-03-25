/*
 * FILE:    variables.cpp
 *
 * PROJECT: MidnightEngine
 *
 * CREATED:
 *
 * AUTHOR:  Chris Wild
 *
 * Copyright 2011 Chilli Hugger. All rights reserved.
 *
 * PURPOSE:
 *
 *
 */

#include "../baseinc/tme_internal.h"


#include <stdlib.h>
#include <string.h>
#include <string>


namespace tme {
    namespace variables {
        sv_num_t    sv_database_version;
        sv_num_t    sv_battle_default_energy_drain;
        sv_num_t    sv_battle_default_char_energy_drain;
        sv_num_t    sv_battle_success_regiment_warriors;
        sv_num_t    sv_battle_success_regiment_riders;
        sv_num_t    sv_character_default_energy_inc;
        sv_num_t    sv_character_recruit_amount;
        sv_num_t    sv_character_guard_amount;
        sv_num_t    sv_character_max_riders;
        sv_num_t    sv_character_max_warriors;
        sv_num_t    sv_character_max_energy;
        sv_num_t    sv_warriors_max_energy;
        sv_num_t    sv_warriors_default_energy_inc;
        sv_num_t    sv_riders_default_energy_inc    ;
        sv_num_t    sv_riders_max_energy    ;
        sv_num_t    sv_object_energy_shelter    ;
        sv_num_t    sv_object_energy_shadowsofdeath ;
        sv_num_t    sv_object_energy_watersoflife ;
        sv_num_t    sv_lookforwarddistance ;
        sv_num_t    sv_regiment_default_moves ;
        sv_num_t    sv_stronghold_default_empty ;
        sv_num_t    sv_stronghold_default_min ;
        sv_num_t    sv_stronghold_default_max ;
        sv_bool_t   sv_stronghold_default_allow_respawn ;
        sv_num_t    sv_stronghold_default_respawn_amount ;
        sv_num_t    sv_success_fey_on_horse_adjuster    ;
        sv_num_t    sv_success_base_level ;
        sv_num_t    sv_success_riders_energy_mountain ;
        sv_num_t    sv_success_riders_energy_other ;
        sv_bool_t   sv_always_attempt_recruit ;

        sv_str_t    sv_map_file ;
        sv_num_t    sv_map_width ;
        sv_num_t    sv_map_height ;
        sv_bool_t   sv_auto_unhide ;
        sv_bool_t   sv_auto_seek ;
        sv_bool_t   sv_auto_approach ;
        sv_num_t    sv_character_default_memory_age ;

        sv_int_t    sv_time_night ;
        sv_int_t    sv_time_dawn ;
        sv_int_t    sv_time_scale ;

        sv_int_t    sv_despondent_scale ;
        sv_int_t    sv_fear_scale ;
        sv_int_t    sv_courage_scale ;
        sv_int_t    sv_energy_scale ;
        sv_int_t    sv_reckless_scale ;

        sv_bool_t   sv_collate_battle_areas ;
        
        sv_mxid_t   sv_character_friend;
        sv_mxid_t   sv_character_foe;
        sv_c_mxid_t sv_character_default;
        sv_c_mxid_t sv_guidance;

        sv_int_t    sv_characters ;
        sv_int_t    sv_routenodes ;
        sv_int_t    sv_strongholds ;
        sv_int_t    sv_regiments ;
        sv_int_t    sv_places ;
        sv_int_t    sv_directions ;
        sv_int_t    sv_objects ;
        sv_int_t    sv_units ;
        sv_int_t    sv_races ;
        sv_int_t    sv_genders ;
        sv_int_t    sv_terrains ;
        sv_int_t    sv_areas ;
        sv_int_t    sv_commands ;
        sv_int_t    sv_missions ;
        sv_int_t    sv_victories ;

#if defined(_DDR_)
        sv_int_t    sv_object_powers ;
        sv_int_t    sv_object_types ;
#endif
        
        sv_int_t    sv_days ;
        sv_int_t    sv_attributes ;
        sv_int_t    sv_variables ;
        sv_int_t    sv_strongholdadjuster ;
        sv_int_t    sv_controlled_character ;

//        sv_bool_t    sv_failed_approach_battle ;
//        sv_bool_t    sv_use_cowardess ;
//        sv_bool_t    sv_use_despondency ;
        sv_int_t    sv_energy_max ;
        sv_int_t    sv_strength_max ;

        sv_bool_t   sv_cheat_armies_noblock ;
        sv_bool_t   sv_cheat_nasties_noblock ;
        sv_bool_t   sv_cheat_movement_free ;
        sv_bool_t   sv_cheat_movement_cheap ;
        sv_bool_t   sv_cheat_commands_free ;
        sv_bool_t   sv_cheat_always_win_fight ;
        sv_bool_t   sv_display_no_mist ;
        
        sv_int_t    sv_energy_cannot_continue;
        
#define VARIABLE(a,b,c,d) { &a, b, c, d, nullptr }
        
        cvarreg_t var_array [] = {

            VARIABLE( sv_database_version,                      "DATABASE_VERSION",                     CVar::NUMBER,   "1.00" ),
            VARIABLE( sv_battle_default_energy_drain,           "BATTLE_DEFAULT_ENERGY_DRAIN",          CVar::NUMBER,   "24" ),
            VARIABLE( sv_battle_default_char_energy_drain,      "BATTLE_DEFAULT_CHAR_ENERGY_DRAIN",     CVar::NUMBER,   "20" ),
            VARIABLE( sv_battle_success_regiment_warriors,      "BATTLE_SUCCESS_REGIMENT_WARRIORS",     CVar::NUMBER,   "5" ),
            VARIABLE( sv_battle_success_regiment_riders,        "BATTLE_SUCCESS_REGIMENT_RIDERS",       CVar::NUMBER,   "4" ),
            VARIABLE( sv_character_default_energy_inc,          "CHARACTER_DEFAULT_ENERGY_INC",         CVar::NUMBER,   "9" ),
            VARIABLE( sv_character_recruit_amount,              "CHARACTER_RECRUIT_AMOUNT",             CVar::NUMBER,   "100" ),
            VARIABLE( sv_character_guard_amount,                "CHARACTER_GUARD_AMOUNT",               CVar::NUMBER,   "100" ),
            VARIABLE( sv_character_max_riders,                  "CHARACTER_MAX_RIDERS",                 CVar::NUMBER,   "1200" ),
            VARIABLE( sv_character_max_warriors,                "CHARACTER_MAX_WARRIORS",               CVar::NUMBER,   "1200" ),
            VARIABLE( sv_character_max_energy,                  "CHARACTER_MAX_ENERGY",                 CVar::NUMBER,   "127" ),
            VARIABLE( sv_warriors_max_energy,                   "WARRIORS_MAX_ENERGY",                  CVar::NUMBER,   "127" ),
            VARIABLE( sv_warriors_default_energy_inc,           "WARRIORS_DEFAULT_ENERGY_INC",          CVar::NUMBER,   "4" ),
            VARIABLE( sv_riders_default_energy_inc,             "RIDERS_DEFAULT_ENERGY_INC",            CVar::NUMBER,   "6" ),
            VARIABLE( sv_riders_max_energy,                     "RIDERS_MAX_ENERGY",                    CVar::NUMBER,   "127" ),
            VARIABLE( sv_object_energy_shelter,                 "OBJECT_ENERGY_SHELTER",                CVar::NUMBER,   "10" ),
            VARIABLE( sv_object_energy_shadowsofdeath,          "OBJECT_ENERGY_SHADOWSOFDEATH",         CVar::NUMBER,   "0" ),
            VARIABLE( sv_object_energy_watersoflife,            "OBJECT_ENERGY_WATERSOFLIFE",           CVar::NUMBER,   "120" ),
            VARIABLE( sv_lookforwarddistance,                   "LOOKFORWARDDISTANCE",                  CVar::NUMBER,   "3" ),
            VARIABLE( sv_regiment_default_moves,                "REGIMENT_DEFAULT_MOVES",               CVar::NUMBER,   "6" ),
            VARIABLE( sv_stronghold_default_empty,              "STRONGHOLD_DEFAULT_EMPTY",             CVar::NUMBER,   "20" ),
            VARIABLE( sv_stronghold_default_min,                "STRONGHOLD_DEFAULT_MIN",               CVar::NUMBER,   "100" ),
            VARIABLE( sv_stronghold_default_max,                "STRONGHOLD_DEFAULT_MAX",               CVar::NUMBER,   "1200" ),
            VARIABLE( sv_stronghold_default_allow_respawn,      "STRONGHOLD_DEFAULT_ALLOW_RESPAWN",     CVar::YESNO,    "NO" ),
            VARIABLE( sv_stronghold_default_respawn_amount,     "STRONGHOLD_DEFAULT_RESPAWN_AMOUNT",    CVar::NUMBER,   "50" ),
            VARIABLE( sv_success_fey_on_horse_adjuster,         "SUCCESS_FEY_ON_HORSE_ADJUSTER",        CVar::NUMBER,   "64" ),
            VARIABLE( sv_success_base_level,                    "SUCCESS_BASE_LEVEL",                   CVar::NUMBER,   "24" ),
            VARIABLE( sv_success_riders_energy_mountain,        "SUCCESS_RIDERS_ENERGY_MOUNTAIN",       CVar::NUMBER,   "32" ),
            VARIABLE( sv_success_riders_energy_other,           "SUCCESS_RIDERS_ENERGY_OTHER",          CVar::NUMBER,   "64" ),
            VARIABLE( sv_always_attempt_recruit,                "ALWAYS_ATTEMPT_RECRUIT",               CVar::YESNO,    "FALSE" ),

            VARIABLE( sv_map_file,                              "MAP_FILE",                             CVar::STRING,   "map" ),
            VARIABLE( sv_map_width,                             "MAP_WIDTH",                            CVar::NUMBER,   "66" ),
            VARIABLE( sv_map_height,                            "MAP_HEIGHT",                           CVar::NUMBER,   "62" ),

            VARIABLE( sv_auto_unhide,                           "CHARACTER_AUTO_UNHIDE",                CVar::YESNO,    "NO" ),
            VARIABLE( sv_auto_seek,                             "CHARACTER_AUTO_SEEK",                  CVar::YESNO,    "NO" ),
            VARIABLE( sv_auto_approach,                         "CHARACTER_AUTO_APPROACH",              CVar::YESNO,    "NO" ),
            VARIABLE( sv_character_default_memory_age,          "CHARACTER_DEFAULT_MEMORY_AGE",         CVar::NUMBER,   "10" ),


//          VARIABLE( sv_use_cowardess,                         "USE_COWARDESS",                        CVar::YESNO,    "YES" } ,
//          VARIABLE( sv_use_despondency,                       "USE_DESPONDENCY",                      CVar::YESNO,    "NO" } ,

            VARIABLE( sv_time_night,                            "TIME_NIGHT",                           CVar::INT,      "0" ),
            VARIABLE( sv_time_dawn,                             "TIME_DAWN",                            CVar::INT,      "16" ), // lords 16 ddr 31
            VARIABLE( sv_time_scale,                            "TIME_SCALE",                           CVar::INT,      "2" ), // lords 2 ddr 4
            VARIABLE( sv_energy_max,                            "ENERGY_MAX",                           CVar::INT,      "127" ),
            VARIABLE( sv_strength_max,                          "STRENGTH_MAX",                         CVar::INT,      "100" ) ,
            VARIABLE( sv_fear_scale,                            "FEAR_SCALE",                           CVar::INT,      "64" ),
            VARIABLE( sv_courage_scale,                         "COURAGE_SCALE",                        CVar::INT,      "1" ),
            VARIABLE( sv_energy_scale,                          "ENERGY_SCALE",                         CVar::INT,      "16" ), // lom  16 ddr 32
            VARIABLE( sv_despondent_scale,                      "DESPONDENT_SCALE",                     CVar::INT,      "32" ),
            VARIABLE( sv_reckless_scale,                        "RECKLESS_SCALE",                       CVar::INT,      "32" ),
            VARIABLE( sv_collate_battle_areas,                  "COLLATE_BATTLE_AREAS",                 CVar::YESNO,    "NO" ),
            VARIABLE( sv_character_friend,                      "CHARACTER_FRIEND",                     CVar::IDT,      "CH_LUXOR" ),
            VARIABLE( sv_character_foe,                         "CHARACTER_FOE",                        CVar::IDT,      "CH_DOOMDARK" ),
            VARIABLE( sv_character_default,                     "CHARACTER_DEFAULT",                    CVar::IDARRAY,  "CH_LUXOR|CH_MORKIN|CH_CORLETH|CH_RORTHRON" ),
            VARIABLE( sv_guidance,                              "SEEK_MESSAGES",                        CVar::IDARRAY,  "" ),
            VARIABLE( sv_characters,                            "CHARACTERS",                           CVar::INT,      "0" ),
            VARIABLE( sv_routenodes,                            "ROUTENODES",                           CVar::INT,      "0" ),
            VARIABLE( sv_strongholds,                           "STRONGHOLDS",                          CVar::INT,      "0" ),
            VARIABLE( sv_regiments,                             "REGIMENTS",                            CVar::INT,      "0" ),
            VARIABLE( sv_places,                                "PLACES",                               CVar::INT,      "0" ),
            VARIABLE( sv_directions,                            "DIRECTIONS",                           CVar::INT,      "0" ),
            VARIABLE( sv_objects,                               "OBJECTS",                              CVar::INT,      "0" ),
            VARIABLE( sv_units,                                 "UNITS",                                CVar::INT,      "0" ),
            VARIABLE( sv_races,                                 "RACES",                                CVar::INT,      "0" ),
            VARIABLE( sv_genders,                               "GENDERS",                              CVar::INT,      "0" ),
            VARIABLE( sv_terrains,                              "TERRAINS",                             CVar::INT,      "0" ),
            VARIABLE( sv_areas,                                 "AREAS",                                CVar::INT,      "0" ),
            VARIABLE( sv_commands,                              "COMMANDS",                             CVar::INT,      "0" ),
            VARIABLE( sv_missions,                              "MISSIONS",                             CVar::INT,      "0" ),
            VARIABLE( sv_victories,                             "VICTORIES",                            CVar::INT,      "0" ),
            VARIABLE( sv_attributes,                            "ATTRIBUTES",                           CVar::INT,      "0" ),
            VARIABLE( sv_variables,                             "VARIABLES",                            CVar::INT,      "0" ),
            VARIABLE( sv_days,                                  "DAYS",                                 CVar::INT,      "0" ),
            VARIABLE( sv_strongholdadjuster,                    "STRONGHOLD_ADJUSTER",                  CVar::INT,      "0" ),
            VARIABLE( sv_controlled_character,                  "CONTROLLED_CHARACTER",                 CVar::INT,      "0" ),
            VARIABLE( sv_energy_cannot_continue,                "ENERGY_CANNOT_CONTINUE",               CVar::INT,      "0" ),
#if defined(_DDR_)
            VARIABLE( sv_object_powers,                         "OBJECT_POWERS",                        CVar::INT,      "0" ),
            VARIABLE( sv_object_types,                          "OBJECT_TYPES",                         CVar::INT,      "0" ),
#endif
        };

        void sv_Initialise ( cvarreg_t* variable ) 
        {
            LPCSTR stringValue =  (variable->currentValue == nullptr)
                                ? variable->defaultValue
                                : variable->currentValue ;

            switch ( variable->type    ) {

                case CVar::VNONE:
                    break;
                case CVar::DOUBLE:
                    break;
                case CVar::FLOAT:
                    break;
                case CVar::PTR:
                    break;
                    
                case CVar::NUMBER:
                    *((sv_num_t*)variable->memory) = (f32)chilli::lib::atol(stringValue);
                    break;

                case CVar::YESNO:
                    *((sv_bool_t*)variable->memory) = 0;

                    if (c_stricmp(stringValue,"TRUE") == 0
                        || c_stricmp(stringValue,"YES") == 0 )
                        *((sv_bool_t*)variable->memory) = 1;

                    break;

                case CVar::INT:
                    *((sv_int_t*)variable->memory) = (s32)chilli::lib::atol(stringValue);
                    break;

                case CVar::STRING:
                    *((sv_str_t*)variable->memory) = (char*)stringValue;
                    break;

                case CVar::IDT:
                {
                    mxid id = mxentity::SafeIdt(mx->EntityByName(stringValue));
                    if ( id == IDT_NONE )
                        id = mx->text->StringByName(stringValue);
                    *((sv_mxid_t*)variable->memory) = id;
                }
                    break;

                case CVar::IDARRAY:
                    {
                        sv_c_mxid_t* c = (sv_c_mxid_t*)variable->memory ;
                        
                        c_string values;
                        std::string value = stringValue;
                        StringExtensions::split(value, '|', values);

                        c->Clear();
                        for( auto token : values) {
                            if(token == "-") {
                                c->Add(IDT_NONE);
                            }else{
                                mxid id = mxentity::SafeIdt(mx->EntityByName(token));
                                if ( id == IDT_NONE )
                                    id = mx->text->StringByName(token);
                                c->Add( id );
                            }
                        }
                    }
                    break;
            }
        }


        void DeInit()
        {
            for ( u32 ii=0; ii<NUMELE(var_array); ii++ ) {
                if ( var_array[ii].type == CVar::IDARRAY ) {
                    sv_c_mxid_t* c = (sv_c_mxid_t*) var_array[ii].memory ;
                        c->Clear();
                }
                var_array[ii].currentValue = nullptr;
            }
        }

        void Init ( int pass )
        {
            for ( u32 ii=0; ii<NUMELE(var_array); ii++ ) {
                if ( pass == 1 ) {
                    var_array[ii].currentValue = nullptr;
                    if ( var_array[ii].type != CVar::IDARRAY && var_array[ii].type != CVar::IDT)
                        sv_Initialise( &var_array[ii] );
                }else if ( pass == 2 ) {
                    if ( var_array[ii].type == CVar::IDARRAY || var_array[ii].type == CVar::IDT)
                        sv_Initialise( &var_array[ii] );
                }
            }
        }

        void Update ( void )
        {
            for ( u32 ii=0; ii<NUMELE(var_array); ii++ ) {
                cvarreg_t* var = mx->FindDBVariable( var_array[ii].name );
                if ( var ) {
                    var_array[ii].currentValue = var->currentValue;
                    sv_Initialise( &var_array[ii] );
                }
            }

            //
        }

        MXRESULT GetProperty(const std::string& name, variant& arg)
        {
            arg.vType = variant::none ;
            arg.vSInt32 = 0 ;

            cvarreg_t* var = NULL ;

            for ( u32 ii=0; ii<NUMELE(var_array); ii++ ) {
                if ( c_stricmp(name.c_str(), var_array[ii].name) == 0 ) {
                    var = &var_array[ii];
                    break;
                }
            }

            if ( var != NULL ) {
            switch ( var->type    ) {
                case CVar::NUMBER:
                    arg = *((sv_num_t*)var->memory);
                    arg.vType = variant::vdouble ;
                    return MX_OK ;
                case CVar::YESNO:
                    arg = *((sv_int_t*)var->memory);
                    arg.vType = variant::vyesno ;
                    return MX_OK ;
                case CVar::INT:
                    arg = *((sv_int_t*)var->memory);
                    //arg = *( (s32*) var->memory );
                    arg.vType = variant::vnumber ;
                    return MX_OK ;
                case CVar::STRING:
                    arg = *((sv_str_t*)var->memory);
                    arg.vType = variant::vstring ;
                    return MX_OK ;
                    
                default:
                    break;
            }
            }

            return MX_FAILED;
        }

        MXRESULT SetProperty(const std::string& name, const std::string& value)
        {
            cvarreg_t* var = NULL ;
            
            for ( u32 ii=0; ii<NUMELE(var_array); ii++ ) {
                if (c_stricmp( name.c_str(), var_array[ii].name ) == 0 ) {
                    var = &var_array[ii];
                    break;
                }
            }
            
            if ( var == nullptr )
                return MX_FAILED;
            
            var->currentValue = value.c_str();
            sv_Initialise( var );
            
            return MX_OK;
        }

        CVar::~CVar()
        {
        }

    }
    // namespace variables

}
// namespace tme



