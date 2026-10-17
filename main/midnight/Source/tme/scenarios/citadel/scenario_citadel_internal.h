#ifndef _CITADELSCENARIO_INTERNAL_H_INCLUDED_
#define _CITADELSCENARIO_INTERNAL_H_INCLUDED_

#include "../../baseinc/tme_internal.h"
#include "../default/default_scenario_internal.h"

#include <map>
#include <vector>

#if defined(_CITADEL_)

namespace tme {

    #define CITADEL_SCENARIO(x) static_cast<citadel_x*>(mx->scenario)->x

    FORWARD_REFERENCE(citadel_character);
    FORWARD_REFERENCE(citadel_object);
    FORWARD_REFERENCE(citadel_stronghold);

    enum CITADELGAMEFLAGS {
        gf_wraith           = MXBIT(0),     // Boroth has risen again in the Dark Citadel
        gf_free_muster      = MXBIT(1),     // the Free have gathered at Corelay for Luxor
    };

    class citadel_x : public mxscenario
    {
    public:
        citadel_x();
        virtual ~citadel_x();
        
        virtual scenarioinfo_t* GetInfoBlock() const override;
        virtual MXRESULT Register ( mxengine* midnightx ) override;
        virtual MXRESULT UnRegister ( mxengine* midnightx ) override;

        virtual void initialise ( u32 version ) override;
        virtual void initialiseAfterCreate ( u32 version ) override;
        virtual void Serialize ( archive& ar ) override;

        virtual MXRESULT Command ( const std::string& arg, variant argv[], u32 argc ) override;

        virtual mxcharacter* BadGuy() const override;
        virtual bool isTerrainImpassable ( mxterrain_t terrain, const mxitem* target ) const override;
        virtual u32 TerrainMovementModifier ( mxrace_t race, mxterrain_t terrain ) const override;
        virtual void NightStart ( void ) override;
        virtual void NightStop ( void ) override;
        virtual void LordsTurn ( void ) override;
        virtual bool RegimentStep ( const mxregiment* regiment, mxgridref target, mxgridref& step ) const override;
        citadel_object* ArtefactAt ( mxgridref loc ) const;

        mxrace_t RealmUnderAttack () const;
        mxstronghold* FrontlineKeep () const;       // his keep nearest the realm he is taking
        bool BorothFallen () const;         // dead, with no wraith left to rise
        bool IsFoughtOver ( mxstronghold* stronghold ) const;
        bool Borders ( mxrace_t a, mxrace_t b ) const;      // two kingdoms share a border
        const std::vector<s32>& StepsFrom ( mxgridref from, const mxitem* walker ) const;     // march cost to 'from'
        bool MarchStep ( const mxitem* walker, mxgridref here, mxgridref target, mxgridref& step ) const;
        bool Reachable ( const mxitem* walker, mxgridref from, mxgridref to ) const;
        void RaiseWraith ();
        mxstronghold* HomeKeep ( mxrace_t people, mxgridref from ) const;   // the nearest keep his people hold
        mxstronghold* KeepOf ( const mxcharacter* owner ) const;
        std::vector<citadel_character*> HostagesHeldAtMaranor () const;
        citadel_character* KingOf ( mxrace_t realm ) const;
        citadel_character* HostageOf ( mxrace_t realm ) const;
        mxrealmstatus_t RealmStatus ( mxrace_t realm ) const;

        mxrace_t Allegiance ( const mxitem* figure ) const;
        bool AtWar ( mxrace_t side ) const;
        bool AtWar ( mxrace_t side, mxrace_t with ) const;
        bool IsEnemyOf ( mxrace_t who, mxrace_t of ) const;
        void DeclareWar ( mxrace_t on, mxrace_t by );

    public:
        // Boroth the Wolfheart, who holds the Citadel and whose host takes the keeps
        citadel_character*  boroth;
        citadel_character*  luxor;
        citadel_stronghold* maranor;
        flags32             flags;

    private:
        struct pace_t {
            mxrace_t    race;
            bool        onfoot;
        };
        pace_t PaceOf ( const mxitem* walker ) const;
        u32 StepCost ( mxrace_t race, bool onfoot, mxgridref into, mxdir_t dir ) const;
        void ExecuteLuxor ();
        void MusterTheFree ();
        void Scout ();

        static constexpr u32 SCOUTING = 3;
        static constexpr u32 FREE_MUSTER = 5000;    // warriors of the Free who gather at Corelay for Luxor

        // StepsFrom's answers for tonight, by the square they are counted from and the walker's pace
        mutable std::map<u64, std::vector<s32>> paths;
        mutable std::vector<s8> passable;       // tonight's, by square: -1 not yet asked

        std::vector<u64>    wars;
        std::vector<mxgridref> packs;
        std::map<mxrace_t, citadel_character*> hostages;
    };

    class citadel_entityfactory : public mxentityfactory
    {
    public:
        virtual mxentity* Create ( id_type_t type ) override;
        virtual mxroutebranch* CreateRouteBranch () override;
    };

    class citadel_routebranch : public mxroutebranch
    {
    public:
        virtual void SetRoute ( mxdir_t dir, u32 w ) override;
        virtual void Serialize ( archive& ar ) override;

    public:
        mxdir_t     direction = DR_NONE;
        u32         weight = 0;
    };

    class citadel_object : public mxobject
    {
    public:
        citadel_object();

        virtual void Serialize ( archive& ar ) override;
        virtual void LoadTsv ( const TsvRow& row ) override;

        bool IsOnMap () const;
    };

    class citadel_stronghold : public mxstronghold
    {
    public:
        virtual bool IsEnemy() const override;
        virtual void MakeChangeSides ( mxrace_t newrace, mxcharacter* newoccupier ) override;
        virtual bool CanCharacterRecruitOrPost ( const mxcharacter* character ) const override;
        virtual bool CanCharacterPost ( const mxcharacter* character ) const override;
        virtual u32 DefenceMultiplier() const;

        mxrace_t Allegiance () const;
        bool Subjugated () const;
        void Conquer ( mxcharacter* lord );
        void Gather ();
    };

    class citadel_area : public mxarea
    {
    public:
        virtual void Serialize ( archive& ar ) override;
        virtual void LoadTsv ( const TsvRow& row ) override;
        virtual bool Borders ( const mxarea* area ) const;

    public:
        c_mxid neighbours;
    };

    class citadel_race : public mxrace
    {
    public:
        virtual void Serialize ( archive& ar ) override;
        virtual void LoadTsv ( const TsvRow& row ) override;
        virtual bool IsFeudingWith ( mxrace_t race ) const;

    public:
        c_mxid feuds;
    };

    class citadel_character : public mxcharacter
    {
    public:
        PROPERTY(mxquest_t, Quest, quest);
        PROPERTY(mxquestnews_t, QuestNews, questnews);
        PROPERTY(mxid, QuestTarget, questtarget);
        PROPERTY(mxpurpose_t, Purpose, purpose);
        PROPERTY(mxreaction_t, Reaction, reaction);
        PROPERTY(mxstronghold*, Home, homestronghold);
        FLAG_PROPERTY(IsAvailable, cf_available);
        GET_PROPERTY(u32, Idle, idle);
        GET_PROPERTY(bool, InDungeon, (flags.Is(cf_dungeon) && Location() == CITADEL_SCENARIO(maranor)->Location()));
        GET_PROPERTY(bool, QuestIdle, (Quest() == QS_NONE || Quest() == QS_REST));
        GET_PROPERTY(bool, AtHome, (Location() == Home()->Location() || ( IsFollowing() && Following()->IsAlive() )));

        virtual void Serialize ( archive& ar ) override;
        virtual void LoadTsv ( const TsvRow& row ) override;
        virtual bool TakesPartInBattle() const override;
        virtual u32  FightStrength() const override;
        virtual bool ShouldDieInFight() const override;
        virtual void InitNightProcessing ( void ) override;
        virtual bool CheckRecruitChar ( mxcharacter* pChar ) const override;
        virtual s32  RecruitScore ( const mxcharacter* other ) const;
        virtual s32  RecruitThreshold(const mxrace_t race) const ;
        virtual bool IsAllowedWarriors() const override;
        virtual bool IsAllowedRiders() const override;
        virtual bool Recruited ( mxcharacter* recruiter ) override;
        virtual MXRESULT Cmd_WalkForward ( bool seek, bool approach ) override;
        virtual mxobject* Cmd_Seek ( void ) override;
        virtual std::string Title() const override { return title; }
        mxobjpower_t WeaponPower() const;

        bool Marches () const;              // will leave his keeps to fight for them
        bool IsKing () const;
        u32 Rank () const;                  // who leads when two fellowships merge
        bool CanQuest () const;             // free to take any quest at all
        bool IsAwaitingOrders () const;     // alive and free, with no quest in hand
        bool CanQuest ( mxquest_t quest, mxid target ) const;
        bool SetQuest ( mxquest_t quest, mxid target );
        void QuestTargets ( mxquest_t quest, c_mxid& targets ) const;
        
        citadel_character* SearchDungeon ( u32 chance );   // the hostage he finds and frees, if any
        void FlyHome ();
        u32 Might () const;
        mxgridref QuestLocation () const;
        std::string QuestText () const;                     // what he is about, in a sentence
        std::string NewsText () const;                      // what he has to tell you at dawn

        static u32 StrengthOf ( const mxitem* figure );     // a lord's Might, or a regiment's men
        bool IsHostileTo ( const mxitem* figure ) const;    // the figure's allegiance is at war with his own
        bool CanFaceAtOdds ( const mxitem* opponent, u32 a, u32 b ) const;  // the design's odds a:b, in his favour
        void AssignQuest ( mxquest_t quest, mxid target );  // unlike SetQuest, takes any order; leaves a leader he is not rejoining
        void EndQuest ( mxquestnews_t news );               // clears the quest, keeping the target for the dawn news
        void ReportQuestBlocked ();                         // the enemy is in the road
        void AttemptToRecruit ( citadel_character* other ); // wins him over, is refused, or is fought for the insult
        void MergeFellowship ( mxcharacter* leader );       // the higher rank leads
        bool ObtainFrom ( citadel_object* object, mxcharacter* holder );   // persuades or fights the holder for it
        bool StandGuard ();                                 // false if he is not the sort to hold a place
        
    private:
        mxquest_t       quest = QS_NONE;
        mxid            questtarget = IDT_NONE;             // a character, a keep, an object, a regiment or a location id
        mxpurpose_t     purpose = PU_NONE;
        mxreaction_t    reaction = RE_RETURN_HOME;
        mxquestnews_t   questnews = QN_NONE;                // what he has to tell you at dawn
        mxstronghold*   homestronghold = nullptr;           // the keep he defends and goes home to
        u32             idle = 0;                           // nights one of yours has stood waiting for orders
        std::string     title;                              // the design's "Titles"; empty for most lords
    };

    inline citadel_race* CitadelRace ( mxrace_t race )
    {
        return static_cast<citadel_race*>(mx->RaceById(race));
    }

    inline citadel_character* CitadelLord ( mxcharacter* character )
    {
        return static_cast<citadel_character*>(character);
    }

    inline citadel_object* CitadelObject ( mxobject* object )
    {
        return static_cast<citadel_object*>(object);
    }

    inline mxcharacter* CharacterTarget ( mxid target )
    {
        return ID_TYPE(target) == IDT_CHARACTER ? mx->CharacterById(GET_ID(target)) : nullptr;
    }

    inline citadel_stronghold* StrongholdTarget ( mxid target )
    {
        return ID_TYPE(target) == IDT_STRONGHOLD
            ? static_cast<citadel_stronghold*>(mx->StrongholdById(GET_ID(target)))
            : nullptr;
    }

    inline citadel_object* ObjectTarget ( mxid target )
    {
        return ID_TYPE(target) == IDT_OBJECT ? CitadelObject(mx->ObjectById(GET_ID(target))) : nullptr;
    }

    inline mxregiment* RegimentTarget ( mxid target )
    {
        return ID_TYPE(target) == IDT_REGIMENT ? mx->RegimentById(GET_ID(target)) : nullptr;
    }
}
#endif

#endif //_CITADELSCENARIO_INTERNAL_H_INCLUDED_

