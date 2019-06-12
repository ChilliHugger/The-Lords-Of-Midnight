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

#include "scenario_ddr.h"
#include "scenario_ddr_internal.h"
#include "ddr_processor_text.h"
#include "ddr_processor_night.h"
#include "ddr_processor_battle.h"

namespace tme {

//	namespace scenarios {

static scenarioinfo_t	ddr_scenario_info = {
	17,
	100,
	"DDR Scenario",
	"Chris Wild",
	"rorthron@doomdarksrevenge.com",
	"http://www.doomdarksrevenge.com",
	"Doomdark's Revenge for the Midnight Engine",
    "Copyright 1985 - 2017 Mike Singleton & Chris Wild"
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


ddr_x::ddr_x()
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
	mx->scenario = (mxscenario*)ddr_scenario;
	
	// set initial feature flags
	mx->scenario->features = SF_APPROACH_DDR|SF_RECRUIT_DDR|SF_RECRUIT_TIME ;
	
	return MX_OK ;
}

MXRESULT ddr_x::UnRegister ( mxengine* midnightx )
{
	SAFEDELETE ( mx->text ) ;
	SAFEDELETE ( mx->night ) ;
	SAFEDELETE ( mx->battle ) ;
	
	// mx will delete the scenario, so just lose our
	// reference to it
	ddr_scenario = NULL ;
	return MX_OK ;
}
	
    
void ddr_x::initialiseAfterCreate( void )
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
    
    for (int ii = 0; ii < sv_characters; ii++) {
        ddr_character* c = static_cast<ddr_character*>(mx->CharacterById(ii+1));
        c->lastlocation=c->Location();
    }
    
    for ( int ii=0; ii < sv_strongholds; ii++ ) {
        mxstronghold* s = static_cast<mxstronghold*>(mx->StrongholdById(ii+1));
        s->Max(sv_stronghold_default_max);
        s->Min(sv_stronghold_default_min);
    }
    
    mxterrain* tinfo = mx->TerrainById(TN_ICYWASTE);
    tinfo->movementcost=2;
    
    mxscenario::initialiseAfterCreate();

}
	
mxentity* ddr_x::CreateEntity ( id_type_t type )
{
	switch ( type ) {
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

	return mxscenario::CreateEntity ( type );
}
    
void ddr_x::MakeMapAreaVisible ( mxgridref l, mxcharacter* character )
{
    // in a tunnel?
    if ( character->IsInTunnel() ) {

        mxloc& current = mx->gamemap->GetAt( l );
        current.flags|=lf_tunnel_looked_at|lf_tunnel_visited;
        
        // check the surrounding locations for tunnels
        // N E S W
        // and mark them as seen
        for ( int ii=DR_NORTH; ii<=DR_NORTHWEST; ii+=2 ) {
            if ( mx->gamemap->IsTunnel(l + (mxdir_t)ii))
                mx->gamemap->SetTunnelVisible(l + (mxdir_t)ii, true);
        }
        
        return;
    }
    
    mxscenario::MakeMapAreaVisible(l, character);
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
    mxobject* object;
    mxgridref loc;
    for ( int ii=0; ii<sv_objects; ii++ ) {
        object = mx->ObjectById(ii+1);
        if ( object->IsUnique() && object->IsRandomStart() ) {
            bool found=false;
            while ( !found ) {
                loc.x = mxrandom(0,mx->gamemap->Size().cx-1);
                loc.y = mxrandom(0,mx->gamemap->Size().cy-1);
                if ( !mx->gamemap->HasObject(loc) && !mx->gamemap->IsLocationBlock(loc)  )
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
    
    mxobject* object;
    for ( int ii=0; ii<sv_objects; ii++ ) {
        object = mx->ObjectById(ii+1);
        if ( object->IsCarried() )
            continue;
        if ( object->Location() == loc )
            return object;
    }
    return nullptr;
}

mxstronghold* ddr_x::StrongholdFromLocation ( mxgridref loc )
{
    mxloc& mapsqr = mx->gamemap->GetAt ( loc );
    if ( !(mapsqr.flags&lf_stronghold) )
        return nullptr;
    
    mxstronghold* stronghold;
    for ( int ii=0; ii<sv_strongholds; ii++ ) {
        stronghold = mx->StrongholdById(ii+1);
        if ( stronghold->Location() == loc )
            return stronghold;
    }
    return nullptr;
}
    
    
void ddr_x::NightStop(void)
{
}
	//}
	// namespace scenarios

void ddr_x::GiveGuidance(tme::mxcharacter *character, s32 hint)
{
	int id = mxrandom(1,sv_characters-1);
    
    mxcharacter* c = mx->CharacterById(id);
    
    if ( (c->Loyalty() != character->Loyalty() && c->IsAlive()) ) {
        c_strcat ( (LPSTR)mx->LastActionMsg(),
                             mx->text->CookText((LPSTR)mx->text->SystemString(SS_SEEK_MSG1),c) );
    } else {
        
        //int firstObject = mx->EntityByName("OB_CROWN_VARENAND")->Id();
        //id =  mxrandom(firstObject,sv_objects-1);
        //mxobject* o = mx->ObjectById(id);
        
        ddr_character* ddr = static_cast<ddr_character*>(character);
        mxobject* o = ddr->desired_object ;
        
        if ( (character->Carrying() == o) || o==NULL ) {
            c = (mxcharacter*)mx->EntityByName("CH_SHARETH");
            c_strcat ( (LPSTR)mx->LastActionMsg(),
                                 mx->text->CookText((LPSTR)mx->text->SystemString(SS_SEEK_MSG1),c) );
            c_strcat ( (LPSTR)mx->LastActionMsg(),
                                 mx->text->CookText((LPSTR)mx->text->SystemString(SS_GUIDANCE2),character) );
            return;
        }
        
        c_strcat ( (LPSTR)mx->LastActionMsg(),
                             mx->text->DescribeObjectLocation(o) );
        c_strcat ( (LPSTR)mx->LastActionMsg(),
                             mx->text->CookText((LPSTR)mx->text->SystemString(SS_GUIDANCE2),character) );
        
    }
}

mxterrain_t ddr_x::NormaliseTerrain( mxterrain_t t) const
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


