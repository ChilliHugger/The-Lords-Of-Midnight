#ifndef _INFO_H_INCLUDED_
#define _INFO_H_INCLUDED_

#include "../../library/inc/library.h"
#include <memory>

#define MAX_CHARACTERS_FOLLOWING        32

#if defined(_DDR_)
constexpr int MAX_CHARACTERS_INLOCATION = 32;
constexpr int MAX_REGIMENTS_INLOCATION = 64;
constexpr int MAX_FOEARMIES_INLOCATION = 32;
constexpr int MAX_FRIENDARMIES_INLOCATION = 32;
#else
constexpr int MAX_CHARACTERS_INLOCATION = 128;
constexpr int MAX_REGIMENTS_INLOCATION = 128;
constexpr int MAX_FOEARMIES_INLOCATION = 128;
constexpr int MAX_FRIENDARMIES_INLOCATION = 128;
#endif

//constexpr int MAX_ARMIES_INLOCATION = (MAX_FOEARMIES_INLOCATION+MAX_FRIENDARMIES_INLOCATION);
constexpr int MAX_STRONGHOLDS_INLOCATION = 1;
constexpr int MAX_ROUTENODES_INLOCATION = 1;
constexpr int MAX_CHARACTERS_IN_SMALL_TUNNEL = 4;

class CListS;

#include "variables.h"
#include "../base/collections.h"



// enum control
//#define DECLARE_ENUM(x)        namespace x { enum
//#define END_ENUM(x)            }; }


#define DEFAULT_IMPLEMENTATION(x) \
        x(void); \
        virtual ~x(void); \
        virtual void Serialize ( chilli::lib::archive& ar ); \
        virtual MXRESULT FillExportData ( info_t* data )


namespace tme {

    using namespace chilli;
    using namespace chilli::lib;
    using namespace variables;
    using namespace flags;
    
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
            class mxgameover;
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

        static u32 SafeId(const mxentity* entity) ;
        static mxid SafeIdt(const mxentity* entity);

        PROPERTY( id_type_t, IdType, idType );
        PROPERTY( u32, Id, id );

        std::string& Symbol() { return symbol ; }
        bool IsSymbol(LPCSTR s) const { return c_stricmp(symbol.c_str(),s) == 0; }

        virtual int Compare ( const mxentity* o, int hint ) const;

        void SetUserData(const void* data) { user_data = data; }
        const void* GetUserData() { return user_data; }

        flags32& Flags() { return flags; }
        bool IsFlags(u32 f) { return flags.Is(f); }
        bool IsType(id_type_t type) const { return idType == type; }
        FLAG_PROPERTY( IsDisabled,    et_disabled )

    protected:
        id_type_t               idType;
        u32                     id;
        std::string             symbol;
        flags32                 flags;
        const void*             user_data ;

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
        mxfragment*             m_Prev;
        mxfragment*             m_Next;
        u32                     m_day;
        mxentity*               m_data;
        mxgridref               m_location;
        mxrace_t                m_race;
        
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
        mxfragment*             m_first;

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
            const std::string& Name() const;

        protected:
            friend class    mxcharacter ;
            mxunit_t            type;
            u32                 total;
            u32                 energy;
            u32                 lost;
            u32                 slew;
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

inline chilli::lib::archive& operator<<( chilli::lib::archive& ar, mxunit& unit )        { return unit.Serialize(ar); }
inline chilli::lib::archive& operator>>( chilli::lib::archive& ar, mxunit& unit )        { return unit.Serialize(ar); }

    //}
    // namespace units


//#pragma push    
//#pragma pack ( 1 )
    
#define MAP_FLAG_PROPERTY( n, f ) \
    bool n() const { return (flags&f) == f; }
    
    // class loc
    class mxloc
    {
    public:
        mxloc();
        ~mxloc();
        
        void RemoveObject()                 { object = 0; }
        MAP_FLAG_PROPERTY( IsInDomain,      lf_domain)
        MAP_FLAG_PROPERTY( IsSpecial,       lf_special)
        MAP_FLAG_PROPERTY( IsVisible,       lf_seen)
        MAP_FLAG_PROPERTY( IsMisty,         lf_mist)
        MAP_FLAG_PROPERTY( IsStronghold,    lf_stronghold)
        MAP_FLAG_PROPERTY( IsRouteNode,     lf_routenode)
        MAP_FLAG_PROPERTY( IsImpassable,    lf_impassable)

        MAP_FLAG_PROPERTY( IsTunnelVisible, lf_tunnel_looked_at)
        MAP_FLAG_PROPERTY( HasTunnel,       lf_tunnel)
        MAP_FLAG_PROPERTY( IsSmallTunnel,   lf_tunnel_small)
        bool HasTunnelExit() const ;
        bool HasTunnelEntrance() const ;
        bool IsTunnelObject() const ;

#if defined(_DDR_)
        MAP_FLAG_PROPERTY( HasObject,       lf_object)
#endif
        MAP_FLAG_PROPERTY( HasArmy,         lf_army)
        MAP_FLAG_PROPERTY( HasCharacter,    lf_character)
        
        bool IsInteresting() const ;
        void Serialize ( chilli::lib::archive& ar ) ;
        
    public:
        u64 terrain             : 7 ; // 128 ( 16x8 )
        u64 variant             : 3 ; // 8 ( 8 )
        u64 climate             : 7 ; // 128 ( 16x8 )
        u64 object              : 9 ; // 512 ( 16x8 )
        u64 area                : 10 ; // 1024 ( 64x16 )
        u64 flags               : 28 ; // ( lf_domain|lf_mist|lf_tunnel )
    };
    // class mxloc
//#pragma pop
    
    
    
    // class mxmap
    class mxmap
    {
    public:

        // landscaping info table structure
        typedef struct landscape_t{
            s8                  dy;
            s8                  dx;
            s16                 xadj;
            u8                  y;
            u8                  size;
        } landscape_t;


    public:
        mxmap();
        mxmap( u32 x, u32 y );
        virtual ~mxmap();

        virtual void Serialize ( chilli::lib::archive& ar ) ;

        bool Load ( const std::string& filename );
        bool Save ( const std::string& filename );
        bool IsLocationSpecial( mxgridref loc );
        bool IsLocationBlock( mxgridref loc ) ;
        bool IsLocOnMap ( mxgridref loc ) const ;
        bool IsLocationVisible( mxgridref loc );
        
        void SetLocationArmy( mxgridref loc, u32 number );
        void SetLocationCharacter( mxgridref loc, u32 number );
        void SetLocationSpecial( mxgridref loc, u32 number );
        void SetLocationVisible( mxgridref l, bool visible );
        void SetLocationLookedAt( mxgridref l, bool visible );
        void SetLocationVisited( mxgridref l, bool visible );

        bool HasTunnelEntrance( mxgridref l );
        bool HasTunnelExit( mxgridref l );
        void SetTunnelVisible( mxgridref l, bool visible );
        bool IsTunnelVisible( mxgridref loc );
        bool IsTunnel( mxgridref loc );
        bool IsTunnelObject( mxgridref l );

#if defined(_DDR_)
        bool HasObject( mxgridref l );
        void SetObject( mxgridref l, bool value );
        void MoveMists ( void );
        void PutThingsOnMap ( void );
#endif
        mxthing_t getLocationObject( const mxcharacter* c, mxgridref loc );

        void ResetVisibleRange();
        void CheckVisibleRange( mxgridref l );
        
        u32 GetPanoramic ( mxgridref loc, mxdir_t dir, panloc_t** locations );

        mxloc& GetAt ( const mxgridref& loc );
        mxloc& operator[] ( mxgridref loc ) { return GetAt(loc); }

        GET_PROPERTY ( size, Size, m_size );
        GET_PROPERTY ( loc_t, Top, m_top_visible );
        GET_PROPERTY ( loc_t, Bottom, m_bottom_visible );
        
        u8 Density(mxgridref loc)  ;

        void ClearVisible();
        void CalculateVisibleArea();
        
    private:
        void Pan_Location ( mxgridref loc, int distance, int xtype, int ytype, int scrtype ) ;
        int Pan_AdjustMapPos ( int pos, int type, int dx, int dy );
        bool Create ( size dimensions );

    
    public:
        u32                     m_version;
        size                    m_size;
        mxloc*                  m_data;
        flags32                 m_flags;
        loc_t                   m_top_visible;
        loc_t                   m_bottom_visible;
        // ptr to the current discovery map
        mxdiscoverymap*         m_discoverymap;

    }; // class mxmap

    class mxdiscoverymap
    {
    public:
        mxdiscoverymap();
        mxdiscoverymap( u32 x, u32 y );
        
        virtual ~mxdiscoverymap();
        virtual void Serialize ( chilli::lib::archive& ar ) ;
        
        void CalculateVisibleArea();
        bool Create ( size dimensions );
        
        void ClearVisibleArea();
        void ResetVisibleArea();
        bool Load ( const std::string& filename );
        bool Save ( const std::string& filename );
        
        bool IsLocationVisible( mxgridref l );
        flags32& GetAt ( const mxgridref& mxloc );
        bool IsLocOnMap ( mxgridref mxloc ) const;
        
        
        void SetLocationVisible( mxgridref l, bool visible );
        void SetLocationLookedAt( mxgridref l, bool visible );
        void SetLocationVisited( mxgridref l, bool visible );
        void CheckVisibleArea( mxgridref l );
                
        bool IsTunnelVisible( mxgridref l );
        void SetTunnelVisible( mxgridref l, bool visible );
        
        void TransferFromMap( mxmap* map );
        
        
        GET_PROPERTY ( size, Size, m_size );
        GET_PROPERTY ( loc_t, Top, m_top_visible );
        GET_PROPERTY ( loc_t, Bottom, m_bottom_visible );
        
    public:
        size                    m_size;
        flags32*                m_data;
        loc_t                   m_top_visible;
        loc_t                   m_bottom_visible;
        
    }; // class mxdiscoverymap
    
        
    //namespace info {
        // base
        class mxinfo : public mxentity
        {
        public:
            DEFAULT_IMPLEMENTATION(mxinfo);

            std::string& Name() { return name ; }

        protected:
            std::string    name;

        }; // base


        // command
        class mxcommand : public mxinfo
        {
        public:
            DEFAULT_IMPLEMENTATION(mxcommand);

            s32 SuccessTime() const ;

        public:
            s32                 successtime;
            s32                 failuretime;
        };
        typedef tme::collections::infos<mxcommand*>          c_command;
        // command

        // area
        class mxarea : public mxinfo
        {
        public:
            DEFAULT_IMPLEMENTATION(mxarea);

        public:
            std::string               prefix;
        };
        typedef tme::collections::infos<mxarea*>             c_area;
        // area

        // direction
        class mxdirection : public mxinfo
        {
        public:
            DEFAULT_IMPLEMENTATION(mxdirection);

        protected:

        };
        typedef tme::collections::infos<mxdirection*>          c_direction;
        // direction

        // gender
        class mxgender : public mxinfo
        {
        public:
            DEFAULT_IMPLEMENTATION(mxgender);

        public:
            std::string         pronoun1;
            std::string         pronoun2;
            std::string         pronoun3;
        };
        typedef tme::collections::infos<mxgender*>           c_gender;
        // gender

        // race
        class mxrace : public mxinfo
        {
        public:
            DEFAULT_IMPLEMENTATION(mxrace);

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

            f64 RidingMovementMultiplier() const    { return (f64)ridingmultiplier/10000.0; }
            
            std::string& SoldiersName();

        protected:
            std::string         soldiersname;
            u32                 success;
            u32                 initialmovement;
            s32                 diagonalmodifier;
            s32                 ridingmultiplier;
            u32                 movementmax;
            u32                 baserestamount;
            u32                 strongholdstartups;

            s32                 misttimeaffect ;
            s32                 mistdespondecyaffect ;
            s32                 baseenergycost ;
            s32                 baseenergycosthorse ;
        };
        typedef tme::collections::infos<mxrace*>             c_race;
        // race

        // terrain
        class mxterrain : public mxinfo
        {
        public:
            DEFAULT_IMPLEMENTATION(mxterrain);
            
            mxterrain( mxterrain_t id, LPCSTR symbol );

            FLAG_PROPERTY ( IsPlural, tif_plural )
            FLAG_PROPERTY ( IsBlock, tif_block )
            FLAG_PROPERTY ( IsInteresting, tif_interesting )
            FLAG_PROPERTY ( IsArmyVisible, tif_army )

            GET_PROPERTY ( u32, Success, success )
            GET_PROPERTY ( u32, Visibility, visibility )
            GET_PROPERTY ( u32, Obstruction, obstruction )
            GET_PROPERTY ( s32, MovementCost, movementcost )

            std::string& Preposition() { return preposition; }
            std::string& Description() { return description; }


        public:
            std::string         preposition;
            std::string         description;
            u32                 success;
            u32                 visibility;
            u32                 obstruction;
            s32                 movementcost;
        };
        typedef tme::collections::infos<mxterrain*>          c_terrain;
        // terrain

        // unit
        class mxunitinfo : public mxinfo
        {
        public:
            DEFAULT_IMPLEMENTATION(mxunitinfo);

            GET_PROPERTY ( u32, Success, success )
            GET_PROPERTY ( u32, BaseRestModifier, baserestmodifier )

        protected:
            u32                 success;
            u32                 baserestmodifier;
                        
            friend class lom_x;
        };
        typedef tme::collections::infos<mxunitinfo*>         c_unit;
        // unit
    
#if defined(_DDR_)
        // object_power
        class mxobjectpower : public mxinfo
        {
        public:
            DEFAULT_IMPLEMENTATION(mxobjectpower);
            
        protected:
            
        };
        typedef tme::collections::infos<mxobjectpower*>      c_objectpower;
        // objectpower

        // objecttype
        class mxobjecttype : public mxinfo
        {
        public:
            DEFAULT_IMPLEMENTATION(mxobjecttype);
            
        protected:
            
        };
        typedef tme::collections::infos<mxobjecttype*>       c_objecttype;
        // objecttype
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
            enum SORT {
                PRIORITY = 1
            };
        public:
            DEFAULT_IMPLEMENTATION(mxvictory);

            FLAG_PROPERTY ( IsGameOver, vf_gameover );
            FLAG_PROPERTY ( IsComplete, vf_complete );

            GET_PROPERTY ( u32, Message, string );
            GET_PROPERTY ( s32, Priority, priority );

            virtual bool CheckComplete ( void );
            virtual int Compare ( const mxentity* o, int hint ) const;

        protected:
            s32                 priority;
            mxid                mission;
            u32                 string;
        };
        typedef tme::collections::entities<mxvictory*>       c_victory;
        // victory


        // mission
        class mxmission : public mxentity
        {
        public:
            enum SORT {
                PRIORITY = 1
            };

        public:
            DEFAULT_IMPLEMENTATION(mxmission);

            FLAG_PROPERTY ( IsComplete, mf_complete );
            FLAG_PROPERTY ( IsAny, mf_any );
            GET_PROPERTY ( s32, Priority, priority );
            GET_PROPERTY ( m_condition_t, Condition, condition );
            
            virtual bool CheckComplete ( void );
            virtual bool Completed ( void );
            virtual mxgridref GetLocation ( mxid id ) const;
            virtual int Compare ( const mxentity* o, int hint ) const;

        protected:
            s32                 priority;
            m_objective_t       objective;
            m_condition_t       condition;
            mxid                reference[5];
            s32                 points;
            mxid                scorer;
            m_action_t          action;
            mxid                actionid;
        };
        typedef tme::collections::entities<mxmission*>       c_mission;
        // mission


        // mxarmy
        class mxarmy  
        {
        public:
            mxarmy();
            ~mxarmy();

        public:
            mxitem*             parent;
            mxarmytype_t        armytype;
            mxrace_t            race;
            mxunit_t            type;
            u32                 total;
            u32                 success;
            u32                 killed;
            mxrace_t            loyalto;
        };
        // mxarmy

        typedef std::vector<mxarmy*>     c_army ;
      
        // base
        class mxitem  : public mxentity
        {
        public:
            DEFAULT_IMPLEMENTATION(mxitem);

            mxitem( mxgridref loc );

            PROPERTY( mxgridref, Location, location );

        protected:
            mxgridref            location;
        };
        // base


        // mxobject
        class mxobject : public mxitem
        {
        public:
            DEFAULT_IMPLEMENTATION(mxobject);

            FLAG_PROPERTY( CanFight,    of_fight )
            FLAG_PROPERTY( CanPickup,   of_pickup )
            FLAG_PROPERTY( CanDrop,     of_drop )
            FLAG_PROPERTY( CanSee,      of_see )
            FLAG_PROPERTY( IsWeapon,    of_weapon )
            FLAG_PROPERTY( MapRemove,   of_remove )
            FLAG_PROPERTY( IsUnique,    of_unique )
            
            bool IsCarried() const ;

            bool CanDestroy ( mxobject* obj ) const ;
            
            std::string& Name()          { return name ; }

        public:
            mxthing_t           kills;
            std::string         name;
            std::string         description;
            u32                 usedescription;
            mxitem*             carriedby;
        };
        typedef tme::collections::entities<mxobject*>        c_object;
        // object


        // mxstronghold
        class mxstronghold  : public mxitem
        {
        public:
            DEFAULT_IMPLEMENTATION(mxstronghold);

            virtual s32 BattleSuccess ( const mxlocinfo& locinfo );
            virtual u32 Remove ( mxrace_t race, mxunit_t type, u32 total );
            virtual u32 Add ( mxrace_t race, mxunit_t type, u32 total );
            virtual void MakeChangeSides( mxrace_t race, mxcharacter* newoccupier );
            virtual mxrace_t Loyalty () const;
            virtual bool IsFriend( const mxcharacter* character ) const;
        
            virtual bool CanCharacterRecruitOrPost(const mxcharacter* character) const;

            void CheckForZero ( void );

            PROPERTY( u32, TotalTroops, totaltroops );
            PROPERTY( u32, Killed, killed );
            PROPERTY( u32, Lost, lost );
            PROPERTY( u32, Respawn, respawn );
            
            GET_PROPERTY ( mxrace_t, Race, race )
            GET_PROPERTY ( mxrace_t, OccupyingRace, occupyingrace )
            GET_PROPERTY ( mxunit_t, Type, type )
            GET_PROPERTY ( mxterrain_t, Terrain, terrain )
            GET_PROPERTY ( mxcharacter*, Owner, owner )
            GET_PROPERTY ( mxcharacter*, Occupier, occupier )
            PROPERTY ( u32, MinTroops, mintroops )
            PROPERTY ( u32, MaxTroops, maxtroops )
            GET_PROPERTY ( u32, Influence, influence )
            bool HasFallen() const { return race!=occupyingrace; }

        protected:
            mxrace_t            occupyingrace;
            mxrace_t            race;
            mxunit_t            type;
            u32                 totaltroops;
            u32                 mintroops;
            u32                 maxtroops;
            u32                 strategical_success;
            u32                 owner_success;
            u32                 enemy_success;
            u32                 influence;
            u32                 respawn;
            mxcharacter*        occupier;
            mxcharacter*        owner;
            mxterrain_t         terrain;
            u32                 killed;
            u32                 lost;
        };
        typedef tme::collections::entities<mxstronghold*>    c_stronghold;
        // mxstronghold

        // mxroutenode
        class mxroutenode  : public mxitem
        {
        public:
            DEFAULT_IMPLEMENTATION(mxroutenode);

            GET_PROPERTY ( mxroutenode*, Left, paths[0] )
            GET_PROPERTY ( mxroutenode*, Right, paths[1] )

        protected:

            mxroutenode*        paths[2];
        };
        typedef tme::collections::entities<mxroutenode*>     c_routenode;
        // routenode


        // mxplace
        class mxplace : public mxitem
        {
        public:
            DEFAULT_IMPLEMENTATION(mxplace);

        };
        typedef tme::collections::entities<mxplace*>         c_place;
        // mxplace


        // mxregiment
        class mxregiment  : public mxitem
        {
        public:
            DEFAULT_IMPLEMENTATION(mxregiment);

            virtual s32 BattleSuccess ( const mxlocinfo& locinfo );

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
            PROPERTY( mxgridref, LastLocation, lastlocation );

        protected:
            mxrace_t        race;
            mxunit_t        type;
            u32             total;
            mxid            targetid;
            mxorders_t      orders;
            u32             delay;
            u32             success;
            u32             killed;
            u32             lost;
            mxcharacter*    loyalty;
            mxgridref       lastlocation;
        };
        typedef tme::collections::entities<mxregiment*>      c_regiment;
        // mxregiment
        
        // mxcharacter
        class mxcharacter  : public mxitem
        {
        public:
            DEFAULT_IMPLEMENTATION(mxcharacter);

            virtual std::unique_ptr<mxlocinfo> GetLocInfo();
            virtual s32 DistanceFromLoc ( mxgridref loc );

            flags32& Traits()    { return traits; } 

            GET_PROPERTY ( mxobject*, Carrying, carrying )
            GET_PROPERTY ( bool, IsCarryingObject, carrying != nullptr )
            
            GET_PROPERTY ( mxwait_t, WaitMode, wait )
            GET_PROPERTY ( mxtime_t, Time, IsResting()?sv_time_night:time )
            GET_PROPERTY ( mxdir_t, Looking, looking )
            GET_PROPERTY ( mxrace_t, Race, race )
            //GET_PROPERTY ( charimages_t, Images, images )
            GET_PROPERTY ( bool, IsWaiting, WaitMode()!=WM_NONE )
            GET_PROPERTY ( bool, IsNight, Time()==(mxtime_t)sv_time_night||IsResting() )
            GET_PROPERTY ( bool, IsDawn, Time()==(mxtime_t)sv_time_dawn&&!IsResting() )
            GET_PROPERTY ( bool, IsFollowing, Following()!=NULL )

            GET_PROPERTY ( mxrace_t, Loyalty, loyalty )
            GET_PROPERTY ( mxcharacter*, Foe, foe )
            GET_PROPERTY ( mxcharacter*, Liege, liege )
            GET_PROPERTY ( mxcharacter*, Following, following )
            GET_PROPERTY ( bool, IsCoward, courage == 0 || traits.Is(ct_coward) )

            FLAG_PROPERTY ( IsHidden, cf_hidden )
            FLAG_PROPERTY ( IsRiding, cf_riding )
            FLAG_PROPERTY ( IsAlive, cf_alive )
            FLAG_PROPERTY ( IsRecruited, cf_recruited )
            FLAG_PROPERTY ( IsAllowedArmy, cf_army )
            FLAG_PROPERTY ( IsAIControlled, cf_ai )

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
            FLAG_PROPERTY ( IsBattleOver, cf_battleover )
            
            virtual bool IsAllowedWarriors() const { return IsAllowedArmy(); }
            virtual bool IsAllowedRiders() const { return IsAllowedArmy(); }
            
            bool HasArmy() const { return ((riders.total+warriors.total) > 0); }
            bool IsDead() const { return !flags.Is(cf_alive); }
            const std::string& Longname() const { return longname; }
            const std::string& Shortname() const { return shortname; }

            void SetLastCommand ( command_t cmd, mxid id );
            void CommandTakesTime ( bool success );
            s32 CommandSuccessTime( command_t cmd );

            // Commands
            virtual MXRESULT Cmd_LookLeft ( void );
            virtual MXRESULT Cmd_LookRight ( void );
            virtual MXRESULT Cmd_LookDir ( mxdir_t dir );
            virtual MXRESULT Cmd_MoveForward ( void );
            virtual MXRESULT Cmd_WalkForward ( bool seek = false, bool approach = false );
            virtual MXRESULT Cmd_PostMen ( mxstronghold* s=NULL, u32 qty=0 );
            virtual MXRESULT Cmd_RecruitMen ( mxstronghold* s=NULL, u32 qty=0 );
            virtual MXRESULT Cmd_Attack ( void );
            virtual MXRESULT Cmd_Rest ( void );
            virtual MXRESULT Cmd_Hide ( void );
            virtual MXRESULT Cmd_UnHide ( void );
            virtual MXRESULT Cmd_Dismount ( void );
            virtual MXRESULT Cmd_Lookat(mxgridref loc);
            virtual MXRESULT Cmd_DropObject ( void );
            virtual MXRESULT Cmd_Wait ( mxwait_t mode );

            virtual MXRESULT Cmd_Follow ( mxcharacter* c );
            virtual MXRESULT Cmd_UnFollow ( mxcharacter* c  );
            
            virtual MXRESULT Cmd_DisbandGroup ( void );
            virtual MXRESULT Cmd_SwapGroupLeader ( mxcharacter* character );
            virtual MXRESULT Cmd_Place(mxgridref loc);
            
            virtual mxcharacter* Cmd_Approach ( mxcharacter* character=NULL );
            virtual mxobject* Cmd_Fight ( void );
            virtual mxobject* Cmd_Seek ( void );
            virtual mxobject* Cmd_PickupObject ( void );
            virtual void Cmd_Dead ( void );
            
            virtual MXRESULT Cmd_EnterTunnel ( void );
            virtual MXRESULT Cmd_ExitTunnel ( void );
            
            virtual MXRESULT EnterBattle ( void );

            virtual bool EnterLocation ( mxgridref loc );
            virtual void CanSeeLocation ( mxgridref loc );
            virtual bool CanFollow( const mxcharacter* c ) const ;
            virtual bool CheckFightObject ( mxobject* object ) const;
            virtual bool CheckRecruitChar ( mxcharacter* pChar ) const ;
            virtual bool Recruited ( mxcharacter* recruiter );
            virtual void SetRecruitmentTime( mxcharacter* recruiter );
            virtual void LostFight ( s32 hint = 0 );
            virtual void LostBattle ( bool canFlee );
            virtual void Displace ( void );
            virtual void DecreaseEnergy ( s32 amount );
            virtual void IncreaseEnergy ( s32 amount );
            virtual void InitNightProcessing ( void );
            virtual bool HasBattleInfo() const ;
            virtual mxthing_t LocationThing() const ;
            virtual void RefreshLocationBasedVariables ( s32 icefear ) ;
            virtual bool CanWalkForward ( void );
            virtual bool ShouldLoseHorse(s32 hint = 0) const;
            virtual bool ShouldDieInFight() const;
            virtual bool ShouldHaveOneToOneWithNasty() const;
            virtual void Dismount();
            virtual void CheckPerformSeek(bool seek, bool tunnelexit);
            virtual mxobject* FoundNothing() const ;
    
            virtual bool AddFollower ( mxcharacter* c );
            virtual bool RemoveFollower ( mxcharacter* c );
            virtual void WalkFollowersForward();

            //
            mxrace_t NormalisedLoyalty() const;
            bool IsFriend( const mxcharacter* c ) const;
  
            void ForcedVariableRefresh() const;

            const std::string& HeOrShe() const ;
            const std::string& HisOrHer() const ;
            const std::string& HimOrHer() const ;

            void ForEachFollower(const std::function<void(mxcharacter*)> &callback);
            mxcharacter* FindFollower(const std::function<bool(mxcharacter*)> &callback);
            
        public:

            mxdir_t             looking;
            mxtime_t            time;
            std::string         longname;
            std::string         shortname;
            //charimages_t images;
            
            mxgridref           battleloc;    // change this to CBattle
            u32                 battleslew;

            u32                 energy;
            u32                 reckless;    // READ ONLY

            // lom
            u32                 strength;    // READ ONLY
            u32                 cowardess;    // READ ONLY
            u32                 courage;
            u32                 fear;
            u32                 RecruitingKey;    // change this to CRecruit
            u32                 RecruitedBy;
            // lom

            mxrace_t            race;
            mxobject*           carrying;
            mxobject*           killedbyobject;
            mxgender_t          gender;
            mxwait_t            wait;
            mxmemory            memory;

            mxid                lastcommandid;
            command_t           lastcommand;

            // this should be a list of units
            mxwarriors          warriors;
            mxriders            riders;

            mxcharacter*        following;
            u32                 followers;
            
            // ddr
            mxorders_t          orders;
            mxrace_t            loyalty;
            mxcharacter*        foe;
            mxcharacter*        liege;

            u32                 despondency;
            flags32             traits;
            //

        };
        typedef tme::collections::entities<mxcharacter*>     c_character;
        
        #define FOR_EACH_FOLLOWER(x) \
            ForEachFollower([&](mxcharacter* x)
        
        #define FIND_FOLLOWER(x) \
            FindFollower([&](mxcharacter* x)
        

        // mxcharacter

        // army total
        class mxarmytotal : public mxitem
        {
        public:
            mxarmytotal();
            virtual ~mxarmytotal();

            void Clear() { warriors=riders=armies=adjustment=characters=0;}
            u32 Total() const { return warriors + riders; } 
            
        public:
            u32                 warriors ;
            u32                 riders ;
            u32                 armies ;
            u32                 adjustment ;
            u32                 characters;
        };
        // mxarmytotal

    
        // mxlocinfo
        class mxlocinfo : public mxitem  
        {
        public:
            mxlocinfo();
            virtual ~mxlocinfo();
            
            mxlocinfo( mxgridref loc, const mxcharacter* owner, flags32_t flags );
            mxlocinfo( mxgridref loc, mxdir_t looking, const mxcharacter* owner, flags32_t flags );

            virtual void FindRecruitCharactersHere();
            virtual void FindApproachCharactersInfront() ;
            virtual void FindCharactersHere();
            virtual void FindArmiesHere();
            virtual void WorkOutLocationDetails();
            
        protected:    
            virtual void Clear();

        public:

            //
            //mxrace_t
            //loyalty_t
            //friendorfoe

            const mxcharacter*  owner ;         // the <character> from whom this info block was created for
            flags32             sel_flags ;     // selection flags

            mxdir_t             looking ;       // for what direction? If DR_NONE then the next two vars are not valid
            mxgridref           loc_lookingat ; // where is the item of interest in the distance?
            mxgridref           loc_infront ;   // location directly infront
            bool                tunnel;         // if there is an owner then we are considering a tunnel
            
            std::unique_ptr<mxlocinfo> infront; // info block for the location in front
            
            mxloc               mapsqr;         // a copy of the location we are on
            
            // <character> conditionals
            // these are only valid when the <character> != NULL
            flags32             flags;

            // character info
            mxthing_t           fightthing;     // what object is available for fighting

            // scenario specific adjusters
            s32                 adj_fear ;      // general fear adjuster
            s32                 adj_moral ;     // general moral adjuster
            s32                 adj_stronghold ;// general stronghold adjuster
            u32                 nArmies ;       // total armies in this location

            u32                 nRecruited ;    // total number of characters in this location already recruited
            
            mxarmytotal         friends;        // info block for friend. ie: Currently NOT doomdark
            mxarmytotal         foe;            // info block for foes ie: Currently IS doomdark
            mxarmytotal         regiments;      // info block for doomdark's regiments, does not include strongolds

            c_army              armies;
            
            c_stronghold        objStrongholds;         // list of strongholds here
            c_routenode         objRoutenodes;          // list of routenodes here
            c_character         objCharacters;          // list of characters here
            c_regiment          objRegiments;           // list of regiments here
            c_character         objRecruit;             // list of characters that <character> can recruit

            
            const mxcharacter*  stubborn_follower_move;     // which character is stopping us moving
            const mxcharacter*  stubborn_follower_battle;   // which character is stopping us attacking

#if defined(_DDR_)
            const mxobject*     object_to_take;
            const mxcharacter*  someone_to_give_to;
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
    inline chilli::lib::archive& operator<<( chilli::lib::archive& ar, mxgridref& loc )        { return loc.Serialize(ar); }
    inline chilli::lib::archive& operator>>( chilli::lib::archive& ar, mxgridref& loc )        { return loc.Serialize(ar); }
}
// namespace tme






#define READ_ENUM(x)    ar >> (u32&)x
#define WRITE_ENUM(x)    ar << (u32)x



#include "processor_text.h"
#include "processor_night.h"
#include "processor_battle.h"





#endif //_INFO_H_INCLUDED_
