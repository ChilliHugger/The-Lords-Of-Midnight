#ifndef _VARIABLES_H_INCLUDED_
#define _VARIABLES_H_INCLUDED_

#include "mxtypes.h"
#include "lomxtypes.h"

namespace tme {
	namespace variables {

		void Init(int pass);
		void Update ( void );
		void DeInit ( void );
		MXRESULT GetProperty(const string& name, variant& arg);

		typedef f32		sv_num_t;
		typedef s32		sv_int_t;
		typedef LPSTR	sv_str_t;
		typedef bool	sv_bool_t;

		typedef collections::c_mxid	sv_c_mxid_t;
		typedef mxid		sv_mxid_t;



		class CVar 
		{
		public:

			CVar();
			virtual ~CVar();

			enum type {
				VNONE=0,
				NUMBER,
				YESNO,
				STRING,
				IDARRAY,
				IDT,
				INT,
				DOUBLE,
				FLOAT,
				PTR,
			};

			s32& operator = ( s32 value );
			f64& operator = ( f64 value );
			char*& operator = ( char* value );
			void*& operator = ( void* value );

			operator s32() const ;
			operator f64() const ;
			operator char*() const ;
			operator void*() const ;

		public:
			union {
				s32				vInt;
				f64				vDouble;
				f32				vFloat;
				char*			vString;
				void*			vPtr;
			};
			type	vType;
		};

		typedef struct cvarreg_t {
			void*		memory;
			LPCSTR		name;
			CVar::type	type;
			LPCSTR		string;
		} cvarreg_t ;



		extern sv_num_t	sv_battle_default_energy_drain ;
		extern sv_num_t	sv_battle_default_char_energy_drain ;
		extern sv_num_t	sv_battle_success_regiment_warriors ;
		extern sv_num_t	sv_battle_success_regiment_riders ;
		extern sv_num_t	sv_character_default_energy_inc ;
		extern sv_num_t	sv_character_recruit_amount ;
		extern sv_num_t	sv_character_guard_amount ;
		extern sv_num_t	sv_character_max_riders ;
		extern sv_num_t	sv_character_max_warriors ;
		extern sv_num_t	sv_character_max_energy ;
		extern sv_num_t	sv_warriors_max_energy ;
		extern sv_num_t	sv_warriors_default_energy_inc ;
		extern sv_num_t	sv_riders_default_energy_inc ;
		extern sv_num_t	sv_riders_max_energy ;
		extern sv_num_t	sv_object_energy_shelter ;
		extern sv_num_t	sv_object_energy_shadowsofdeath ;
		extern sv_num_t	sv_object_energy_watersoflife ;
		extern sv_num_t	sv_lookforwarddistance ;
		extern sv_num_t	sv_regiment_default_moves ;
		extern sv_num_t	sv_regiment_stop_move ;
		extern sv_num_t	sv_stronghold_success_none ;
		extern sv_num_t	sv_stronghold_default_empty ;
		extern sv_num_t	sv_stronghold_default_min ;
		extern sv_num_t	sv_stronghold_default_max ;
		extern sv_bool_t	sv_stronghold_default_allow_respawn ;
		extern sv_num_t	sv_stronghold_default_respawn_amount ;
		extern sv_num_t	sv_success_fey_on_horse_adjuster	;
		extern sv_num_t	sv_success_base_level ;
		extern sv_num_t	sv_success_riders_energy_mountain ;
		extern sv_num_t	sv_success_riders_energy_other ;
		extern sv_bool_t	sv_always_attempt_recruit ;

		extern sv_str_t	sv_map_file ;
		extern sv_num_t	sv_map_width ;
		extern sv_num_t	sv_map_height ;
		extern sv_bool_t	sv_auto_unhide ;
		extern sv_bool_t	sv_auto_seek ;
		extern sv_num_t	sv_character_default_memory_age ;

		extern sv_int_t	sv_time_night ;
		extern sv_int_t	sv_time_dawn ;
		extern sv_int_t	sv_time_scale ;

		extern sv_int_t	sv_despondent_scale ;
		extern sv_int_t	sv_fear_scale ;
		extern sv_int_t	sv_courage_scale ;
		extern sv_int_t	sv_energy_scale ;
		extern sv_int_t	sv_reckless_scale ;
		
        extern sv_bool_t	sv_collate_battle_areas ;
        
		extern sv_int_t	sv_energy_max ;
		extern sv_int_t	sv_strength_max ;

		extern sv_mxid_t	sv_character_friend;
		extern sv_mxid_t	sv_character_foe;

		extern sv_c_mxid_t	sv_character_default;
		extern sv_c_mxid_t	sv_guidance;

		extern sv_int_t	sv_characters ;
		extern sv_int_t	sv_routenodes ;
		extern sv_int_t	sv_strongholds ;
		extern sv_int_t	sv_regiments ;
		extern sv_int_t	sv_places ;
		extern sv_int_t	sv_directions ;
		extern sv_int_t	sv_objects ;
		extern sv_int_t	sv_units ;
		extern sv_int_t	sv_races ;
		extern sv_int_t	sv_genders ;
		extern sv_int_t	sv_terrains ;
		extern sv_int_t	sv_areas ;
		extern sv_int_t	sv_commands ;
		extern sv_int_t	sv_missions ;
		extern sv_int_t	sv_victories ;
		extern sv_int_t	sv_days ;
		extern sv_int_t	sv_attributes ;
		extern sv_int_t	sv_variables ;
		extern sv_int_t	sv_strongholdadjuster ;
		extern sv_int_t	sv_controlled_character ;
        extern sv_int_t sv_energy_cannot_continue;

#if defined(_DDR_)
		extern sv_int_t	sv_object_powers ;
		extern sv_int_t	sv_object_types ;
#endif
		
		// Enemy armies to not block our movement
		extern sv_bool_t	sv_cheat_armies_noblock ;
		// Nasties do not need fighting
		extern sv_bool_t	sv_cheat_nasties_noblock ;
		// movement costs nothing
		extern sv_bool_t	sv_cheat_movement_free ;
		// movement cost just 1 unit
		extern sv_bool_t	sv_cheat_movement_cheap ;
		// commands take no time
		extern sv_bool_t	sv_cheat_commands_free ;
		
		
	}

#define REGIMENT_STOP_MOVE					0
#define STRONGHOLD_SUCCESS_NONE				0



}


using namespace tme::variables ;

		inline CVar::CVar()
			{ vInt=0; vType=CVar::VNONE; }
		//virtual CVar::~CVar();

		inline s32& CVar::operator = ( s32 value )
			{ vInt = value ; vType = CVar::INT; return vInt; }
		inline f64& CVar::operator = ( f64 value )
			{ vDouble = value ; vType = CVar::DOUBLE; return vDouble; }
		inline char*& CVar::operator = ( char* value )
			{ vString = value ; vType = CVar::STRING; return vString; }
		inline void*& CVar::operator = ( void* value )
			{ vPtr = value ; vType = CVar::PTR; return vPtr; }

		inline CVar::operator s32() const
			{ return vInt; }
		inline CVar::operator f64() const
			{ return vDouble; }
		inline CVar::operator char*() const
			{ return vString; }
		inline CVar::operator void*() const
			{ return vPtr; }



#endif //_VARIABLES_H_INCLUDED_
