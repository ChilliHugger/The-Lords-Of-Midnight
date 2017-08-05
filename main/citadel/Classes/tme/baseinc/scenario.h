#ifndef _SCENARIO_H_INCLUDED_
#define _SCENARIO_H_INCLUDED_

namespace tme {

	//namespace scenarios {

	//class mxengine;
	
		typedef struct scenarioinfo_t {
			u32		Id;
			u32		Version;
			LPCSTR	Title;
			LPCSTR	Author;
			LPCSTR	AuthorEmail;
			LPCSTR	AuthorUrl;
			LPCSTR	Description;
			LPCSTR	Copyright;
		} scenarioinfo_t ;


		// default engine scenario
		// features
		enum FEATURES {
			SF_MOONRING		=	MXBIT(0),
			SF_ICEFEAR		=	MXBIT(1),
			SF_APPROACH_DDR	=	MXBIT(2),
			SF_RECRUIT_DDR	=	MXBIT(3),
			SF_RECRUIT_TIME	=	MXBIT(4),
            SF_TUNNELS      =   MXBIT(5),
            SF_MIST         =   MXBIT(6),
		};

		class mxscenario_i 
		{
		public:

			virtual scenarioinfo_t* GetInfoBlock() const =0;
			virtual MXRESULT Command ( const string& arg, variant argv[], u32 argc )=0;
			virtual MXRESULT GetProperties ( const string& arg, variant argv[], u32 argc )=0;
			virtual MXRESULT Text ( const string& command, variant* argv=NULL, u32 args=0 )=0;
			
			//virtual MXRESULT Register ( mxengine* midnightx ) =0;
			//virtual MXRESULT UnRegister ( mxengine* midnightx )=0;
		};
		
	
	//}

	namespace scenarios {
		// export structures
		namespace exports {
			
			typedef struct character_t : public mxitem_t {
				
				LPSTR			longname;
				LPSTR			shortname;
				
				flags32			traits;
				
				mxrace_t		race;
				mxgender_t		gender;
				mxdir_t			looking;
				mxtime_t		time;
				mxrace_t		loyalty;
				
				//charimages_t	images;
				battle_t		battle;
				recruitment_t	recruit;
				
				u32				reckless;
				u32				energy;
				u32				strength;
				u32				cowardess;
				u32				courage;
				u32				fear;
				
				mxid			foe;
				mxid			liege;
				mxid			object;
				mxid			killedby;
				
				mxwait_t		wait;
				
				mxid			riders;
				mxid			warriors;
				
                mxid            following;
                u32             followers;
                
                command_t       lastcommand;
                mxid            lastcommandid;
                
                loc_t           lastlocation;
				loc_t           targetlocation;
                loc_t           homelocation;
                mxorders_t      orders;
                
			} character_t;
			
			typedef struct routenode_t : public mxitem_t {
				mxid			paths[2];
			} routenode_t ;
			
			typedef struct regiment_t : public mxitem_t {
				mxrace_t			race;
				mxunit_t		type;
				u32				total;
				mxid			target;
				loc_t			targetlocation;
				mxorders_t		orders;
				u32				success;
				u32				killed;
				mxid			loyalty;
                loc_t           lastlocation;
			} regiment_t ;
			
			typedef struct stronghold_t : public mxitem_t {
				mxrace_t		occupyingrace;
				mxrace_t		race;
				mxunit_t		type;
				u32				total;
				u32				min;
				u32				max;
				u32				strategical_success;
				u32				owner_success;
				u32				enemy_success;
				u32				influence;
				u32				respawn;
				mxid			occupier;
				mxid			owner;
				mxterrain_t		terrain;
				u32				killed;
                mxrace_t		loyalty;
			} stronghold_t ;
			
			typedef struct place_t : public mxitem_t {
			} place_t ;
			
			typedef struct memoryitem_t : public mxitem_t {
				u32				day;
				mxid			data;
				mxrace_t		race;
			} memoryitem_t ;
			
			//
			// infos
			//
			typedef struct areainfo_t : public mxinfo_t {
				LPSTR			prefix;
			} areainfo_t ;
			
			typedef struct directioninfo_t : public mxinfo_t {
				mxdir_t			type;
			} directioninfo_t ;
			
			typedef struct genderinfo_t : public mxinfo_t {
				mxgender_t		type;
				LPSTR			pronoun[3];
			} genderinfo_t ;
			
			typedef struct object_t : public mxitem_t {
				mxthing_t		type;
				mxthing_t		kills;
				LPSTR			name;
				LPSTR			description;
				mxid			usedescription;
			} object_t ;
			
			typedef struct raceinfo_t : public mxinfo_t {
				mxrace_t			type;
				LPSTR			soldiersname;
				u32				success;
				u32				initialmovement;
				s32				diagonalmodifier;
				s32				ridingmultiplier;
				u32				movementmax;
				u32				baserestamount;
				u32				strongholdstartups;
				s32				misttimeaffect ;
				s32				mistdespondecyaffect ;
				s32				baseenergycost ;
				s32				baseenergycosthorse ;
			} raceinfo_t ;
			
			typedef struct terraininfo_t : public mxinfo_t {
				mxterrain_t		type;
				LPSTR			preposition;
				LPSTR			description;
				u32				success;
				u32				visibility;
				u32				obstruction;
				s32				movementcost;
			} terraininfo_t ;
			
			typedef struct unitinfo_t : public mxinfo_t {
				mxunit_t		type;
				u32				success;
				u32				baserestmodifier;
			} unitinfo_t ;
			
			typedef struct commandinfo_t : public mxinfo_t {
				command_t		type;
				s32				successtime;
				s32				failuretime;
			} commandinfo_t ;
			
			//
			// other
			//
			
			typedef struct armytotal_t {
				u32				warriors ; 
				u32				riders ;
				u32				armies ;
				u32				adjustment ;
				mxid			character;
			} armytotal_t ;
			
			typedef struct army_t : public tme::info_t {
				mxid			parent;
				mxarmytype_t	type;				// this is probably redundant, contained within parent?
				mxunit_t		unit;
				mxrace_t		race;
				u32				total;
				u32				energy;
				u32				lost;
				u32				killed;
			} army_t ;
			
			typedef struct location_t : public mxitem_t {
				mxterrain_t		terrain;
				mxid			object;
                mxid            object_tunnel;
				mxid			area;
				u8				climate;
				u8				density;
                u8              tunnel;
                flags32			discovery_flags;
			} location_t ;
		} // namespace export ;
	} // namespace scenarios
	
}// namespace tme


//typedef tme::scenarios::base* (__cdecl *FNSCENARIOREGISTER) ( engine* mx );

// alias default export namespace
#define defaultexport	tme::scenarios::exports


#endif //_SCENARIO_H_INCLUDED_
