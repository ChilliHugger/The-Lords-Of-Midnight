#ifndef _LOMXTYPES_H_INCLUDED_
#define _LOMXTYPES_H_INCLUDED_

#include "library.h"


//#define _DDR_
#define _LOM_


/* these are the various enums
 * that define variable types used within 
 * the midnight engine
 * these are just the default as used in the LOM and DDR scenarios
 */

//#if _MSC_VER >= 1300
//#define _DEFINE_ENUMS_
//#endif 

//#define _TME_DEMO_MODE_
//#define _TEST_WINLOSE_CONDITIONS_

// enum control
#define DECLARE_ENUM(x)		namespace x { enum x##_t
#define END_ENUM(x)			}; }


//#ifdef _DEFINE_ENUMS_
#define MXRESULT	mxresult_t
//#else
//#define MXRESULT	int
//#endif

//#ifdef _DEFINE_ENUMS_
//namespace tme {
//	namespace enums {
//#endif

		enum mxresult_t {
			MX_OK					=	1,
			MX_FAILED				=	0,
			MX_UNKNOWN				=	-1,
			MX_NULL_STRUCT_PTR		=	-2,
			MX_INVALID_INFO_TYPE	=	-3,
			MX_INVALID_INFO_SIZE	=	-4,
			MX_INVALID_IDTYPE		=	-5,
			MX_INVALID_ID			=	-6,
			MX_NOT_IMPLEMENTED		=	-7,
			MX_INVALID_ARGUMENT_COUNT		=	-8,
			MX_INVALID_ARGUMENT		=	-9,

			MX_INCORRECT_VERSION	=	-10,
			MX_UNKNOWN_FILE			=	-11,
		};

		enum mxgender_t {
			GD_NONE,
			GD_MALE,
			GD_FEMALE,
		};

		typedef u32 mxtime_t ;

		enum mxrace_t {
			RA_NONE=0,
			RA_DOOMGUARD,
			RA_FREE,
			RA_FEY,
			RA_TARG,
			RA_WISE,
			RA_MORKIN,
			RA_SKULKRIN,
			RA_DRAGON,
			RA_TROLL,

#if defined(_LOM_)
            RA_MIDWINTER,
#endif
            
			// DDR
			RA_MOONPRINCE,
			RA_BARBARIAN,
			RA_ICELORD,
			RA_GIANT,
			RA_HEARTSTEALER,
			RA_DWARF,
			RA_TARITHEL,
			RA_DOOMDARK,
			//

#if defined(_DDR_)
            RA_MIDWINTER,
#endif
    
            
        };

		enum mxunit_t {
			UT_NONE=0,
			UT_WARRIORS=1,
			UT_RIDERS,
		};

		enum mxdir_t {
			DR_NORTH,
			DR_NORTHEAST,
			DR_EAST,
			DR_SOUTHEAST,
			DR_SOUTH,
			DR_SOUTHWEST,
			DR_WEST,
			DR_NORTHWEST,
			DR_NONE=255,
		};

		enum mxthing_t {
			OB_NONE=0,
			OB_WOLVES,
			OB_DRAGONS,
			OB_ICETROLLS,
			OB_SKULKRIN,
			OB_WILDHORSES,
			OB_SHELTER,
			OB_GUIDANCE,
            
			OB_SHADOWSOFDEATH,
			OB_WATERSOFLIFE,
			OB_HANDOFDARK,
			OB_CUPOFDREAMS,
			OB_WOLFSLAYER,
			OB_DRAGONSLAYER,
			OB_ICECROWN,
			OB_MOONRING,

			// DDR
			OB_CLAWS,
			OB_FLAMES,
			OB_THORNS,
			OB_BLOOD,
			OB_LANGUOR,
			OB_SPRINGS,
            
            // other
#if defined(_LOM_)
            OB_EYEOFTHEMOON,
#endif
            
#if defined(_DDR_)
            //
            OB_CROWN_VARENAND=22,
            OB_CROWN_CARUDRIUM,
            OB_SPELL_THIGROR,
            OB_RUNES_FINORN,
            OB_CROWN_IMIRIEL
#endif
            
		};

#define MAX_DESPONDENCY                 255
#define MAX_ENERGY                      255
#define DESPONDENCY_BOOST_KILLED_FOE    32
#define SOLDIERS_RECRUIT_GAP            20
#define MIN_AI_MOVEMENT_ENERGY          96
#define RESTING_ENERGY_BOOST            32
#define DAWN_ENERGY_BOOST               20
#define DAWN_ENERGY_BOOST_AI            10

		enum mxterrain_t {
			// lom
			TN_PLAINS		=	0,
			TN_CITADEL		=	1,
			TN_FOREST		=	2,
			TN_HENGE		=	3,
			TN_TOWER		=	4,
			TN_VILLAGE		=	5,
			TN_DOWNS		=	6,
			TN_KEEP			=	7,
			TN_SNOWHALL		=	8,
			TN_LAKE			=	9,
			TN_FROZENWASTE	=	10,
			TN_RUIN			=	11,
			TN_LITH			=	12,
			TN_CAVERN		=	13,
			TN_MOUNTAIN		=	14,
            TN_UNUSED1      =   15,
            
			// ddr
			TN_PLAINS2		=	16,
			TN_MOUNTAIN2	=	17,
			TN_FOREST2		=	18,
			TN_HILLS		=	19,
            
			TN_GATE			=	20, // tunnel exit/entrance
			TN_TEMPLE		=	21, // tunnel exit/entrance
			TN_PIT			=	22, // tunnel exit/entrance
			TN_PALACE		=	23,// tunnel exit/entrance
            
			TN_FORTRESS		=	24,
			TN_HALL			=	25,
			TN_HUT			=	26,
			TN_WATCHTOWER	=	27,
			TN_CITY			=	28,
			TN_FOUNTAIN		=	29,
			TN_STONES		=	30,
			TN_ICYWASTE     =	31,

//			TERRAIN_VARIANT(TN_MOUNTAINS,1),
//			TERRAIN_VARIANT(TN_FOREST,1),
//			TERRAIN_VARIANT(TN_TOWER,1),

//			TN_ARMY,		// remove
//			TN_MIST,		// remove


			TN_NONE=255,

		};

		enum mxbattlestatus_t {
			BA_NONE,
			BA_CONTINUES,
			BA_FRIEND,
			BA_FOE,
		};

		enum mxwait_t {
			WM_NONE,
			WM_OVERNIGHT,
			WM_FOREVER,
		};

		enum command_t {
			CMD_NONE,
			CMD_MOVE,
			CMD_APPROACH,
			CMD_RECRUITMEN,
			CMD_POSTMEN,
			CMD_ATTACK,
			CMD_FIGHT,
			CMD_REST,
			CMD_HIDE,
			CMD_UNHIDE,
			CMD_SEEK,
			CMD_TAKEOBJECT,
			CMD_DROPOBJECT,
			CMD_WAIT,
		};

		enum mxcommanding_t {
			CMDG_ALL,
			CMDG_AVAILABLE,
			CMDG_WAITING,
			CMDG_CURRENTLOC,
			CMDG_LOYAL,
		};

		enum mxarmytype_t {
			AT_NONE,
			AT_CHARACTER,
			AT_REGIMENT,
			AT_STRONGHOLD,
		};

		enum mxorders_t {
			OD_NONE						=	0,
			// regiment
			OD_GOTO						=	1,
			OD_WANDER					=	2,
			OD_FOLLOW					=	3,
			OD_ROUTE					=	4,
            OD_DELAYED_WANDER           =   5,
			OD_HOLD						=	6,
#if defined(_DDR_)
            // character
            OD_FOLLOW_LIEGE             =   7,  // 0
            OD_FOLLOW_FOE               =   8,  // 1
            OD_FIND_OBJECT              =   9,  // 2
            OD_HOME                     =   10, // 3
#endif
		};

		enum m_objective_t	{
			MO_NONE						=	0,
			MO_MINOR					=	1,
			MO_MAJOR					=	2,
			MO_FLAG						=	3,
		};

		enum m_condition_t {
			MC_NONE						=	0,
			MC_STRONGHOLD_TAKEN			=	1,
			MC_DESTROY_OBJECT			=	2,
			MC_CHARACTER_DEAD			=	3,
			MC_MISSION_COMPLETE			=	4,
			MC_STRONGHOLD_FALLEN		=	5,
			MC_SAME_LOCATION			=	6,
		};

		enum m_action_t {
			MA_NONE						=	0,
			MA_DISABLE					=	1,
			MA_ENABLE					=	2,
		};

        enum m_gameover_t {
            MG_NONE                     =   0,
            MG_WIN                      =   1,
            MG_LOSE                     =   2
        };

#if defined(_DDR_)
        enum mxobjpower_t {
            OP_NONE                     =   0,
            OP_PERSUASION               =   1,
            OP_PROTECTION               =   2,
            OP_BATTLE                   =   3,
            OP_SWIFTNESS                =   4,
            OP_CONTROL                  =   5, // mooon ring
            OP_FEAR                     =   6, // ice crown
            
            OP_ATTACK,
            OP_ATTACK_DDR,
            OP_SHELTER,
            OP_SHELTER_DDR,
            OP_GUIDANCE,
            OP_GUIDANCE_DDR,
            OP_NO_ENERGY,
            OP_FULL_ENERGY,
            OP_NIGHT,
            OP_DAWN,
            OP_HORSE,
            OP_FULL_DESPONDANCE,
            OP_NO_DESPONDANCE,
            OP_FULL_TIREDNESS,
            OP_NO_TIREDNESS,

        };

        enum mxobjtype_t {
            OT_NONE                     =   0,
            OT_CROWN,
            OT_RUNES,
            OT_BOW,
            OT_SPEAR,
            OT_SWORD,
            OT_AXE,
            OT_SPELL,
            OT_HAMMER,
            OT_RING,
        };

#endif

		enum infotype_t {
			INFO_NONE					=	0,
			INFO_AREAINFO				=	1,
			INFO_ATTRIBUTEINFO			=	2,
			INFO_DIRECTIONINFO			=	3,
			INFO_GENDERINFO				=	4,
			INFO_OBJECT					=	5,
			INFO_RACEINFO				=	6,
			INFO_TERRAININFO			=	7,
			INFO_UNITINFO				=	8,
			INFO_ARMY					=	9,
			INFO_CHARACTER				=	10,
			INFO_LOCATIONINFO			=	11,
			INFO_REGIMENT				=	12,
			INFO_ROUTENODE				=	13,
			INFO_STRONGHOLD				=	14,
			//INFO_OBJECT					=	15,
			INFO_UNIT					=	16,
			INFO_ARMYTOTAL				=	17,
			INFO_MAPLOCATION			=	18,
			INFO_STRING					=	19,
			INFO_MEMORYITEM				=	20,
			INFO_COMMANDINFO			=	21,
			INFO_PLACE					=	22,
			// skip 23
			// skip 24
            // skip 25
#if defined(_DDR_)
            INFO_OBJECT_POWER            =   26,
            INFO_OBJECT_TYPE             =   27,
#endif
};

		enum id_type_t {
			IDT_NONE					=	0,
			IDT_AREAINFO				=	1,
			IDT_ATTRIBUTEINFO			=	2,
			IDT_DIRECTIONINFO			=	3,
			IDT_GENDERINFO				=	4,
			IDT_OBJECT					=	5,
			IDT_RACEINFO				=	6,
			IDT_TERRAININFO				=	7,
			IDT_UNITINFO				=	8,
			IDT_ARMY					=	9,
			IDT_CHARACTER				=	10,
			IDT_LOCATIONINFO			=	11,
			IDT_REGIMENT				=	12,
			IDT_ROUTENODE				=	13,
			IDT_STRONGHOLD				=	14,
			//IDT_OBJECT					=	15,
			IDT_UNIT					=	16,
			IDT_ARMYTOTAL				=	17,
			IDT_LOCATION				=	18,
			IDT_STRING					=	19,
			IDT_MEMORYITEM				=	20,
			IDT_COMMANDINFO				=	21,
			IDT_PLACE					=	22,
			IDT_MISSION					=	23,
			IDT_VICTORY					=	24,
            IDT_MAPLOCATION             =   25,
            
#if defined(_DDR_)
            IDT_OBJECT_POWER            =   26,
            IDT_OBJECT_TYPE             =   27,
#endif
		};

		enum SYSTEMSTRING {
			SS_ADVERBS,             //0
			SS_NUMBERS,
			SS_ENERGYTOKENS,
			SS_FEARTOKENS,
			SS_COURAGETOKENS,

			SS_DEFAULT_TERRAIN,
			SS_LOCATION_DOMAIN,
			SS_LOCATION_NOTDOMAIN,

			SS_DAWN,
			SS_NIGHT,
			SS_THINGATLOCATION,     // 10

			SS_RECRUITMEN,
			SS_POSTMEN,
			
			SS_ITISNIGHT,
			SS_ITISDAWN,
			SS_LESSTHAN,
			SS_HOURSOFDAY,

			SS_CHARACTER_ENERGY,
			SS_CHARACTER_FEAR,
			SS_CHARACTER_COURAGE,

			SS_CARRYNOTHING,
			SS_CARRYING,            // 20

			SS_KILLED_OBJECT,
			SS_KILLED_BATTLE,

			SS_HIDDEN,
			SS_MUSTFIGHT,

			SS_MESSAGE1,
			SS_MESSAGE2,
			SS_MESSAGE3,
			SS_MESSAGE4,

			SS_MESSAGE5,
			SS_MESSAGE6,            // 30

			SS_SEEK,
			SS_FIGHT,

			SS_GUIDANCE1,
			SS_GUIDANCE2,
			SS_SEEK_NOTHING,

			SS_BATTLE1,
			SS_BATTLE_NONE,
			SS_BATTLE_WARRIORS,
			SS_BATTLE_RIDERS,
			SS_BATTLE_WARRIORS_RIDERS,  // 40

			SS_BATTLE_CHARACTER_SLEW,
			SS_BATTLE_WARRIORS_SLEW,
			SS_BATTLE_RIDERS_SLEW,

			SS_BATTLE_VICTORY_FREE,
			SS_BATTLE_VICTORY_ENEMY,
			SS_BATTLE_VICTORY_NONE,

			SS_ARMY_CHARACTER_COMMANDS,
			SS_ARMY_CHARACTER_WARRIORS,
			SS_ARMY_CHARACTER_RIDERS,
			
			SS_ARMY_NONE,               // 50
			SS_ARMY_WARRIORS,
			SS_ARMY_RIDERS,
			SS_ARMY_WARRIORS_RIDERS,

			SS_TERRAIN_PLURAL,
			SS_TERRAIN_SINGLE,

			SS_AREA_PREFIXED,

			SS_DEFEAT1,
			SS_DEFEAT2,
			SS_DEFEAT3,

			SS_VICTORY1,                // 60
			SS_VICTORY2,
			SS_VICTORY3,
			SS_VICTORY4,
			SS_VICTORY5,
			SS_VICTORY6,

			SS_ZEROTOKENS,
			SS_WARRIORS_RIDERS,

			SS_MESSAGE7,

			SS_BATTLE2,
			SS_BATTLE3,                 // 70

			SS_PLURALTOKENS,
			SS_STRONGHOLD1,
			SS_STRONGHOLD2,
			SS_STRONGHOLD3,

			SS_FIGHT2,
			SS_FIGHT3,
			SS_FIGHT4,
            
			SS_PREFIX_HERE,
			SS_PREFIX_AHEAD,
			SS_PREFIX_DIRECTION,        // 80

			SS_SEEK_MSG1,
			SS_SEEK_MSG2,
			SS_SEEK_MSG3,
			SS_SEEK_MSG4,
		//	SS_SEEK_MSG5,
		//	SS_SEEK_MSG6,
		//	SS_SEEK_MSG7,
		//	SS_SEEK_MSG8,
		//	SS_SEEK_MSG9,
		//	SS_SEEK_MSG10,
           
            SS_VICTORY7=89,

            SS_MESSAGE8,
            SS_CHARACTER_CANNOT_CONTINUE,
            
            // DDR
			SS_DESPONDENTTOKENS=92,
			SS_RECKLESSTOKENS=93,
            SS_CHAR_DESPONDENCY_TIRED=94,
            
			SS_LIEGE=95,
			SS_FOE=96,
			SS_TRAITS=97,
			SS_TUNNEL=98,
            //
            
            SS_GROUP_LEADER=99,
            SS_GROUP_FOLLOWER=100,
            SS_LOYAL_TO=101,
            
            SS_BATTLE_PREPARES_BATTLE=102,
            SS_KILLED_BY=103,
            SS_KILLED_IN_BATTLE=104,
            
            SS_SPARE_1=105,
            SS_SPARE_2=106,
            SS_SPARE_3=107,
            SS_SPARE_4=108,
            SS_SPARE_5=109,

            SS_SEES_1=110,
            SS_SEES_2=111,
            SS_SEES_3=112,
            SS_LOCATION_PREP_1=113,
            SS_LOCATION_PREP_2=114,
            SS_CHARACTER_DEAD=115,
            
            SS_CHARACTER_HERE=116,
            SS_SHARETH_DEAD=117,
            SS_OBJECT_USE_1=118,
            SS_OBJECT_USE_2=119,
            SS_OBJECT_USE_3=120,
            SS_OBJECT_USE_4=121,
            SS_MESSAGE_MIST=122,
            SS_TOKENS_VICTORY=123,
		};

		enum PLURAL {
			PLURAL_HAS=0,
			PLURAL_DAY=2,
			};

#ifdef _DEFINE_ENUMS_
	}
	// namespace enums
}
// namespace tme
	using namespace tme::enums ;
#endif














namespace tme {

// Foward Prototypes
	
	mxid MakeLocId(s16 x, s16 y);
	mxid MakeMapLocId(s16 x, s16 y);
	s16 GetLocX(mxid id);
	s16 GetLocY(mxid id);
	

#define MXSUCCESS(x)				(x > 0)
#define MXFAILED(x)					(x <= 0)

// normal idt
#define MAKE_ID(type,id)			(mxid)(((type)<<24)|id)
#define ID_TYPE(id) 				(id_type_t)((id>>24)&0xff)
#define GET_ID(id) 					(id&0x00ffffff)
// army idt
#define MAKE_ARMYID(x,y)			MAKE_ID(IDT_ARMY,((x<<16)|y))
#define GET_ARMYIDTYPE(id)			((id>>16)&0xff)
#define GET_ARMYID(id)				(id&0xffff)
// location idt

#define PREV_DIRECTION(x)			((mxdir_t)((x-1)&7)) 
#define NEXT_DIRECTION(x)			((mxdir_t)((x+1)&7))

#define MAKE_MAPLOCID(x,y)			MakeMapLocId((s16)x,(s16)y)
#define MAKE_LOCID(x,y)				MakeLocId((s16)x,(s16)y)
#define GET_LOCIDX(id)				GetLocX(id)
#define GET_LOCIDY(id)				GetLocY(id)

#define CLEARINFOSTRUCT(x,t) 		CLEARSTRUCT(x); x.id = MAKE_ID(t,0); x.size = sizeof(x);
#define CLEARINFOSTRUCTPTR(ptr,t,s) memset(ptr,0,sizeof(s)); ptr->id = MAKE_ID(t,0); ptr->size = sizeof(s);
#define ID_4CC(ch0,ch1,ch2,ch3)		((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#define TERRAIN_VARIANT(x,y)		(x|(y*<<6))

	typedef struct callback_t {
		enum type_t {
			none	= 0,
			msg     = 1,
            gameover= 2,
            battle	= 4,
		};
		
		type_t	type;
	} callback_t ;
	
	typedef struct battle_callback_t : callback_t {
		int             initiative;
		int             idChar;
		int             idFriend;
		int             idFoe;
		int             amount;
		void*           armyFriend;
		void*           armyFoe;
	} battle_callback_t ;

    typedef struct message_callback_t : callback_t {
        char*           message;
	} message_callback_t ;

    typedef struct gameover_callback_t : callback_t {
        //char*           message;
        m_gameover_t    condition;
	} gameover_callback_t ;
    
	typedef void (*PFNNIGHTCALLBACK)( callback_t* );
	typedef BOOL (*PFNSERIALIZE)( u32 version, lib::archive& ar );
	typedef MXRESULT (*PFNCOMMAND)( const string& arg, variant argv[], u32 argc);
	
	
//
	typedef struct loc_t {
		s32				x;
		s32				y;
        
        loc_t() { this->x=0; this->y=0; }
        loc_t(s32 x, s32 y ) { this->x=x; this->y=y; }
	
 		void operator+=(loc_t value)				{ x += value.x; y += value.y; }
		void operator-=(loc_t value)				{ x -= value.x; y -= value.y; }
        
        loc_t operator+(loc_t value) const			{ return loc_t(x + value.x, y + value.y); }
        loc_t operator-(loc_t value) const			{ return loc_t(x - value.x, y - value.y); }

        BOOL operator==(loc_t value) const			{ return (x == value.x && y == value.y); }

    
    } loc_t ;

	typedef struct panloc_t {
		loc_t	location;		// location
		u32		size;			// size of location 0-7
		f32		scale;			// float scaling version of size based on 1.0 = 128x128
		point	screen;			// screen position when rendered in a 512x384 screen
	} panloc_t ;

	typedef struct info_t {
		size_t			size;							// size of structure 
		mxid			id;								// 
	} info_t ;								

	typedef struct mxinfo_t : public tme::info_t {
		flags32			flags;
		LPSTR			name;
		void*			userdata;
	} mxinfo_t ;

	typedef struct mxitem_t : public tme::info_t {
		loc_t			location;
		flags32			flags;
		void*			userdata;
	} mxitem_t ;

	//typedef struct charimages_t {
	//	u32				shield;
	//	u32				face;
	//	u32				body;
	//} charimages_t ;

	typedef struct battle_t {
		loc_t			location;
		u32				killed;
		mxbattlestatus_t	status;
	} battle_t ;

	typedef struct recruitment_t {
		u32				group;
		u32				by;
	} recruitment_t ;
    
	namespace flags {

		// interface flags
		enum SELECTIONFLAGS {
			slf_none				=	0,
			slf_all					=	MXBIT(1),
			slf_tunnel				=	MXBIT(2),
		};

		enum ENTITYFLAGS {
			et_disabled					= MXBIT(31),
		};

		enum MISSIONFLAGS {
			mf_complete					= MXBIT(0),	// this score has been completed
			//mf_enabled				= MXBIT(1),	// this score is enabled
			mf_any						= MXBIT(2),	// any reference will do
		};
		enum VICTORYFLAGS {
			vf_gameover					= MXBIT(0),	// this victory condition triggers game over 
			//vf_enabled					= MXBIT(1),	// this victory condition is enabled
			vf_complete					= MXBIT(2),
		};

		enum LOCATIONINFOFLAGS {
			lif_moveforward				= MXBIT(0),	// can we move into the location we are looking at?
			lif_seek					= MXBIT(1),	// can we do a search - always true at present
			lif_hide					= MXBIT(2),	// can the character hide here?
			lif_fight					= MXBIT(3),	// is there anything to fight, and can the <character> do it?
			lif_recruitchar				= MXBIT(4),	// is there a lord to fight?
			lif_recruitmen				= MXBIT(5),	// are there any soldiers to recruit, and can the <character> do it
			lif_guardmen				= MXBIT(6),	// could the <character> place and men on guard
			lif_enterbattle				= MXBIT(7),	// can we enter into battle?
			lif_enter_tunnel			= MXBIT(8),	// can we enter tunnel
			lif_exit_tunnel				= MXBIT(9),	// can we exit tunnel
			lif_rest					= MXBIT(10),	// can we rest
			lif_give					= MXBIT(11),	// can we give
			lif_take					= MXBIT(12),	// can we take
			lif_use						= MXBIT(13),	// can we use
            lif_blocked                 = MXBIT(14),    // way forward is blocked
		};

		enum LOCATIONFLAGS { // 28 bits
			lf_mist						= MXBIT(0), // DDR Scenario, currently not used
			lf_tunnel					= MXBIT(1), // DDR Scenario, currently not used
			lf_creature					= MXBIT(2), // DDR Scenario, currently not used
			lf_character				= MXBIT(3), // character in location
			lf_army						= MXBIT(4), // army at location
			lf_seen						= MXBIT(5), // has been seen by the mapper
			lf_domain					= MXBIT(6), // special domain
			lf_special					= MXBIT(7), // is interesting
			lf_building					= MXBIT(8), // currently not used
			lf_stronghold				= MXBIT(9), // stronghold at location
			lf_routenode				= MXBIT(10), // routenode at location
			lf_terrain_custom			= MXBIT(11), // terrain is custom stored
			lf_object_custom			= MXBIT(12), // object is custom stored
			lf_area_custom				= MXBIT(13), // area is custom stored
#if defined(_DDR_)
            lf_tunnel_exit				= MXBIT(14), // can exit tunnel here
            lf_tunnel_entrance			= MXBIT(15), // can enter tunnel here
#endif
            lf_looked_at                = MXBIT(16), // has been stood in front of
            lf_visited                  = MXBIT(17), // player has visited this location
#if defined(_DDR_)
            lf_tunnel_looked_at         = MXBIT(18), // player has seen the tunnel
            lf_tunnel_visited           = MXBIT(19), // played has visited the tunnel location
            lf_tunnel_passageway        = MXBIT(20), // tunnel is a pssageway NOTE: this is technicall ~(lt_tunnel_exit|lf_tunnel_entrance)
            lf_object                   = MXBIT(21), // object to take
            lf_object_special           = MXBIT(22), // the object to take is one of the special objects
#endif

		};

		enum CHARACTERFLAGS {
			cf_hidden					= MXBIT(0),	// currently hiding 
			cf_riding					= MXBIT(1),	// currently riding
			cf_alive					= MXBIT(2),	// alive
			cf_recruited				= MXBIT(3),	// currently recruited
			cf_army						= MXBIT(4),	// allowed an army
			cf_hide						= MXBIT(5),	// allowed to hide
			cf_horse					= MXBIT(6),	// allowed a horse
			cf_moonring					= MXBIT(7),	// allowed the moonring

			// these should be user defined for the scenario
			// lom
			cf_icecrown					= MXBIT(8),	// allowed the icecrown
			cf_destroyicecrown			= MXBIT(9),	// can destroy the ice crown
			// lom
			
			// ddr
			cf_allowedwarriors			= MXBIT(10),	// can have warriors
			cf_allowedriders			= MXBIT(11),	// can have warriors
			// ddr

            cf_ai                       = MXBIT(12),    // character is an AI character
            cf_killed_foe               = MXBIT(13),    // character killed his foe
			
#if defined(_LOM_)
            cf_resting					= MXBIT(16),	// currently resting
            cf_inbattle					= MXBIT(17),	// currently in battle
            cf_wonbattle				= MXBIT(18),	// just won a battle
            cf_tunnel					= MXBIT(19),	// currently in a tunnel (DDR)
            cf_usedobject				= MXBIT(20),	// has used his special object (DDR)
            cf_followers                = MXBIT(21),    // has followers
            cf_preparesbattle           = MXBIT(22),    // prepares to do battle
            cf_approaching              = MXBIT(23),    // we are approaching a lord (DDR)
#endif
#if defined(_DDR_)
            cf_resting					= MXBIT(14),	// currently resting
            cf_inbattle					= MXBIT(15),	// currently in battle
            cf_wonbattle				= MXBIT(16),	// just won a battle
            cf_tunnel					= MXBIT(17),	// currently in a tunnel (DDR)
            cf_usedobject				= MXBIT(18),	// has used his special object (DDR)
            cf_followers                = MXBIT(19),    // has followers
            cf_preparesbattle           = MXBIT(20),    // prepares to do battle
            cf_approaching              = MXBIT(21),    // we are approaching a lord (DDR)
#endif
            
		};

		enum CHARACTERTRAITS {
			ct_good						= MXBIT(0),
			ct_strong					= MXBIT(1),
			ct_forceful					= MXBIT(2),
			ct_generous					= MXBIT(3),
			ct_stubborn					= MXBIT(4),
			ct_brave					= MXBIT(5),
			ct_swift					= MXBIT(6),
			ct_loyal					= MXBIT(7),
            
			ct_evil						= MXBIT(8),
			ct_weak						= MXBIT(9),
			ct_reticent					= MXBIT(10),
			ct_greedy					= MXBIT(11),
			ct_fawning					= MXBIT(12),
			ct_coward					= MXBIT(13),
			ct_slow						= MXBIT(14),
			ct_treacherous				= MXBIT(15),
		};

		enum REGIMENTFLAGS {
			rf_direct					= MXBIT(0),	// regiment will directly do their command
			rf_inbattle					= MXBIT(17),	// currently in battle
			rf_tunnel					= MXBIT(19),	// currently in a tunnel (DDR)
		};

		enum OBJECTFLAGS {
			of_fight					= MXBIT(0),	// object can be fought
			of_pickup					= MXBIT(1),	// object can be picked up
			of_drop						= MXBIT(2),	// object can be dropped
			of_weapon					= MXBIT(3),	// object is a weapon
			of_see						= MXBIT(4),	// object can be seen
			of_remove					= MXBIT(5),	// object is removed from map when found
			of_unique					= MXBIT(6),	// object is unique
			of_enabled					= MXBIT(7),	// object is enabled
#if defined(_DDR_)
            of_recruitment              = MXBIT(8), // object can help with recruitment
            of_randomstart              = MXBIT(9), // object starts randomly on map
#endif
		};

		enum TERRAININFOFLAGS {
			tif_plural					= MXBIT(0),	// terrain has plural
			tif_block					= MXBIT(1),	// terrain blocks movement? ( not implemented )
			tif_interesting				= MXBIT(2),	// terrain is interesting
			tif_army					= MXBIT(3),	// army is visible at terrain
		};
	}
	// namespace flags


	// mxgridref
	class mxgridref : public loc_t
	{
	public:
		mxgridref();
		mxgridref( u32 x, u32 y );
		mxgridref( loc_t loc );
		~mxgridref();

		lib::archive& Serialize ( lib::archive& ar );

		// there are no subtracts
		void AddDirection( mxdir_t dir );
		mxgridref operator + ( mxdir_t dir );
		s32 operator - ( mxgridref loc );
		BOOL operator == ( mxgridref& loc );
		BOOL operator != ( mxgridref& loc );
		void operator += ( mxdir_t dir )						{ AddDirection(dir); }
		mxdir_t DirFromHere ( mxgridref loc ) const;

	};
	// mxgridref


	//namespace types {
	
	class CStrBuf
	{
	public:
		CStrBuf();
		CStrBuf( u32 size );
		~CStrBuf();
		void sprintf ( LPSTR input, ... );
		void sprintf ( CStrBuf* input, ... );
		
		size_t Length () const			{ return chilli::lib::strlen(buffer); }
		LPSTR End ()					{ return GetAt()+Length(); }
		LPSTR GetAt ()					{ return buffer; }
		operator LPSTR() const			{ return buffer; }
		operator LPCSTR() const			{ return buffer; }
		void strcat ( LPCSTR input );
		void strcpy ( LPCSTR input );
		void Append (  CStrBuf* input );
		void Copy (  CStrBuf* input );
		void Resize ( size_t size );
		void CheckSize ( size_t size );
		int Id() const					{ return id; }
		void Id( u32 value )			{ id = value; }
	private:
		//	friend class tme::processors::text;
		LPSTR	buffer;
		u32		id;
		size_t	cBytes;
	};
	


}
// namespace tme

using namespace tme::flags ;



#define DATABASEHEADER		"MidnightEngineDatabase"
#if defined(_LOM_)
#define DATABASEVERSION		7
#else
#define DATABASEVERSION		11
#endif
#define MAPHEADER			"MidnightEngineMap"
#define MAPVERSION			2

#define DISCOVERYHEADER		"MidnightEngineDiscovery"
#define DISCOVERYVERSION	1

#define SAVEGAMEHEADER		"MidnightEngineSaveGame"

#if defined(_DDR_)
    #define SAVEGAMEVERSION		12
#endif
#if defined(_LOM_)
    #define SAVEGAMEVERSION		9
#endif


#define TME_MAGIC_NO		ID_4CC('T','M','E','!')


#endif //_LOMXTYPES_H_INCLUDED_

