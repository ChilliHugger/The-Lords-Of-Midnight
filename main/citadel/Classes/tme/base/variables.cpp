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


namespace tme {
	namespace variables {
		sv_num_t	sv_database_version;
		sv_num_t	sv_battle_default_energy_drain;
		sv_num_t	sv_battle_default_char_energy_drain;
		sv_num_t	sv_battle_success_regiment_warriors;
		sv_num_t	sv_battle_success_regiment_riders;
		sv_num_t	sv_character_default_energy_inc;
		sv_num_t	sv_character_recruit_amount;
		sv_num_t	sv_character_guard_amount;
		sv_num_t	sv_character_max_riders;
		sv_num_t	sv_character_max_warriors;
		sv_num_t	sv_character_max_energy;
		sv_num_t	sv_warriors_max_energy;
		sv_num_t	sv_warriors_default_energy_inc;
		sv_num_t	sv_riders_default_energy_inc	;
		sv_num_t	sv_riders_max_energy	;
		sv_num_t	sv_object_energy_shelter	;
		sv_num_t	sv_object_energy_shadowsofdeath ;
		sv_num_t	sv_object_energy_watersoflife ;
		sv_num_t	sv_lookforwarddistance ;
		sv_num_t	sv_regiment_default_moves ;
		sv_num_t	sv_stronghold_default_empty ;
		sv_num_t	sv_stronghold_default_min ;
		sv_num_t	sv_stronghold_default_max ;
		sv_bool_t	sv_stronghold_default_allow_respawn ;
		sv_num_t	sv_stronghold_default_respawn_amount ;
		sv_num_t	sv_success_fey_on_horse_adjuster	;
		sv_num_t	sv_success_base_level ;
		sv_num_t	sv_success_riders_energy_mountain ;
		sv_num_t	sv_success_riders_energy_other ;
		sv_bool_t	sv_always_attempt_recruit ;

		sv_str_t	sv_map_file ;
		sv_num_t	sv_map_width ;
		sv_num_t	sv_map_height ;
		sv_bool_t	sv_auto_unhide ;
		sv_bool_t	sv_auto_seek ;
		sv_num_t	sv_character_default_memory_age ;

		sv_int_t	sv_time_night ;
		sv_int_t	sv_time_dawn ;
		sv_int_t	sv_time_scale ;

		sv_int_t	sv_despondent_scale ;
		sv_int_t	sv_fear_scale ;
		sv_int_t	sv_courage_scale ;
		sv_int_t	sv_energy_scale ;
		sv_int_t	sv_reckless_scale ;

        sv_bool_t   sv_collate_battle_areas ;
        
		sv_mxid_t	sv_character_friend;
		sv_mxid_t	sv_character_foe;
		sv_c_mxid_t	sv_character_default;
		sv_c_mxid_t	sv_guidance;

		sv_int_t	sv_characters ;
		sv_int_t	sv_routenodes ;
		sv_int_t	sv_strongholds ;
		sv_int_t	sv_regiments ;
		sv_int_t	sv_places ;
		sv_int_t	sv_directions ;
		sv_int_t	sv_objects ;
		sv_int_t	sv_units ;
		sv_int_t	sv_races ;
		sv_int_t	sv_genders ;
		sv_int_t	sv_terrains ;
		sv_int_t	sv_areas ;
		sv_int_t	sv_commands ;
		sv_int_t	sv_missions ;
		sv_int_t	sv_victories ;

#if defined(_DDR_)
		sv_int_t	sv_object_powers ;
		sv_int_t	sv_object_types ;
#endif
        
		sv_int_t	sv_days ;
		sv_int_t	sv_attributes ;
		sv_int_t	sv_variables ;
		sv_int_t	sv_strongholdadjuster ;
		sv_int_t	sv_controlled_character ;

//		sv_bool_t	sv_failed_approach_battle ;
//		sv_bool_t	sv_use_cowardess ;
//		sv_bool_t	sv_use_despondency ;
		sv_int_t	sv_energy_max ;
		sv_int_t	sv_strength_max ;

		sv_bool_t	sv_cheat_armies_noblock ;
		sv_bool_t	sv_cheat_nasties_noblock ;
		sv_bool_t	sv_cheat_movement_free ;
		sv_bool_t	sv_cheat_movement_cheap ;
		sv_bool_t	sv_cheat_commands_free ;
		
        sv_int_t    sv_energy_cannot_continue;
		
		cvarreg_t var_array [] = {

			{ &sv_database_version,					"DATABASE_VERSION",					CVar::NUMBER,	"1.00" },	
			{ &sv_battle_default_energy_drain,		"BATTLE_DEFAULT_ENERGY_DRAIN",		CVar::NUMBER,	"24" },		
			{ &sv_battle_default_char_energy_drain,	"BATTLE_DEFAULT_CHAR_ENERGY_DRAIN",	CVar::NUMBER,	"20" },
			{ &sv_battle_success_regiment_warriors,	"BATTLE_SUCCESS_REGIMENT_WARRIORS",	CVar::NUMBER,	"5" },
			{ &sv_battle_success_regiment_riders,	"BATTLE_SUCCESS_REGIMENT_RIDERS",	CVar::NUMBER,	"4" },
			{ &sv_character_default_energy_inc,		"CHARACTER_DEFAULT_ENERGY_INC",		CVar::NUMBER,	"9" },
			{ &sv_character_recruit_amount,			"CHARACTER_RECRUIT_AMOUNT",			CVar::NUMBER,	"100" },
			{ &sv_character_guard_amount,			"CHARACTER_GUARD_AMOUNT",			CVar::NUMBER,	"100" },
			{ &sv_character_max_riders,				"CHARACTER_MAX_RIDERS",				CVar::NUMBER,	"1200" },
			{ &sv_character_max_warriors,			"CHARACTER_MAX_WARRIORS",			CVar::NUMBER,	"1200" },
			{ &sv_character_max_energy,				"CHARACTER_MAX_ENERGY",				CVar::NUMBER,	"127" },
			{ &sv_warriors_max_energy,				"WARRIORS_MAX_ENERGY",				CVar::NUMBER,	"127" },
			{ &sv_warriors_default_energy_inc,		"WARRIORS_DEFAULT_ENERGY_INC",		CVar::NUMBER,	"4" },
			{ &sv_riders_default_energy_inc,		"RIDERS_DEFAULT_ENERGY_INC",		CVar::NUMBER,	"6" },
			{ &sv_riders_max_energy,				"RIDERS_MAX_ENERGY",				CVar::NUMBER,	"127" },
			{ &sv_object_energy_shelter,			"OBJECT_ENERGY_SHELTER",			CVar::NUMBER,	"10" },
			{ &sv_object_energy_shadowsofdeath,		"OBJECT_ENERGY_SHADOWSOFDEATH",		CVar::NUMBER,	"0" },
			{ &sv_object_energy_watersoflife,		"OBJECT_ENERGY_WATERSOFLIFE",		CVar::NUMBER,	"120" },
			{ &sv_lookforwarddistance,				"LOOKFORWARDDISTANCE",				CVar::NUMBER,	"3" },
			{ &sv_regiment_default_moves,			"REGIMENT_DEFAULT_MOVES",			CVar::NUMBER,	"6" },
			{ &sv_stronghold_default_empty,			"STRONGHOLD_DEFAULT_EMPTY",			CVar::NUMBER,	"20" },
			{ &sv_stronghold_default_min,			"STRONGHOLD_DEFAULT_MIN",			CVar::NUMBER,	"100" },
			{ &sv_stronghold_default_max,			"STRONGHOLD_DEFAULT_MAX",			CVar::NUMBER,	"1200" },
			{ &sv_stronghold_default_allow_respawn,	"STRONGHOLD_DEFAULT_ALLOW_RESPAWN",	CVar::YESNO,	"NO" },
			{ &sv_stronghold_default_respawn_amount,"STRONGHOLD_DEFAULT_RESPAWN_AMOUNT",CVar::NUMBER,	"50" },
			{ &sv_success_fey_on_horse_adjuster,	"SUCCESS_FEY_ON_HORSE_ADJUSTER",	CVar::NUMBER,	"64" },
			{ &sv_success_base_level,				"SUCCESS_BASE_LEVEL",				CVar::NUMBER,	"24" },
			{ &sv_success_riders_energy_mountain,	"SUCCESS_RIDERS_ENERGY_MOUNTAIN",	CVar::NUMBER,	"32" },
			{ &sv_success_riders_energy_other,		"SUCCESS_RIDERS_ENERGY_OTHER",		CVar::NUMBER,	"64" },
			{ &sv_always_attempt_recruit,			"ALWAYS_ATTEMPT_RECRUIT",			CVar::YESNO,	"FALSE" },

			{ &sv_map_file,							"MAP_FILE",							CVar::STRING,	"map" },
			{ &sv_map_width,						"MAP_WIDTH",						CVar::NUMBER,	"66" },
			{ &sv_map_height,						"MAP_HEIGHT",						CVar::NUMBER,	"62" },

			{ &sv_auto_unhide,						"CHARACTER_AUTO_UNHIDE",			CVar::YESNO,	"NO" },
			{ &sv_auto_seek,						"CHARACTER_AUTO_SEEK",				CVar::YESNO,	"NO" },
			{ &sv_character_default_memory_age,		"CHARACTER_DEFAULT_MEMORY_AGE",		CVar::NUMBER,	"10" },


//			{ &sv_use_cowardess,					"USE_COWARDESS",					CVar::YESNO,	"YES" } ,
//			{ &sv_use_despondency,					"USE_DESPONDENCY",					CVar::YESNO,	"NO" } ,

			{ &sv_time_night,						"TIME_NIGHT",						CVar::INT,		"0" },
			{ &sv_time_dawn,						"TIME_DAWN",						CVar::INT,		"16" }, // lords 16 ddr 31
			{ &sv_time_scale,						"TIME_SCALE",						CVar::INT,		"2" }, // lords 2 ddr 4

			{ &sv_energy_max,						"ENERGY_MAX",						CVar::INT,		"127" } ,
			{ &sv_strength_max,						"STRENGTH_MAX",						CVar::INT,		"100" } ,

			{ &sv_fear_scale,						"FEAR_SCALE",						CVar::INT,		"64" }, 
			{ &sv_courage_scale,					"COURAGE_SCALE",					CVar::INT,		"1" }, 
			{ &sv_energy_scale,						"ENERGY_SCALE",						CVar::INT,		"16" }, // lom  16 ddr 32
			{ &sv_despondent_scale,					"DESPONDENT_SCALE",					CVar::INT,		"32" }, 
			{ &sv_reckless_scale,					"RECKLESS_SCALE",					CVar::INT,		"32" },

			{ &sv_collate_battle_areas,				"COLLATE_BATTLE_AREAS",             CVar::YESNO,	"NO" },

			{ &sv_character_friend,					"CHARACTER_FRIEND",					CVar::IDT,		"CH_LUXOR" },
			{ &sv_character_foe,					"CHARACTER_FOE",					CVar::IDT,		"CH_DOOMDARK" },
			{ &sv_character_default,				"CHARACTER_DEFAULT",				CVar::IDARRAY,	"CH_LUXOR|CH_MORKIN|CH_CORLETH|CH_RORTHRON" },
			{ &sv_guidance,							"SEEK_MESSAGES",					CVar::IDARRAY,	"" },

            
            
		//
			{ &sv_characters,						"CHARACTERS",						CVar::INT,		"0" },
			{ &sv_routenodes,						"ROUTENODES",						CVar::INT,		"0" },
			{ &sv_strongholds,						"STRONGHOLDS",						CVar::INT,		"0" },
			{ &sv_regiments,						"REGIMENTS",						CVar::INT,		"0" },
			{ &sv_places,							"PLACES",							CVar::INT,		"0" },
			{ &sv_directions,						"DIRECTIONS",						CVar::INT,		"0" },
			{ &sv_objects,							"OBJECTS",							CVar::INT,		"0" },
			{ &sv_units,							"UNITS",							CVar::INT,		"0" },
			{ &sv_races,							"RACES",							CVar::INT,		"0" },
			{ &sv_genders,							"GENDERS",							CVar::INT,		"0" },
			{ &sv_terrains,							"TERRAINS",							CVar::INT,		"0" },
			{ &sv_areas,							"AREAS",							CVar::INT,		"0" },
			{ &sv_commands,							"COMMANDS",							CVar::INT,		"0" },
			{ &sv_missions,							"MISSIONS",							CVar::INT,		"0" },
			{ &sv_victories,						"VICTORIES",						CVar::INT,		"0" },
			{ &sv_attributes,						"ATTRIBUTES",						CVar::INT,		"0" },
			{ &sv_variables,						"VARIABLES",						CVar::INT,		"0" },
			{ &sv_days,								"DAYS",								CVar::INT,		"0" },
			{ &sv_strongholdadjuster,				"STRONGHOLD_ADJUSTER",				CVar::INT,		"0" },
			{ &sv_controlled_character,				"CONTROLLED_CHARACTER",				CVar::INT,		"0" },
            { &sv_energy_cannot_continue,           "ENERGY_CANNOT_CONTINUE",           CVar::INT,      "0" },
#if defined(_DDR_)
			{ &sv_object_powers,					"OBJECT_POWERS",					CVar::INT,		"0" },
			{ &sv_object_types,                     "OBJECT_TYPES",						CVar::INT,		"0" },
#endif
        };

		void sv_Initialise ( cvarreg_t* variable ) 
		{
			switch ( variable->type	) {

				case CVar::VNONE:
					break;
				case CVar::DOUBLE:
					break;
				case CVar::FLOAT:
					break;
				case CVar::PTR:
					break;
					
				case CVar::NUMBER:
					*((sv_num_t*)variable->memory) = (f32)chilli::lib::atol(variable->string);
					break;

				case CVar::YESNO:
					*((sv_bool_t*)variable->memory) = 0;

					if ( chilli::lib::stricmp(variable->string,"TRUE") == 0 
						|| chilli::lib::stricmp(variable->string,"YES") == 0 )
						*((sv_bool_t*)variable->memory) = 1;

					break;

				case CVar::INT:
					*((sv_int_t*)variable->memory) = chilli::lib::atol(variable->string);
					break;

				case CVar::STRING:
					*((sv_str_t*)variable->memory) = (char*)variable->string;
					break;

				case CVar::IDT:
				{
					mxid id = mx->EntityByName(variable->string)->SafeIdt();
					if ( id == IDT_NONE )
						id = mx->text->StringByName(variable->string);
					*((sv_mxid_t*)variable->memory) = id;
				}
					break;

				case CVar::IDARRAY:
					{
						sv_c_mxid_t* c = (sv_c_mxid_t*)variable->memory ;
						string s = variable->string ;
						c->Clear();
						LPSTR	token;
						int		count;
							//set up arrays
							count=0;
							token = strtok( s, "|" );
							while ( token!=NULL ) {
								if ( chilli::lib::strlen(token)==1 && token[0]=='-' )
									c->Add(NULL);
								else {
									mxid id = mx->EntityByName(token)->SafeIdt();
									if ( id == IDT_NONE )
										id = mx->text->StringByName(token);
									c->Add( id );
								}
								token=strtok(NULL,"|");
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
			}
		}

		void Init ( int pass )
		{
			for ( u32 ii=0; ii<NUMELE(var_array); ii++ ) {
				if ( pass == 1 ) {
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
					var_array[ii].string = var->string;
					sv_Initialise( &var_array[ii] );
				}
			}

			//
		}

		MXRESULT GetProperty(const string& name, variant& arg) 
		{
			arg.vType = variant::none ;
			arg.vInt = 0 ;

			cvarreg_t* var = NULL ;

			for ( u32 ii=0; ii<NUMELE(var_array); ii++ ) {
				if ( chilli::lib::stricmp( name, var_array[ii].name ) == 0 ) {
					var = &var_array[ii];
					break;
				}
			}

			if ( var != NULL ) {
			switch ( var->type	) {
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


		CVar::~CVar()
		{
		}

	}
	// namespace variables

}
// namespace tme



