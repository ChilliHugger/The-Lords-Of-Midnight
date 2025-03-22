/*
 * FILE:    ddr_scenario.cpp
 * 
 * PROJECT: ddr
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

#include "../../baseinc/tme_internal.h"

#if defined(_DDR_)

#include "../../utils/savegamemapping.h"
#include "scenario_ddr.h"
#include "scenario_ddr_internal.h"
#include "ddr_processor_text.h"
#include "ddr_processor_night.h"
#include "ddr_processor_battle.h"
#include "ddr_gameover.h"

using namespace chilli::lib::StringExtensions;

namespace tme {

//    namespace scenarios {

static scenarioinfo_t    ddr_scenario_info = {
    mxscenarioid::DDR,
    100,
    "DDR Scenario",
    "Chris Wild",
    "rorthron@doomdarksrevenge.com",
    "http://www.doomdarksrevenge.com",
    "Doomdark's Revenge for the Midnight Engine",
    "Copyright 1985 - 2025 Mike Singleton & Chris Wild"
};

static ddr_x* ddr_scenario = NULL ;

MXRESULT MXAPI ddr::Create ( tme::mxinterface* engine )
{
variant args;

    if ( engine == NULL )
        return MX_FAILED ;

    ddr_scenario = new ddr_x ;

    if ( ddr_scenario == NULL )
        return MX_FAILED ;

    args = ddr_scenario ;
    return engine->Command("@SETSCENARIO", &args, 1);

}


//
// DDR_X
// Internal Scenario Class Code
//
//


ddr_x::ddr_x() :
    morkin(nullptr),
    tarithel(nullptr),
    rorthron(nullptr),
    shareth(nullptr),
    cityofglireon(nullptr)
{
}

ddr_x::~ddr_x()
{
}

scenarioinfo_t* ddr_x::GetInfoBlock() const
{
    return &ddr_scenario_info ;
}


MXRESULT ddr_x::Register ( mxengine* midnightx )
{
    // mx = midnightx ;
    // add in the interfaces
    mx->text = new ddr_text;
    mx->night = new ddr_night;
    mx->battle = new ddr_battle;
    mx->gameover = new ddr_gameover;
    mx->entityfactory = new ddr_entityfactory;
    mx->scenario = (mxscenario*)ddr_scenario;
    
    // set initial feature flags
    mx->scenario->features = SF_TUNNELS
            |SF_MIST
            |SF_APPROACH_DDR
            |SF_RECRUIT_DDR
            |SF_RECRUIT_TIME ;
    
    return MX_OK ;
}

MXRESULT ddr_x::UnRegister ( mxengine* midnightx )
{
    SAFEDELETE ( mx->gameover );
    SAFEDELETE ( mx->text ) ;
    SAFEDELETE ( mx->night ) ;
    SAFEDELETE ( mx->battle ) ;
    SAFEDELETE ( mx->entityfactory );
    
    // mx will delete the scenario, so just lose our
    // reference to it
    ddr_scenario = nullptr ;
    return MX_OK ;
}

void ddr_x::initialise( u32 version )
{
    morkin = static_cast<ddr_character*>(mx->CharacterBySymbol("CH_MORKIN"));
    tarithel = static_cast<ddr_character*>(mx->CharacterBySymbol("CH_TARITHEL"));
    rorthron = static_cast<ddr_character*>(mx->CharacterBySymbol("CH_RORTHRON"));
    shareth = static_cast<ddr_character*>(mx->CharacterBySymbol("CH_SHARETH"));
    
    cityofglireon = static_cast<mxplace*>(mx->EntityByName("SH_CITY_GLIREON", IDT_PLACE));
    
    crownofvarenand = static_cast<ddr_object*>(mx->EntityByName("OB_CROWN_VARENAND", IDT_OBJECT));
    crownofcarudrium = static_cast<ddr_object*>(mx->EntityByName("OB_CROWN_CARUDRIUM", IDT_OBJECT));
    spellofthigrorn = static_cast<ddr_object*>(mx->EntityByName("OB_SPELL_THIGRORN", IDT_OBJECT));
    runesoffinorn = static_cast<ddr_object*>(mx->EntityByName("OB_RUNES_FINORN", IDT_OBJECT));
    crownofimiriel = static_cast<ddr_object*>(mx->EntityByName("OB_CROWN_IMIRIEL", IDT_OBJECT));
    
    mxscenario::initialise(version);
}
    
void ddr_x::initialiseAfterCreate( u32 version )
{
    //sv_riders_max_energy=255;
    //sv_warriors_max_energy=255;
    sv_stronghold_default_max=1250;
    sv_stronghold_default_min=50;
    sv_character_max_riders=1250;
    //sv_character_max_energy=255;
    sv_character_max_warriors=1250;
    //sv_object_energy_watersoflife=255;
    sv_energy_cannot_continue=31;
        
MXTRACE("Place Objects On Map");
    PlaceObjectsOnMap();
    
    FOR_EACH_CHARACTER(character) {
        auto c = static_cast<ddr_character*>(character);
        c->lastlocation=c->Location();
    }
    
    FOR_EACH_STRONGHOLD(stronghold) {
        stronghold->MaxTroops(sv_stronghold_default_max);
        stronghold->MinTroops(sv_stronghold_default_min);
    }
    
    // Obigorn the giant starts with Riders and not Warriors
    // this needs a data fix too
    // https://github.com/ChilliHugger/The-Lords-Of-Midnight/issues/135
    auto obigrorn = static_cast<ddr_character*>(mx->CharacterBySymbol("CH_OBIGRORN"));
    if(obigrorn!=nullptr){
        obigrorn->Flags().Reset(cf_allowedriders);
        obigrorn->Flags().Set(cf_allowedwarriors);
        obigrorn->warriors.Total(obigrorn->riders.Total());
        obigrorn->warriors.Energy(obigrorn->riders.Energy());
        obigrorn->warriors.Lost(obigrorn->riders.Lost());
        obigrorn->warriors.Killed(obigrorn->riders.Killed());
        obigrorn->riders.Total(0);
    }
  
    auto tinfo = mx->TerrainById(TN_ICYWASTE);
    tinfo->movementcost=2;
    
    // remap
    utils::UpdateDDRObjects::FixInvalidTypes();
    utils::UpdateDDRObjects::FixRecruitmentFlags();
        
    mxscenario::initialiseAfterCreate(version);

}

void ddr_x::updateAfterLoad ( u32 version )
{
    if ( version >= 11 ) {
        utils::FixMorkinFromBeingAIAfterRecruited();
    }

    utils::UpdateDDRObjects::FixRecruitmentFlags();

    mxscenario::updateAfterLoad(version);
}
    
mxentity* ddr_entityfactory::Create ( id_type_t type )
{
    switch ( type ) {
        case IDT_OBJECT:
            return static_cast<mxobject*>(new ddr_object);
        case IDT_CHARACTER:
            return static_cast<mxentity*>(new ddr_character) ;
        case IDT_STRONGHOLD:
            return static_cast<mxentity*>(new ddr_stronghold);
        case IDT_OBJECT_POWER:
            return static_cast<mxentity*>(new mxobjectpower) ;
        case IDT_OBJECT_TYPE:
            return static_cast<mxentity*>(new mxobjecttype) ;
        default:
            break;
    }

    return mxentityfactory::Create ( type );
}
    
mxcharacter* ddr_x::WhoHasObject( mxobject* object ) const
{
    return static_cast<mxcharacter*>(object->carriedby) ;
}


bool ddr_x::DropObject ( mxgridref loc, mxobject* obj )
{
    if ( obj->IsUnique() ) {
        obj->Location( loc );
        mx->gamemap->SetObject( loc, true );
        return true;
    }
    return mxscenario::DropObject(loc, obj);
}


mxobject* ddr_x::PickupObject ( mxgridref loc )
{
    if ( mx->gamemap->GetAt ( loc ).HasObject() ) {
        mxobject* object = FindObjectAtLocation(loc);
        mx->gamemap->SetObject(loc, false);
        object->Location(mxgridref(0, 0));
        return object;
    }
    return nullptr;
}
    
void ddr_x::PlaceObjectsOnMap ( void )
{
    mxgridref loc;
    
    
    FOR_EACH_OBJECT(o) {
        auto object = static_cast<ddr_object*>(o);
        if ( object->IsUnique() && object->IsRandomStart() ) {
            bool found=false;
            while ( !found ) {
                loc.x = mxrandom(0,mx->gamemap->Size().cx-1);
                loc.y = mxrandom(0,mx->gamemap->Size().cy-1);
                
                if ( !mx->gamemap->HasObject(loc) && !mx->scenario->isLocationImpassable(loc,luxor)  )
                    found=true;
            }
            object->Location(loc);
            mx->gamemap->SetObject( loc, true );
        }
    }
}

mxobject* ddr_x::FindObjectAtLocation ( mxgridref loc )
{
    if ( !mx->gamemap->HasObject(loc) )
        return nullptr;
    
    FOR_EACH_OBJECT(o) {
        auto object = static_cast<ddr_object*>(o);
        CONTINUE_IF ( object->IsCarried() );
        if ( object->Location() == loc )
            return object;
    }
    return nullptr;
}

mxstronghold* ddr_x::StrongholdFromLocation ( mxgridref loc )
{
    mxloc& mapsqr = mx->gamemap->GetAt ( loc );
    
    if ( mapsqr.flags&lf_stronghold ) {
        FOR_EACH_STRONGHOLD(stronghold) {
            if ( stronghold->Location() == loc )
                return stronghold;
        }
    }
    return nullptr;
}
    
mxcharacter* ddr_x::IsEnemyAtLocation( mxgridref loc, const ddr_character* character) const
{
    FOR_EACH_CHARACTER(c)
    {
        CONTINUE_IF(c->IsDead());

        CONTINUE_IF(c->IsInTunnel());

        CONTINUE_IF(c->Location() != loc);
        
        if ( !character->IsFriend(c) ) {
        
            auto loyalty = c->NormalisedLoyalty();
        
            if ( IsLoyalToTheMoonprince(loyalty) && character->InterestedInMoonprince() ) {
                return c;
            }
            
            if ( character->IsLoyalToTheFoe(loyalty) && character->InterestedInFoe() )
                return c ;
                
            if ( character->InterestedInOthers() )
                return c;
        }
    }
    
    return nullptr;
}

bool ddr_x::IsLoyalToTheMoonprince(mxrace_t race) const
{
    return race == RA_MOONPRINCE;
}
    
void ddr_x::NightStop(void)
{
}
    //}
    // namespace scenarios

void ddr_x::GiveGuidance(tme::mxcharacter *character, s32 hint)
{
std::string buffer = mx->text->CookedSystemString(SS_GUIDANCE1, character);

    RETURN_IF_NULL(character);

    int id = mxrandom(1,sv_characters-1);
    
    auto c = mx->CharacterById(id);

    RETURN_IF_NULL(c);
   
    if ( c->IsAlive() && !c->IsFriend(character) ) {
        // SS_SEEK_MSG1
        // , "{char:longname} stands {char:loc:terrain:prep} {char:loc:name}."
        buffer += mx->text->CookedSystemString(SS_SEEK_MSG1, c);
    
    } else {
        
        auto ddr = static_cast<ddr_character*>(character);
        auto o = ddr->desired_object ;
 
        if ( (character->Carrying() == o) || o==nullptr ) {
            c = shareth;
            
            if ( c->IsAlive() ) {
                // SS_SEEK_MSG1
                // , "{char:longname} stands {char:loc:terrain:prep} {char:loc:name}."
                buffer += mx->text->CookedSystemString(SS_SEEK_MSG1, c);
            } else {
                // SS_SEEK_MSG4
                // , "{char:longname} is dead."
                buffer += mx->text->CookedSystemString(SS_SEEK_MSG4, c);
            }
            
        } else {
            buffer += static_cast<ddr_text*>(mx->text)->DescribeObjectLocation(o);
        }
        
        // SS_GUIDANCE2
        // {char:text:obj}. {char:text:group}
        buffer += mx->text->CookedSystemString(SS_GUIDANCE2, character);
    }
    
    mx->SetLastActionMsg(buffer);
}

mxterrain_t ddr_x::toScenarioTerrain(mxterrain_t t) const
{
    if ( t == TN_PLAINS )
        t=TN_PLAINS2;
    else if ( t==TN_FOREST )
        t=TN_FOREST2;
    else if ( t==TN_MOUNTAIN )
        t=TN_MOUNTAIN2;
    else if ( t==TN_TOWER )
        t=TN_WATCHTOWER;
    return t;
}

}
// namespace tme


#endif // _DDR_


