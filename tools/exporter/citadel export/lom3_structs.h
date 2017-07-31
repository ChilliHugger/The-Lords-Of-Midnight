//-------------------------------------------------------------------------
//			LORDS OF MIDNIGHT III
//
//			    THE CITADEL
//-------------------------------------------------------------------------
// (C) David Ollman & James Shaw 1995.
// C/C++ implementation Chris Wild
//-------------------------------------------------------------------------

#ifndef _LOM3_H_INCLUDED_
#define _LOM3_H_INCLUDED_

#pragma pack ( push, _LOM3_, 1 )

//		    ---------- BIT FLAG EQUATES ----------

//*** OBJ_CHAR_FLAGS ***
enum OBJ_CHAR_FLAGS {
	recruit_bit				=	1,		//set if recruited
	knowledge_bit			=	2,		//set if known
	dead_bit				=	4,		//set if dead
	friendly_bit			=	8,		//set if friendly
	rescue_bit				=	16,		//set if rescue required
	detail_bit				=	32,		//set if intimate details known
	interest_bit			=	64,		//set if interested in characters actions
	rest_bit				=	128,	//set if object at rest
	visible_bit				=	256,	//set if object was visible last movement
	sink_bit				=	512,	//set if object is sinking below water
	major_bit				=	1024,	//set if major character
	accel_bit				=	2048,	//set if catching up with party/unit member
	hidden_bit				=	4096,	//set if object hidden from view in landscape
	control_bit				=	8192,	//set if object can be controlled
	location_bit			=	16384,	//set if object location known
	watch_bit				=	32768,	//set if object watchable
};

//*** OBJ_FLAGS ***
enum OBJ_FLAGS {
	height_bit				=	1,		//set if object has relative height
	static_bit				=	2,		//set if object does not move
	obstacle_bit			=	4,		//set if object should be avoided
	generated_bit			=	8,		//set if object was generated not plonked
	anim_bit				=	16,		//set if object to be animated
	process_bit				=	32,		//set if object already processed this turn
	corridor_bit			=	64,		//set if character enterring/exitting building
	en_garde_bit			=	128,	//set if combat to be initialised
	combat_bit				=	256,	//set if character arrived at combat position
	crowd_bit				=	512,	//set if character crowding around combat

	doorway_bit				=	512,	//set if shelter doorway is being approached : crowd bit not used for shelters!

	talking_bit				=	1024,	//set if character involved in a conversation
	initiate_bit			=	2048,	//set if character starts a conversation
	inside_bit				=	4096,	//set if character viewed inside a building

	seize_bit				=	8192,	//set if character involved in a seige

	available_bit			=	16384,	//set if character not involved in a quest
	female_bit				=	32768,	//set if character female
};

//*** OBJ_FLAGS2 *** JS 12/2/95
enum OBJ_FLAGS2 {
	autoq_bit				=	1,		//set if character is auto questing
	really_inside_bit		=	2,		//set if character is really inside & not just resting
										//=> inside_bit is set
	shelter_stop_bit		=	4,		//set if using shelter code only to move player inside
	meet_autoq_bit			=	8,		//set if character only on a meet quest
	seize_ready_bit			=	16,		//set if invisible seige has been setup and is ready to go!
	no_process_bit			=	32,		//set if this character not to be processed
	near_collapse_mess_bit	=	64,		//set if a message has been posted for this character's near collapse
	collapse_mess_bit		=	128,	//set if a message has been posted for this character's collapse
	seek_shelter_bit		=	256,	//set if a message has been posted for this character seeking shelter
	wait_party_bit			=	512,	//set if a message has been posted for this character waiting for his party
	was_hostage_bit			=	1024,	//set if was a hostage
};

//---------------------- Weapons Equates --------------
enum weapons_t {
	w_none					=	0,
	w_sword					=	1,
	w_axe					=	2,
};

//*****************************************************************************

//		---------- COMMON DATA STRUCTURE ----------

typedef struct common_t 
{
	s32	obj_prev_link ;					// Previous link address (0=first object in list)
	s32	obj_next_link ;					// Next link address (0=last object in list)

	u16	obj_num ;						// Object ID number in relevant list (cast_list, etc..)
	u16	obj_type ;						// Object type (person/building/etc...)

	u16	obj_char_flags ;				// Character based flags (alive/sleeping/rescued)

	u16	obj_flags ;						// Object based flags (relative height)

	u32	obj_x ;							// Absolute X coordinate (16:16 format)
	u32	obj_y ;							// Absolute Y coordinate (16:16 format)
	u16	obj_z ;							// Height adjust

	u16	obj_dir ;						// Direction

	u16	obj_sprite ;					// Sprite number (Landscape representation)
	u16	obj_cladding ;					// Sprite graphic cladding
	u16	obj_print ;						// Sprite printing routine

	u32	obj_x_scale ;					// X scaling factor (16:16 format)
	u32	obj_y_scale ;					// Y scaling factor (16:16 format)

	u8	obj_x_size ;					// Collision detection X range
	u8	obj_y_size ;					// Collision detection Y range
	u8	obj_z_size ;					// Collision detection Z range

	s8	obj_cam_off ;					// Vertical camera offset (+/- 127)
} common_t ;

//*****************************************************************************

//		---------- FEATURE DATA STRUCTURE ----------
//
//rs_set (obj_cam_off+1)
//rs common_len,0		// Length of common block

typedef struct feature_t : public common_t  
{
} feature_t ;

//*****************************************************************************

#define LOM3_MAX_RACE	16


typedef struct graphic_t {
	u8	obj_shield ;					// Heraldic shield type
	u8	obj_port ;						// Portrait number
} graphic_t ;

typedef struct building_t : public common_t  
{
	graphic_t	graphic;

	u16	obj_race_size[LOM3_MAX_RACE] ;	// Warrior count (1 word per race)
	
	u16	obj_owner ;

	u16	obj_land_ruler ;				//current ruler
	u8	obj_land_war_state ;			//state of war
	u8	obj_land_realm ;				//which realm land is in
	u16	obj_land_owner ;				//original owner
	u16	obj_land_enemies ;				//enemy flags

	u16	obj_cit_link ;					//citadel linked list

} building_t ; 



//		---------- CASTLE DATA STRUCTURE ----------

typedef struct castle_t : public building_t  
{
} castle_t ;

//*****************************************************************************

//		---------- CITY DATA STRUCTURE ----------
typedef struct city_t : public building_t  
{
} city_t ;

//*****************************************************************************

//		---------- CITADEL DATA STRUCTURE ----------

typedef struct citadel_t : public building_t  
{
	// obj_cit_link = citadel occupants linked list
} citadel_t ;

//*****************************************************************************

typedef struct mobile_obj_t : public common_t 
{
	graphic_t	graphic;

	u8	obj_movetype ;					// Movement type (walking/flying/floating)
	s8	obj_movemode ;					// Movement mode (search/evade/track/auto/pause/etc...)

	u16	obj_speed ;						// Speed of movement
	u8	obj_int_speed ;					// Integer speed of movement

	u8	obj_max_turn ;					// Maximum rate of turn

	s16	obj_targ_x ;					// Target X coordinate
	s16	obj_targ_y ;					// Target Y coordinate

	s16	obj_tsq_x ;						// Current target X square on 128x1 map
	s16	obj_tsq_y ;						// Current target Y square on 128x1 map

	u16	obj_guide_type ;				// Current guide control routine
	s16	obj_route ;						// Current index to object route

	s16	obj_link_type ;					// Type of linked object
	s16	obj_link_num ;					// Linked objects list index

	u16	obj_move_cnt ;					// Invisible movement counter
	u32	obj_move_x ;					// Invisible X movement
	u32	obj_move_y ;					// Invisible Y movement

	u8	obj_anim_no ;					// Current animation number
	u8	obj_frame_no ;					// Current animation frame number

	u32 obj_energy ;					// Overall energy level
	u16	obj_max_limit ;					// Maximum energy limit
	u16	obj_mid_limit ;					// Medium energy limit

	u16	obj_rest ;						// Maximum rest time after collapse
	u16	obj_rest_count ;				// Collapse rest counter

	u16	obj_timeout ;					// Party delay counter

	u16	obj_opponent ;					// Combat opponent
	u32	obj_strength ;					// Combat strength available
	u16	obj_power ;						// Combat power level
	u16	obj_limit ;						// Combat power level limit

	u16	obj_state ;						// Type of direction finding
	u16	obj_wander_dir ;				// Random movement direction

} mobile_obj_t ;

typedef struct quest_t {
	u16	obj_quest ;						// Type of quest
	u16	obj_qstate ;					// Status of quest
	u16	obj_qdat1 ;						// Object list type, destination X coord
	u16	obj_qdat2 ;						// Object number, destination Y coord
	u16	obj_qdat3 ;						// X Position to return to after sub-quest
	u16	obj_qdat4 ;						// Y Position to return to after sub-quest
} quest_t ;


typedef struct party_obj_t : public mobile_obj_t  
{
	quest_t	quest;

	s16	obj_prev_party ;				// Link to previous party member (-1 = party leader)
	s16	obj_next_party ;				// Link to next party member (-1 = last party member)

} party_obj_t ;


//		---------- WILD BEAST DATA STRUCTURE ----------

typedef struct beast_t : public mobile_obj_t  
{
	u8	obj_anim1 ;						// anim control vectors
	u8	obj_anim2 ;
	u8	obj_anim3 ;
	u8	obj_anim4 ;

} beast_t ;

//*****************************************************************************

//		---------- CITIZEN DATA STRUCTURE ----------

typedef struct citizen_t : public party_obj_t    
{
} citizen_t ;

//*****************************************************************************

//		---------- WARRIOR DATA STRUCTURE ----------

typedef struct warrior_t : public party_obj_t  
{
} warrior_t ;
//*****************************************************************************

typedef struct shelter_t {
	s16	obj_rstate ;					// Current sheltering status
	u16	obj_shelter_x ;					// Shelter X coordinate
	u16	obj_shelter_y ;					// Shelter Y coordinate
	u16	obj_shelter_dir ;				// Orientation of shelter graphic
	s16	obj_shelter_num ;				// Building index number
	u16	obj_shelter_typ ;				// Type of building to be used as shelter
} shelter_t ;


//		---------- ARMY DATA STRUCTURE ----------

typedef struct base_army_t : public party_obj_t
{
	shelter_t	shelter;

	u8	obj_lstblk_x ;					// Last link-map scan origin X block
	u8	obj_lstblk_y ;					// Last link-map scan origin Y block

	u16	obj_rank ;						// Leadership ranking

	u16	obj_army_race ;					// Race of army (0-15)
	u16	obj_army_size ;					// Current army strength
	u16	obj_army_lost ;					// Number of army lost in battle
	u16	obj_army_kill ;					// Number of enemy killed in battle
	u16	obj_army_flag ;					// Recruitable race flags
	
	u8	obj_start_realm ;				// Index value of 1st associated army unit
	u8	obj_start_land ;				// Index value of 2nd associated army unit

	u16	obj_sleep_time ;				// Time of the day to sleep (0-1439)
	u16	obj_wake_time ;					// Time of the day to wake (0-1439)

	u8	obj_weapons ;					// Weapons carried
	u8	obj_weap2 ;						// Weapon details
	s16	obj_weap_link ;					// List # of weapon

	u16	obj_find_status ;				// Current hierarchy level of find quest
	s8	obj_qtype1 ;					// What type of info is in qdat1,2
    s8	obj_qtype2 ;					// What type of info is in qdat3,4

	u16	obj_time ;						// time object finished quest
	u16	obj_wait ;						// hours to wait once finished

	u16	obj_flags2 ;					// more flags


} base_army_t ;

typedef struct army_t : public base_army_t
{
    //rs obj_shelt_x ;					// position of player before sheltering on time races on
    u16	obj_shelt_y ;
} army_t ;


//*****************************************************************************

//		---------- CHARACTER DATA STRUCTURE ----------

typedef struct character_t : public base_army_t
{
    u16 obj_next_rlink	; // next character# in building shelterer linked list
    
	u16	obj_boat_state ;				// state of boat searching on quest

	s16	obj_purpose ;					// general purpose
	u16	obj_reaction ;					// current reaction

	u8	obj_clothing[3];					// character cladding
	s8	obj_allegiance ;				// (was pad) who this char is allied to (realm)

	u8	obj_curr_land ;					// land position for quest eval
	u8	obj_curr_realm ;				// realm position for quest eval

	u32	obj_qualities1 ;				// character quality flags
	u32	obj_qualities2 ;				// character quality flags

	u32	obj_xi ;						// interior x/y position
	u32	obj_yi ;						//
	u16	obj_lives ;						// hidden life count
	u32	obj_start_strength ;			// start strength
	u16	obj_start_armies ;				// start armies

	//rs obj_null,0						// Viewing angle (up/down)
	//rs obj_null,0						// Name
	//rs obj_null,0						// Title
	//rs obj_null,0						// Narrative details
	//rs obj_null,0						// Exhaustion/Energy level
	//rs obj_null,0						// Personality (ie Chicken factor - run away from fight)
	//rs obj_null,0						// Means of transport (foot/horse/dragon/ship)
	//rs obj_null,0						// Cash
	//rs obj_null,0						// Objects carried
	//rs obj_null,0						// Persuasion difficulty
	//rs obj_null,0						// Morale
	//rs obj_null,0						// List of recruited citizens
	//rs obj_null,0						// List of recruited warriors
	//rs obj_null,0						// Current army losses
	//rs obj_null,0						// Help info/weapons/objects/money/right of passage
	//rs obj_null,0						// Game time

} character_t ;

//*****************************************************************************

//		---------- OBJECT DATA STRUCTURE ----------

typedef struct object_t : public common_t  
{
	graphic_t	graphic;

	s16	obj_weapon_owner ;				// List # of owner
	u8	obj_weapon_type ;				// Weapon type
	u8	obj_weapon_details ;			// extra weapon info

} object_t ;
//*****************************************************************************

// realm structure
typedef struct realm_t {
    u16	realm_ruler;					//current ruler
    u8	realm_war_state;				//state of war
    u8	realm_state;					//state of realm
    u16	realm_owner;					//original owner
    u16	realm_enemies;					//enemy flags
    u16	realm_hostage;					//character number of hostage
} realm_t ;

#define MY_HOMELAND		0

// purposes
enum purpose {
	NO_PURPOSE					= -1,
	DEFEND_HOMELAND				= 0,
	DARK_FEY					= 1,
	RANDOMLY_WANDER				= 2,
	BE_A_HOSTAGE				= 3,
} ;

// reactions
enum reactions {
	RETURN_HOME				=	0,
	STAND_FIRM,
	TAKE_BACK_STRONGHOLD,
	ATTACK_ENEMY,
	HELP_NEIGHBOUR,
	RETREAT,
	GATHER_STRENGTH,
	COUNTER_THREAT,
	LEND_SERVICE,
};

// states of war (for lands)
enum ls_war_states {
	LS_AT_PEACE					=	0,
	LS_TROUBLED,
	LS_THREATENED,
	LS_MENACED,
	LS_IN_DANGER,
	LS_IN_PERIL,
	LS_UNDER_ATTACK,
	LS_SUBJUGATED,
};

// states of war (for realms)
enum rs_war_states {
	RW_AT_PEACE				=	0,
	RW_THREATENED,
	RW_UNDER_ATTACK,
	RW_SUBJUGATED,
};

// state of realm
enum rs_states {
	RS_UNFETTERED				=	0,
	RS_AT_RANSOM,
	RS_BLOOD_FEUD,
	RS_INTERREGNUM,
};

// quests
enum quests {
	QRECRUIT					=	0,
	QJOIN						=	1,
	QKILL						=	2,
	QRESCUE						=	3,
	QFOLLOW						=	4,
	QGOTO						=	5,
	QGUARD						=	6,
	QSEIZE						=	7,
	QFIN						=	8,
	QTAKE						=	9,
	QDESTROY					=	10,
	QREST						=	11,
	QUNDER						=	12,
};



#pragma pack ( pop, _LOM3_ )

#endif //_LOM3_H_INCLUDED_
