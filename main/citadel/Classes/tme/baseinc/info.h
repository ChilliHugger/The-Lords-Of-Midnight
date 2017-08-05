#ifndef _INFO_H_INCLUDED_
#define _INFO_H_INCLUDED_

#include "library.h"

#define MAX_CHARACTERS_FOLLOWING        32

#if defined(_DDR_)
#define MAX_CHARACTERS_INLOCATION		32
#define MAX_REGIMENTS_INLOCATION		64
#define MAX_FOEARMIES_INLOCATION		32
#define MAX_FRIENDARMIES_INLOCATION		32
#else
#define MAX_CHARACTERS_INLOCATION		128
#define MAX_REGIMENTS_INLOCATION		128
#define MAX_FOEARMIES_INLOCATION		128
#define MAX_FRIENDARMIES_INLOCATION		128
#endif

#define MAX_ARMIES_INLOCATION			(MAX_FOEARMIES_INLOCATION+MAX_FRIENDARMIES_INLOCATION)
#define MAX_STRONGHOLDS_INLOCATION		1
#define MAX_ROUTENODES_INLOCATION		1

class CListS;

#include "variables.h"

using namespace tme ;
using namespace chilli;
using namespace chilli::lib;
using namespace chilli::collections;

// enum control
//#define DECLARE_ENUM(x)		namespace x { enum  
//#define END_ENUM(x)			}; }


#define DEFAULT_IMPLEMENTATION(x) \
		x(void); \
		virtual ~x(void); \
		virtual void Serialize ( chilli::lib::archive& ar ); \
		virtual MXRESULT FillExportData ( info_t* data ) 


namespace tme {

	// forward references

	namespace types {} ;
	//namespace lib {} ;
	namespace os {} ;
	namespace variables {} ;

	class mxentity ;
	class mxgridref ;
	class mxmemory ;
	class mxmap;
    class mxdiscoverymap;

	// units
	//namespace units {
		class mxunit ;
			class mxwarriors ;
			class mxriders ;
	//}

	// collections
	//namespace collections {
		//class c_idt ;
		class mxentities ;
		class mxinfos ;
	//}

	//namespace processors 
	//{
		//class mxprocessor;
			class mxtext;
			class mxnight;
			class mxbattle;
	//} ;

	//namespace scenarios 
	//{
		class mxscenario;
			class lom;
			class ddr;
	//};

	//namespace info 
	//{
		class mxinfo;
			class mxdirection;
			class mxunit;
			class mxrace;
			class mxgender;
			class mxterrain;
			class mxarea;
			class mxcommand;
	//} ;

	//namespace item {
		class mxarmy;
		class mxmission;
		class mxvictory;
		class mxitem ;
			class mxregiment;
			class mxstronghold;
			class mxcharacter;
			class mxroutenode;
			class mxplace;
			//class mxentity;
			class mxlocinfo;
			class mxarmytotal;
	//} ;
	// forward references

	// class entity
	class mxentity
	{
	public:

		DEFAULT_IMPLEMENTATION(mxentity);

		u32 SafeId() const ;
		mxid SafeIdt() const ;

		PROPERTY( id_type_t, Type, type );
		PROPERTY( u32, Id, id );

		string& Symbol() 		{ return symbol ; }
		BOOL IsSymbol(LPCSTR s) const { return lib::stricmp(symbol,s) == 0; }

		virtual int Compare ( mxentity* o, int hint ) const;

		void SetUserData(const void* data) { user_data = data; }
		const void* GetUserData() { return user_data; }

		flags32& Flags()			{ return flags; } 
		BOOL IsFlags(u32 f)			{ return flags.Is(f); }
		FLAG_PROPERTY( IsDisabled,	et_disabled )

	protected:
		id_type_t	type;
		u32			id;
		string		symbol;
		flags32		flags;
		const void*		user_data ;

	};
	// class entity

	
	// class mxfragment
	class mxfragment
	{
	public:
		mxfragment();
		mxfragment( mxitem* item );
		virtual ~mxfragment();
		
		void Serialize ( chilli::lib::archive& ar );
		u32 Age() const;
		
	public:
		mxfragment*		m_Prev;
		mxfragment*		m_Next;
		u32				m_day;
		mxentity*		m_data;
		mxgridref		m_location;
		mxrace_t			m_race;
		
	};
	// class mxfragment
	
	// class mxmemory
	class mxmemory
	{
	public:
		mxmemory();
		virtual ~mxmemory();
		void Serialize ( chilli::lib::archive& ar );

		void Memorise ( mxitem* info, mxrace_t race );
		void ForgetAll ( void );
		void Update ( void );
		mxfragment* IsMemorised ( mxitem* info ) const;
		mxfragment* IsMemorised ( mxgridref loc, mxrace_t race ) const;
		void ForgetFragment ( mxfragment* item );
		void DeleteFragment ( mxfragment* item );

	private:
		mxfragment*	m_first;

	};
	
	//namespace units {
		// class mxunit
		class mxunit
		{
		public:
			mxunit();
			~mxunit();
			chilli::lib::archive& Serialize ( chilli::lib::archive& ar );

			s32 BattleSuccess ( const mxlocinfo& locinfo, const mxcharacter* parent );
			s32 BaseRestModifier() const;

			friend chilli::lib::archive& operator<<( chilli::lib::archive& ar, mxunit& unit );
			friend chilli::lib::archive& operator>>( chilli::lib::archive& ar, mxunit& unit );

			PROPERTY( mxunit_t, Type, type );
			PROPERTY( u32, Total, total );
			PROPERTY( u32, Energy, energy );
			PROPERTY( u32, Lost, lost );
			PROPERTY( u32, Killed, slew );
#if defined(_DDR_)
            void Loses ( s32 value ) { total = BSub(total, value,0); }
#endif
			const string& Name() const;

		protected:
			friend class	mxcharacter ;
			mxunit_t		type;
			u32				total;
			u32				energy;
			u32				lost;
			u32				slew;
		};
		// class mxunit


		// class mxwarriors
		class mxwarriors : public mxunit
		{
		public:
			mxwarriors();
			~mxwarriors();
			s32 BattleSuccess ( const mxlocinfo& locinfo, const mxcharacter* parent );
		};
		// class mxwarriors

		// class mxriders
		class mxriders : public mxunit
		{
		public:
			mxriders();
			~mxriders();
			s32 BattleSuccess ( const mxlocinfo& locinfo, const mxcharacter* parent );
		};
		// class mxriders

inline chilli::lib::archive& operator<<( chilli::lib::archive& ar, mxunit& unit )		{ return unit.Serialize(ar); }
inline chilli::lib::archive& operator>>( chilli::lib::archive& ar, mxunit& unit )		{ return unit.Serialize(ar); }

	//}
	// namespace units


//#pragma push	
//#pragma pack ( 1 )
	
#define MAP_FLAG_PROPERTY( n, f ) \
	BOOL n() const { return (flags&f) == f; }
	
	// class loc
	class mxloc
	{
	public:
		mxloc();
		~mxloc();
		
		void RemoveObject() 				{ object = 0; }
		MAP_FLAG_PROPERTY( IsInDomain,		lf_domain)
		MAP_FLAG_PROPERTY( IsSpecial,		lf_special)
		MAP_FLAG_PROPERTY( IsVisible,		lf_seen)
		MAP_FLAG_PROPERTY( IsMisty,			lf_mist)
		MAP_FLAG_PROPERTY( IsStronghold,	lf_stronghold)
		MAP_FLAG_PROPERTY( IsRouteNode,		lf_routenode)
#if defined(_DDR_)
		MAP_FLAG_PROPERTY( IsTunnelVisible,	lf_tunnel_looked_at)
		MAP_FLAG_PROPERTY( HasTunnel,		lf_tunnel)
		MAP_FLAG_PROPERTY( HasObject,		lf_object)
#endif
        MAP_FLAG_PROPERTY( HasArmy,			lf_army)
		MAP_FLAG_PROPERTY( HasCharacter,	lf_character)
		BOOL IsInteresting() const ;
		void Serialize ( chilli::lib::archive& ar ) ;
		
#if defined(_DDR_)
		BOOL HasTunnelExit() const ;
		BOOL HasTunnelEntrance() const ;
        BOOL IsTunnelPassageway() const ;
#endif
        
	public:
		u64 terrain		: 7 ; // 128 ( 16x8 )
		u64 variant		: 3 ; // 8 ( 8 )
		u64 climate		: 7 ; // 128 ( 16x8 )
		u64 object		: 9 ; // 512 ( 16x8 )
		u64 area		: 10 ; // 1024 ( 64x16 )
		u64 flags		: 28 ; // ( lf_domain|lf_mist|lf_tunnel )
	};
	// class mxloc
//#pragma pop
	
	
	
	// class mxmap
	class mxmap
	{
	public:

		// landscaping info table structure
		typedef struct landscape_t{
			s8		dy;
			s8		dx;
			s16		xadj;
			u8		y;
			u8		size;
		} landscape_t;


	public:
		mxmap();
		mxmap( u32 x, u32 y );
		virtual ~mxmap();

		virtual void Serialize ( chilli::lib::archive& ar ) ;

		BOOL Load ( LPCSTR filename );
		BOOL Save ( LPCSTR filename );
		BOOL IsLocationSpecial( mxgridref loc );
		BOOL IsLocationBlock( mxgridref loc ) ;
		BOOL IsLocOnMap ( mxgridref loc ) const ;
        BOOL IsLocationVisible( mxgridref loc );
        
		void SetLocationArmy( mxgridref loc, u32 number );
		void SetLocationCharacter( mxgridref loc, u32 number );
		void SetLocationSpecial( mxgridref loc, u32 number );
        void SetLocationVisible( mxgridref l, BOOL visible );
        void SetLocationLookedAt( mxgridref l, BOOL visible );
        void SetLocationVisited( mxgridref l, BOOL visible );

#if defined(_DDR_)
        BOOL HasTunnelEntrance( mxgridref l );
        BOOL HasTunnelExit( mxgridref l );

        void SetTunnelVisible( mxgridref l, BOOL visible );
        BOOL IsTunnelVisible( mxgridref loc );
        BOOL IsTunnel( mxgridref loc );
        BOOL IsTunnelPassageway( mxgridref l );
        BOOL HasObject( mxgridref l );
        void SetObject( mxgridref l, BOOL value );
        void MoveMists ( void );
        void PutThingsOnMap ( void );
#endif
        mxthing_t getLocationObject( const mxcharacter* c, mxgridref loc );

        void ResetVisibleRange();
        void CheckVisibleRange( mxgridref l );
        
		u32 GetPanoramic ( mxgridref loc, mxdir_t dir, panloc_t** locations );

		mxloc& GetAt ( const mxgridref& loc );
		mxloc& operator[] ( mxgridref loc )		{ return GetAt(loc); }

		GET_PROPERTY ( size, Size, m_size );
		GET_PROPERTY ( loc_t, Top, m_top_visible );
		GET_PROPERTY ( loc_t, Bottom, m_bottom_visible );
        
		u8 Density(mxgridref loc)  ;

        void ClearVisible();
        
	private:
		void Pan_Location ( mxgridref loc, int distance, int xtype, int ytype, int scrtype ) ;
		int Pan_AdjustMapPos ( int pos, int type, int dx, int dy );
		BOOL Create ( size dimensions );

        void CalculateVisibleArea();

        
	private:
		size	m_size;
		mxloc*	m_data;
        
        loc_t   m_top_visible;
        loc_t   m_bottom_visible;
        
    public:
        
        // ptr to the current discovery map
        mxdiscoverymap* m_discoverymap;

	}; // class mxmap

    class mxdiscoverymap
    {
    public:
        mxdiscoverymap();
        mxdiscoverymap( u32 x, u32 y );
        
        virtual ~mxdiscoverymap();
        virtual void Serialize ( chilli::lib::archive& ar ) ;
        
        void CalculateVisibleArea();
        BOOL Create ( size dimensions );
        
        void ClearVisibleArea();
        void ResetVisibleArea();
        BOOL Load ( LPCSTR filename );
        BOOL Save ( LPCSTR filename );
        
        BOOL IsLocationVisible( mxgridref l );
        flags32& GetAt ( const mxgridref& mxloc );
        BOOL IsLocOnMap ( mxgridref mxloc ) const;
        
        
        void SetLocationVisible( mxgridref l, BOOL visible );
        void SetLocationLookedAt( mxgridref l, BOOL visible );
        void SetLocationVisited( mxgridref l, BOOL visible );
        void CheckVisibleArea( mxgridref l );
        
        
#if defined(_DDR_)
        BOOL IsTunnelVisible( mxgridref l );
        void SetTunnelVisible( mxgridref l, BOOL visible );
#endif
        
        void TransferFromMap( mxmap* map );
        
        
        GET_PROPERTY ( size, Size, m_size );
        GET_PROPERTY ( loc_t, Top, m_top_visible );
        GET_PROPERTY ( loc_t, Bottom, m_bottom_visible );
        
    public:
        size        m_size;
        flags32*    m_data;
        loc_t       m_top_visible;
        loc_t       m_bottom_visible;
        
    }; // class mxdiscoverymap
    
    
	// namespace collections
	namespace collections {
		// class entities collection
		class entities
		{
		public:
			entities(void);
			~entities(void);
			BOOL Create( u32 count );
			BOOL Create( mxscenario* scenario, id_type_t type, u32 count );
			void Clear(); 
			BOOL CreateIdtCollection ( c_mxid& obj );
			u32 Count() const;

			void operator = ( const entities& src );
			mxentity*& operator[]( u32 nSubscript );
			void operator += ( mxentity* o );

			void Destroy( void );
			virtual void Serialize( chilli::lib::archive& ar );
			mxentity* FindSymbol ( const string& name ) ;
			void Sort ( int hint ) ; // not thread safe

			int Add ( mxentity* );
			u32 Compact( void );
			void Resize ( u32 newsize );
            s32 IndexOf ( mxentity* entity );

		protected:
			u32				m_max;
			u32				m_used;
			mxentity**		m_objElements;
			BOOL			m_bOwner;
		};
		// class entities collection

		// class infos collection
		class infos : public entities
		{
		public:
			infos(void);
			~infos(void);
			virtual void Serialize( chilli::lib::archive& ar );
		};
		// class infos collection
	}
	// namespace collections




	// namespace info
	//namespace info {

		// base
		class mxinfo : public mxentity
		{
		public:
			DEFAULT_IMPLEMENTATION(mxinfo);

			//flags32& Flags()	{ return flags; } 
			string& Name() 		{ return name ; }

		protected:
			//u32		id;
			//flags32	flags;
			string	name;

		}; // base


		// command
		class mxcommand : public mxinfo
		{
		public:
			DEFAULT_IMPLEMENTATION(mxcommand);

		public:
			s32		successtime;
			s32		failuretime;
		}; // command


		// area
		class mxarea : public mxinfo
		{
		public:
			DEFAULT_IMPLEMENTATION(mxarea);

		public:
			string	prefix;
		}; // area

		// direction
		class mxdirection : public mxinfo
		{
		public:
			DEFAULT_IMPLEMENTATION(mxdirection);

		protected:

		}; // direction

		// gender
		class mxgender : public mxinfo
		{
		public:
			DEFAULT_IMPLEMENTATION(mxgender);

		public:
			string	pronoun1;
			string	pronoun2;
			string	pronoun3;
		};

		// race
		class mxrace : public mxinfo
		{
		public:
			DEFAULT_IMPLEMENTATION(mxrace);

			virtual u32 TerrainMovementModifier( mxterrain_t terrain ) const;

			GET_PROPERTY ( u32, Success, success )
			GET_PROPERTY ( u32, InitialMovementValue, initialmovement )
			GET_PROPERTY ( s32, DiagonalMovementModifier, diagonalmodifier )
			GET_PROPERTY ( u32, MovementMax, movementmax )
			GET_PROPERTY ( u32, BaseRestAmount, baserestamount )
			GET_PROPERTY ( u32, StrongholdStartups, strongholdstartups )

			GET_PROPERTY ( s32, BaseEnergyCost, baseenergycost )
			GET_PROPERTY ( s32, BaseEnergyCostHorse, baseenergycosthorse )
			GET_PROPERTY ( s32, MistTimeAffect, misttimeaffect )
			GET_PROPERTY ( s32, MistDespondecyAffect, mistdespondecyaffect )

			f64 RidingMovementMultiplier() const	{ return (f64)ridingmultiplier/10000.0; }
			
			string& SoldiersName();

		protected:
			string	soldiersname;
			u32		success;
			u32		initialmovement;
			s32		diagonalmodifier;
			s32		ridingmultiplier;
			u32		movementmax;
			u32		baserestamount;
			u32		strongholdstartups;

			s32		misttimeaffect ;
			s32		mistdespondecyaffect ;
			s32		baseenergycost ;
			s32		baseenergycosthorse ;
		}; // race

		// terrain
		class mxterrain : public mxinfo
		{
		public:
			DEFAULT_IMPLEMENTATION(mxterrain);

			FLAG_PROPERTY ( IsPlural, tif_plural )
			FLAG_PROPERTY ( IsBlock, tif_block )
			FLAG_PROPERTY ( IsInteresting, tif_interesting )
			FLAG_PROPERTY ( IsArmyVisible, tif_army )

			GET_PROPERTY ( u32, Success, success )
			GET_PROPERTY ( u32, Visibility, visibility )
			GET_PROPERTY ( u32, Obstruction, obstruction )
			GET_PROPERTY ( s32, MovementCost, movementcost )

			string& Preposition() 					{ return preposition; }
			string& Description() 					{ return description; }


		public:
			string	preposition;
			string	description;
			u32		success;
			u32		visibility;
			u32		obstruction;
			s32		movementcost;
		}; // terrain

		// unit
		class mxunitinfo : public mxinfo
		{
		public:
			DEFAULT_IMPLEMENTATION(mxunitinfo);

			GET_PROPERTY ( u32, Success, success )
			GET_PROPERTY ( u32, BaseRestModifier, baserestmodifier )

		protected:
			u32		success;
			u32		baserestmodifier;
		};
    
#if defined(_DDR_)
        // object_power
        class mxobjectpower : public mxinfo
        {
        public:
            DEFAULT_IMPLEMENTATION(mxobjectpower);
            
        protected:
            
        }; // objectpower

        // objecttype
        class mxobjecttype : public mxinfo
        {
        public:
            DEFAULT_IMPLEMENTATION(mxobjecttype);
            
        protected:
            
        }; // objecttype
#endif
    
	//}
	// namespace mxunitinfo


	//namespace item {

		class mxmission;
		class mxvictory;
	
		class mxcharacter;
		class mxroutenode;
		class mxstronghold;
		class mxregiment;
		class mxlocinfo;
		class mxarmytotal;
		class mxobject;

		// victory
		class mxvictory : public mxentity
		{
		public:
			DEFAULT_IMPLEMENTATION(mxvictory);

			//flags32& Flags()	{ return flags; } 

			FLAG_PROPERTY ( IsGameOver, vf_gameover );
			//FLAG_PROPERTY ( IsEnabled, vf_enabled );
			FLAG_PROPERTY ( IsComplete, vf_complete );

			GET_PROPERTY ( u32, Message, string );
			GET_PROPERTY ( s32, Priority, priority );

			virtual BOOL CheckComplete ( void );
			virtual int Compare ( mxentity* o, int hint ) const;


		protected:
			//flags32			flags;
			s32				priority;
			mxid			mission;
			u32				string;
		};
		// victory

		// mission
		class mxmission : public mxentity
		{
		public:
			DEFAULT_IMPLEMENTATION(mxmission);

			//flags32& Flags()	{ return flags; } 

			FLAG_PROPERTY ( IsComplete, mf_complete );
			//FLAG_PROPERTY ( IsEnabled, mf_enabled );
			FLAG_PROPERTY ( IsAny, mf_any );
			GET_PROPERTY ( s32, Priority, priority );
			GET_PROPERTY ( m_condition_t, Condition, condition );
            
			virtual BOOL CheckComplete ( void );
			virtual BOOL Completed ( void );
			virtual mxgridref GetLocation ( mxid id ) const;
			virtual int Compare ( mxentity* o, int hint ) const;


		protected:
			//flags32			flags;
			s32					priority;
			m_objective_t		objective;
			m_condition_t		condition;
			mxid				reference[5];
			s32					points;
			mxid				scorer;
			m_action_t			action;
			mxid				actionid;
		};
		// mission


		// mxarmy
		class mxarmy  
		{
		public:
			mxarmy();
			~mxarmy();

		public:
			mxitem*			parent;
			mxarmytype_t	armytype;
			mxrace_t		race;
			mxunit_t		type;
			u32				total;
			u32				success;
			u32				killed;
			mxrace_t		loyalto;
		};
		// mxarmy


		// base
		class mxitem  : public mxentity
		{
		public:
			DEFAULT_IMPLEMENTATION(mxitem);

			mxitem( mxgridref loc );

			//flags32& Flags()	{ return flags; } 

			PROPERTY( mxgridref, Location, location );



		protected:
			mxgridref			location;
			//flags32			flags;

		};
		// base


		// mxobject
		class mxobject : public mxitem
		{
		public:
			DEFAULT_IMPLEMENTATION(mxobject);

			FLAG_PROPERTY( CanFight,	of_fight )
			FLAG_PROPERTY( CanPickup,	of_pickup )
			FLAG_PROPERTY( CanDrop,		of_drop )
			FLAG_PROPERTY( CanSee,		of_see )
			FLAG_PROPERTY( IsWeapon,	of_weapon )
			FLAG_PROPERTY( MapRemove,	of_remove )
			FLAG_PROPERTY( IsUnique,	of_unique )
#if defined(_DDR_)
            FLAG_PROPERTY( CanHelpRecruitment,		of_recruitment )
			FLAG_PROPERTY( IsRandomStart,	of_randomstart )
            BOOL IsCarried() const;
            BOOL IsSpecial() const;
#endif
            
			BOOL CanDestroy ( mxobject* obj ) const ;
            
			string& Name()  		{ return name ; }

		public:
			mxthing_t		kills;
			string			name;
			string			description;
			u32				usedescription;
			mxitem*			carriedby;

#if defined(_DDR_)
            mxobjtype_t   type;
            mxobjpower_t  power;
#endif

		}; // object


		// mxstronghold
		class mxstronghold  : public mxitem
		{
		public:
			DEFAULT_IMPLEMENTATION(mxstronghold);

			virtual s32 BattleSuccess ( const mxlocinfo& locinfo );
			virtual u32 Remove ( mxrace_t race, mxunit_t type, u32 total );
			virtual u32 Add ( mxrace_t race, mxunit_t type, u32 total );
			virtual void MakeChangeSides( mxrace_t race, mxcharacter* newoccupier );
#if defined(_DDR_)
            virtual void OnRespawn();
            virtual mxrace_t Loyalty ( void );
#endif
            
			void CheckForZero ( void );

			PROPERTY( u32, Total, total );
			PROPERTY( u32, Killed, killed );
			PROPERTY( u32, Lost, lost );
			PROPERTY( u32, Respawn, respawn );
            
			GET_PROPERTY ( mxrace_t, Race, race )
			GET_PROPERTY ( mxrace_t, OccupyingRace, occupyingrace )
			GET_PROPERTY ( mxunit_t, Type, type )
			GET_PROPERTY ( mxterrain_t, Terrain, terrain )
			GET_PROPERTY ( mxcharacter*, Owner, owner )
			GET_PROPERTY ( mxcharacter*, Occupier, occupier )
			PROPERTY ( u32, Min, min )
			PROPERTY ( u32, Max, max )
			GET_PROPERTY ( u32, Influence, influence )
#if defined(_DDR_)
			PROPERTY ( u32, Energy, energy )
#endif
			BOOL HasFallen() const				{ return race!=occupyingrace; }

		protected:
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
			mxcharacter*	occupier;
			mxcharacter*	owner;
			mxterrain_t		terrain;
			u32				killed;
			u32				lost;
            u32             energy;
		};
		// mxstronghold

		// mxroutenode
		class mxroutenode  : public mxitem
		{
		public:
			DEFAULT_IMPLEMENTATION(mxroutenode);

			GET_PROPERTY ( mxroutenode*, Left, paths[0] )
			GET_PROPERTY ( mxroutenode*, Right, paths[1] )

		protected:

			mxroutenode*		paths[2];
		};
		// routenode


		// mxplace
		class mxplace : public mxitem
		{
		public:
			DEFAULT_IMPLEMENTATION(mxplace);

		};
		// mxplace


		// mxregiment
		class mxregiment  : public mxitem
		{
		public:
			DEFAULT_IMPLEMENTATION(mxregiment);

			virtual s32 BattleSuccess ( const mxlocinfo& locinfo );
			virtual void Cmd_Night ( void );

			PROPERTY ( mxrace_t, Race, race )
			PROPERTY ( mxunit_t, Type, type )
			PROPERTY ( mxorders_t, Orders, orders )
			PROPERTY ( u32, Success, success )
			PROPERTY ( mxid, TargetId, targetid )

			GET_PROPERTY ( mxcharacter*, Loyalty, loyalty )

			FLAG_PROPERTY ( IsInTunnel, rf_tunnel )

			PROPERTY( u32, Delay, delay );
			PROPERTY( u32, Total, total );
			PROPERTY( u32, Killed, killed );
			PROPERTY( u32, Lost, lost );

			virtual void Target ( const mxitem* target );

		protected:
		// internal
			virtual void NextTurn( void );
			virtual void EndOfTurn( void );
			virtual BOOL RetargetTarget ( void );
			virtual void Cmd_Goto ( void );
			virtual void Cmd_Wander ( void );
			virtual void Cmd_Follow ( void );
			virtual void Cmd_Route ( void );

			mxgridref TargetLocation() const ;

		protected:
			mxrace_t		race;
			mxunit_t		type;
			u32				total;
			mxid			targetid;
			mxorders_t		orders;
            u32             delay;
			u32				success;
			u32				killed;
			u32				lost;
			mxcharacter*	loyalty;
            
            mxgridref       lastlocation;

		// night processing and thus not required for storage
			u32             turns;
			mxgridref		targetlocation;
		//

		};
		// mxregiment
		
		// mxcharacter
		class mxcharacter  : public mxitem
		{
		public:
			DEFAULT_IMPLEMENTATION(mxcharacter);

			virtual mxlocinfo* GetLocInfo();
			virtual s32 DistanceFromLoc ( mxgridref loc );

			flags32& Traits()	{ return traits; } 

			GET_PROPERTY ( mxobject*, Carrying, carrying )

			GET_PROPERTY ( mxwait_t, WaitMode, wait )
			GET_PROPERTY ( mxtime_t, Time, IsResting()?sv_time_night:time )
			GET_PROPERTY ( mxdir_t, Looking, looking )
			GET_PROPERTY ( mxrace_t, Race, race )
			//GET_PROPERTY ( charimages_t, Images, images )
			GET_PROPERTY ( BOOL, IsWaiting, WaitMode()!=WM_NONE )
			GET_PROPERTY ( BOOL, IsNight, Time()==(mxtime_t)sv_time_night||IsResting() )
			GET_PROPERTY ( BOOL, IsDawn, Time()==(mxtime_t)sv_time_dawn&&!IsResting() )
			GET_PROPERTY ( BOOL, IsFollowing, Following()!=NULL )

			GET_PROPERTY ( mxrace_t, Loyalty, loyalty )
			GET_PROPERTY ( mxcharacter*, Foe, foe )
			GET_PROPERTY ( mxcharacter*, Liege, liege )
			GET_PROPERTY ( mxcharacter*, Following, following )

			FLAG_PROPERTY ( IsHidden, cf_hidden )
			FLAG_PROPERTY ( IsRiding, cf_riding )
			FLAG_PROPERTY ( IsAlive, cf_alive )
			FLAG_PROPERTY ( IsRecruited, cf_recruited )
			FLAG_PROPERTY ( IsAllowedArmy, cf_army )
#if defined(_DDR_)
			FLAG_PROPERTY ( IsAllowedWarriors, cf_allowedwarriors )
			FLAG_PROPERTY ( IsAllowedRiders, cf_allowedriders )
            FLAG_PROPERTY ( IsPreparingForBattle, cf_preparesbattle )
            FLAG_PROPERTY ( IsAIControlled, cf_ai )
            FLAG_PROPERTY ( IsApproaching, cf_approaching )
#endif
            FLAG_PROPERTY ( IsAllowedHide, cf_hide )
			FLAG_PROPERTY ( IsAllowedHorse, cf_horse )
			FLAG_PROPERTY ( IsAllowedMoonring, cf_moonring )
			FLAG_PROPERTY ( IsAllowedIcecrown, cf_icecrown )
			FLAG_PROPERTY ( CanDestroyIcecrown, cf_destroyicecrown )
			FLAG_PROPERTY ( IsInBattle, cf_inbattle )
			FLAG_PROPERTY ( HasWonBattle, cf_wonbattle )
			FLAG_PROPERTY ( IsInTunnel, cf_tunnel )
			FLAG_PROPERTY ( HasUsedObject, cf_usedobject )
			FLAG_PROPERTY ( IsResting, cf_resting )
			FLAG_PROPERTY ( HasFollowers, cf_followers )
            
            
			BOOL HasArmy() const				{ return ((riders.total+warriors.total) > 0); }
			BOOL IsDead() const					{ return !flags.Is(cf_alive); }
			const string& Longname() const		{ return longname; }
			const string& Shortname() const		{ return shortname; }

			void SetLastCommand ( command_t cmd, mxid id );
			void CommandTakesTime ( BOOL success );

			// Commands
			virtual MXRESULT Cmd_LookLeft ( void );
			virtual MXRESULT Cmd_LookRight ( void );
			virtual MXRESULT Cmd_LookDir ( mxdir_t dir );
			virtual MXRESULT Cmd_MoveForward ( void );
			virtual MXRESULT Cmd_WalkForward ( BOOL peform_seek );
			virtual MXRESULT Cmd_PostMen ( mxstronghold* s=NULL, u32 qty=0 );
			virtual MXRESULT Cmd_RecruitMen ( mxstronghold* s=NULL, u32 qty=0 );
			virtual MXRESULT Cmd_Attack ( void );
			virtual MXRESULT Cmd_Rest ( void );
			virtual MXRESULT Cmd_Hide ( void );
			virtual MXRESULT Cmd_UnHide ( void );
			virtual MXRESULT Cmd_Lookat(mxgridref loc);
			virtual MXRESULT Cmd_DropObject ( void );
			virtual MXRESULT Cmd_Wait ( mxwait_t mode );

			virtual MXRESULT Cmd_Follow ( mxcharacter* c );
			virtual MXRESULT Cmd_UnFollow ( mxcharacter* c  );
            
            virtual MXRESULT Cmd_DisbandGroup ( void );
            virtual MXRESULT Cmd_SwapGroupLeader ( mxcharacter* character );

            
			virtual mxcharacter* Cmd_Approach ( mxcharacter* character=NULL );
			virtual mxobject* Cmd_Fight ( void );
			virtual mxobject* Cmd_Seek ( void );
			virtual mxobject* Cmd_PickupObject ( void );
			virtual void Cmd_Dead ( void );
            
#if defined(_DDR_)
			virtual MXRESULT Cmd_EnterTunnel ( void );
			virtual MXRESULT Cmd_ExitTunnel ( void );
            virtual MXRESULT Cmd_Use ( void ) ;
            virtual MXRESULT Cmd_Take ( void ) ;
            virtual MXRESULT Cmd_Give ( mxcharacter* character ) ;
            virtual void Turn ( void );
            virtual mxcharacter* GetNextFoe() const;
            virtual mxcharacter* GetNextLiege() const;
#endif
            virtual MXRESULT EnterBattle ( void );

            virtual BOOL EnterLocation ( mxgridref loc );
			virtual void CanSeeLocation ( mxgridref loc );
            virtual BOOL CanFollow( const mxcharacter* c ) const ;
			virtual BOOL CheckFightObject ( mxobject* object ) const;
			virtual BOOL CheckRecruitChar ( mxcharacter* pChar ) const ;
			virtual BOOL Recruited ( mxcharacter* character );
			virtual void LostFight ( void );
			virtual void Displace ( void );
			virtual void DecreaseEnergy ( s32 amount );
			virtual void IncreaseEnergy ( s32 amount );
			virtual void StartDawn ( void );
			virtual BOOL HasBattleInfo() const ;
			virtual mxthing_t LocationThing() const ;
			virtual void RefreshLocationBasedVariables ( const mxlocinfo* info ) ;
            virtual BOOL CanWalkForward ( void );

            virtual BOOL AddFollower ( mxcharacter* c );
            virtual BOOL RemoveFollower ( mxcharacter* c );

			//
			BOOL IsFriend( const mxcharacter* c ) const;
			BOOL IsOnSameSide ( const mxcharacter* c ) const;
			const mxcharacter* CommanderInChief ( void ) const;

			void ForcedVariableRefresh() const;

			const string& HeOrShe() const ;
			const string& HisOrHer() const ;
			const string& HimOrHer() const ;


		public:

			mxdir_t			looking;
			mxtime_t		time;
			string			longname;
			string			shortname;
			//charimages_t images;
            
			mxgridref		battleloc;	// change this to CBattle
			u32				battleslew;

			u32				energy;
			u32				reckless;	// READ ONLY

			// lom
			u32				strength;	// READ ONLY
			u32				cowardess;	// READ ONLY
			u32				courage;
			u32				fear;
			u32				RecruitingKey;	// change this to CRecruit
			u32				RecruitedBy;
			// lom

			mxrace_t		race;
			mxobject*		carrying;
			mxobject*		killedbyobject;
			mxgender_t		gender;
			mxwait_t		wait;
			mxmemory		memory;

			mxid			lastcommandid;
			command_t		lastcommand;

			// this should be a list of units
			mxwarriors		warriors;
			mxriders		riders;		

            mxcharacter*    following;
            u32             followers;
            
			// ddr
            mxorders_t      orders;
            mxrace_t		loyalty;
			mxcharacter*	foe;
			mxcharacter*	liege;
#if defined(_DDR_)
            mxgridref       lastlocation;
#endif
			//u32			tiredness - energy
			u32				despondency;
			flags32			traits;
			//

		};
		// mxcharacter

		// army total
		class mxarmytotal : public mxitem
		{
		public:
			mxarmytotal();
			virtual ~mxarmytotal();

            void Clear() { warriors=riders=armies=adjustment=0; character=NULL;}
            
		public:
			u32			warriors ; 
			u32			riders ;
			u32			armies ;
			u32			adjustment ;
			mxcharacter*	character;
		};
		// mxarmytotal

	using namespace tme::collections;
	
		// mxlocinfo
		class mxlocinfo : public mxitem  
		{
		public:
			mxlocinfo();
			virtual ~mxlocinfo();
			
			mxlocinfo( mxgridref loc, mxcharacter* owner, flags32_t flags );
			mxlocinfo( mxgridref loc, mxdir_t looking, mxcharacter* owner, flags32_t flags );

			virtual void FindRecruitCharactersHere ( const mxcharacter* c );
			virtual void FindApproachCharactersInfront ( const mxcharacter* c ) ;
			virtual void FindCharactersHere ( void );
			virtual void FindArmiesHere ( void );
			virtual void WorkOutLocationDetails();

		protected:	
			virtual void Clear();

		public:

			//
			//mxrace_t
			//loyalty_t
			//friendorfoe

			mxcharacter*		owner ;									// the <character> from whom this info block was created for
			flags32				sel_flags ;								// selection flags

			mxdir_t				looking ;								// for what direction? If DR_NONE then the next two vars are not valid
			mxgridref			loc_lookingat ;							// where is the item of interest in the distance?
			mxgridref			loc_infront ;							// location directly infront
			
			mxlocinfo*			infront;								// info block for the location in front
			
			mxloc               mapsqr;									// a copy of the location we are on
			
			// <character> conditionals
			// these are only valid when the <character> != NULL
			flags32				flags;

			// character info
			mxthing_t			fightthing;								// what object is available for fighting

			// scenario specific adjusters
			s32					adj_fear ;								// general fear adjuster
			s32					adj_moral ;								// general moral adjuster
			s32					adj_stronghold ;						// general stronghold adjuster
			u32					nArmies ;								// total armies in this location

			u32					nRecruited ;							// total number of characters in this location already recruited
            
			mxarmytotal			friends;								// info block for friend. ie: Currently NOT doomdark
			mxarmytotal			foe;									// info block for foes ie: Currently IS doomdark
			mxarmytotal			doomdark;								// info block for doomdark's regiments, does not include strongolds
			mxarmy				armies[MAX_ARMIES_INLOCATION];			// list of armies here

			entities            objStrongholds;							// list of strongholds here
			entities            objRoutenodes;							// list of routenodes here
			entities            objCharacters;							// list of characters here
			entities            objRegiments;							// list of regiments here
			entities            objRecruit;								// list of characters that <character> can recruit

            mxcharacter*        stubborn_follower_move;                 // which character is stopping us moving
            mxcharacter*        stubborn_follower_battle;               // which character is stopping us attacking

#if defined(_DDR_)
            mxobject*           object_to_take;
            mxcharacter*        someone_to_give_to;
#endif
            
		};
		// mxlocinfo

		chilli::lib::archive& operator<<( chilli::lib::archive& ar, mxroutenode* node );
		chilli::lib::archive& operator>>( chilli::lib::archive& ar, mxroutenode*& node );
		chilli::lib::archive& operator<<( chilli::lib::archive& ar, mxcharacter* ptr );
		chilli::lib::archive& operator>>( chilli::lib::archive& ar, mxcharacter*& ptr );
		chilli::lib::archive& operator<<( chilli::lib::archive& ar, mxobject* ptr );
		chilli::lib::archive& operator>>( chilli::lib::archive& ar, mxobject*& ptr );
        chilli::lib::archive& operator<<( chilli::lib::archive& ar, mxstronghold* ptr );
        chilli::lib::archive& operator>>( chilli::lib::archive& ar, mxstronghold*& ptr );

		chilli::lib::archive& operator<<( chilli::lib::archive& ar, mxitem* item);
		chilli::lib::archive& operator>>( chilli::lib::archive& ar, mxitem*& item);

	//}
	// namespace item

	chilli::lib::archive& operator<<( chilli::lib::archive& ar, mxentity* ptr );
	chilli::lib::archive& operator>>( chilli::lib::archive& ar, mxentity*& ptr );
	inline chilli::lib::archive& operator<<( chilli::lib::archive& ar, mxgridref& loc )		{ return loc.Serialize(ar); }
	inline chilli::lib::archive& operator>>( chilli::lib::archive& ar, mxgridref& loc )		{ return loc.Serialize(ar); }



}
// namespace tme






#define READ_ENUM(x)	ar >> (u32&)x
#define WRITE_ENUM(x)	ar << (u32)x



#include "processor_text.h"
#include "processor_night.h"
#include "processor_battle.h"





#endif //_INFO_H_INCLUDED_
