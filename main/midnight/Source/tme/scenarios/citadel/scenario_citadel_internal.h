#ifndef _CITADELSCENARIO_INTERNAL_H_INCLUDED_
#define _CITADELSCENARIO_INTERNAL_H_INCLUDED_

#include "../../baseinc/tme_internal.h"
#include "../default/default_scenario_internal.h"

#include <map>
#include <vector>

#if defined(_CITADEL_)

namespace tme {
    FORWARD_REFERENCE(citadel_character);

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

        virtual MXRESULT Command ( const std::string& arg, variant argv[], u32 argc ) override;
        virtual MXRESULT Text ( const std::string& arg, variant argv[], u32 argc ) override;

        virtual mxcharacter* BadGuy() const override;
        virtual bool isTerrainImpassable ( mxterrain_t terrain, const mxitem* target ) const override;
        virtual u32 TerrainMovementModifier ( mxrace_t race, mxterrain_t terrain ) const override;
        virtual void NightStart ( void ) override;
        virtual void NightStop ( void ) override;
        virtual void LordsTurn ( void ) override;
        virtual bool RegimentStep ( const mxregiment* regiment, mxgridref target, mxgridref& step ) const override;
        virtual mxobject* FindObjectAtLocation ( mxgridref loc ) override;
        virtual mxobject* PickupObject ( mxgridref loc ) override;
        virtual bool DropObject ( mxgridref loc, mxobject* object ) override;

        mxrace_t CampaignTarget () const;
        bool IsFoughtOver ( mxstronghold* stronghold ) const;
        bool Borders ( mxrace_t a, mxrace_t b ) const;      // two kingdoms share a border
        const std::vector<s32>& StepsFrom ( mxgridref from, const mxregiment* walker ) const;
        bool MarchStep ( mxgridref here, mxgridref target, mxgridref& step ) const;

    public:
        // Boroth the Wolfheart, who holds the Citadel and whose host takes the keeps
        mxcharacter*    boroth;

    private:
        // StepsFrom's answers for tonight, by the square they are counted from
        mutable std::map<u32, std::vector<s32>> paths;
    };

    class citadel_entityfactory : public mxentityfactory
    {
    public:
        virtual mxentity* Create ( id_type_t type ) override;
    };

    class citadel_object : public mxobject
    {
    public:
        citadel_object();

        virtual void Serialize ( archive& ar ) override;
        virtual void LoadTsv ( const TsvRow& row ) override;

    public:
        mxobjtype_t     type;
        mxobjpower_t    power;
    };

    class citadel_stronghold : public mxstronghold
    {
    public:
        virtual bool IsEnemy() const override;
        virtual void MakeChangeSides ( mxrace_t newrace, mxcharacter* newoccupier ) override;
        virtual bool CanCharacterRecruitOrPost ( const mxcharacter* character ) const override;
        virtual bool CanCharacterPost ( const mxcharacter* character ) const override;
        virtual u32 DefenceMultiplier() const;
    };

    class citadel_area : public mxarea
    {
    public:
        virtual void Serialize ( archive& ar ) override;
        virtual void LoadTsv ( const TsvRow& row ) override;
        virtual bool Borders ( const mxarea* area ) const;

    public:
        std::vector<u32>    neighbours;     // area ids
    };

    class citadel_race : public mxrace
    {
    public:
        virtual void Serialize ( archive& ar ) override;
        virtual void LoadTsv ( const TsvRow& row ) override;
        virtual bool IsFeudingWith ( mxrace_t race ) const;

    public:
        std::vector<mxrace_t>   feuds;
    };

    class citadel_character : public mxcharacter
    {
    public:
        virtual void Serialize ( archive& ar ) override;
        virtual void LoadTsv ( const TsvRow& row ) override;
        virtual bool TakesPartInBattle() const override;
        virtual u32  FightStrength() const override;
        virtual bool ShouldDieInFight() const override;
        virtual void InitNightProcessing ( void ) override;
        virtual bool CheckRecruitChar ( mxcharacter* pChar ) const override;
        virtual s32  RecruitScore ( const mxcharacter* other ) const;
        virtual s32  RecruitThreshold(const mxrace_t race) const;
        virtual bool IsAllowedWarriors() const override;
        virtual bool IsAllowedRiders() const override;
        virtual bool Recruited ( mxcharacter* recruiter ) override;
        virtual std::string Title() const override { return title; }
        mxobjpower_t WeaponPower() const;

        bool CanQuest ( mxquest_t quest, mxid target ) const;
        bool SetQuest ( mxquest_t quest, mxid target );
        mxgridref QuestLocation () const;
        std::string QuestText () const;     // what he is about, in a sentence
        std::string NewsText () const;      // what he has to tell you at dawn

    public:
        mxquest_t       quest = QS_NONE;
        mxid            questtarget = IDT_NONE;     // a character, a keep, an object, a regiment or a location id
        mxpurpose_t     purpose = PU_NONE;
        mxreaction_t    reaction = RE_RETURN_HOME;
        u32             idle = 0;                   // nights one of yours has stood waiting for orders
        mxquestnews_t   news = QN_NONE;             // what he has to tell you at dawn

    protected:
        std::string title;      // the design's "Titles"; empty for most lords
    };

    #define CITADEL_SCENARIO(x) static_cast<citadel_x*>(mx->scenario)->x

    inline citadel_race* CitadelRace ( mxrace_t race )
    {
        return static_cast<citadel_race*>(mx->RaceById(race));
    }

    inline citadel_character* CitadelLord ( mxcharacter* character )
    {
        return static_cast<citadel_character*>(character);
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

    inline mxobject* ObjectTarget ( mxid target )
    {
        return ID_TYPE(target) == IDT_OBJECT ? mx->ObjectById(GET_ID(target)) : nullptr;
    }

    inline mxregiment* RegimentTarget ( mxid target )
    {
        return ID_TYPE(target) == IDT_REGIMENT ? mx->RegimentById(GET_ID(target)) : nullptr;
    }

    bool IsArtefact ( const mxobject* object );

    bool ObjectOnMap ( const mxobject* object, mxgridref& where );

    mxobject* ArtefactAt ( mxgridref loc );

    void LiftObject ( mxobject* object );
}
#endif

#endif //_CITADELSCENARIO_INTERNAL_H_INCLUDED_

