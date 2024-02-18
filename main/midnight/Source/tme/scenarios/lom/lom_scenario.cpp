/*
 * FILE:    lom_scenario.cpp
 * 
 * PROJECT: lom
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
#include "../../utils/savegamemapping.h"
#include "scenario_lom.h"
#include "scenario_lom_internal.h"
#include "lom_processor_battle.h"
#include "lom_gameover.h"
#include <string>

#if defined(_LOM_)

namespace tme {

//    namespace scenarios {

    static scenarioinfo_t    lom_scenario_info = {
        16,
        100,
        "LOM Scenario",
        "Chris Wild",
        "rorthron@thelordsofmidnight.com",
        "http://www.thelordsofmidnight.com",
        "The Lords of Midnight for the Midnight Engine",
        "Copyright 1984 - 2017 Mike Singleton & Chris Wild"
    };

static lom_x* lom_scenario = NULL ;

    
MXRESULT MXAPI lom::Create ( tme::mxinterface* engine )
{
variant args;

    if ( engine == NULL )
        return MX_FAILED ;

    lom_scenario = new lom_x ;

    if ( lom_scenario == NULL )
        return MX_FAILED ;

    args = lom_scenario ;
    return engine->Command("@SETSCENARIO", &args, 1);

}
    

scenarioinfo_t* lom::GetInfoBlock() const
    {
        return lom_scenario->GetInfoBlock();
    }
    
MXRESULT lom::Command ( const std::string& arg, variant argv[], u32 argc )
    {
        return lom_scenario->Command(arg, argv, argc);
    }
    
MXRESULT lom::GetProperties ( const std::string& arg, variant argv[], u32 argc )
    {
        return lom_scenario->GetProperties(arg, argv, argc);
    }
    
MXRESULT lom::Text ( const std::string& command, variant* argv, u32 args )
    {
        return lom_scenario->Text(command, argv, args);
    }
    

//
// LOM_X
// Internal Scenario Class Code
//
//

lom_x::lom_x() :
    towerofdoom(nullptr),
    morkin(nullptr),
    luxorAlive(true),
    morkinAlive(true)
{
}

lom_x::~lom_x()
{
}

scenarioinfo_t* lom_x::GetInfoBlock() const
{
    return &lom_scenario_info ;
}

MXRESULT lom_x::Register ( mxengine* midnightx )
{
    // mx = midnightx ;
    // add in the interfaces
    mx->text = new mxtext;
    mx->night = new mxnight;
    mx->battle = new lom_battle;
    mx->gameover = new lom_gameover;
    mx->entityfactory = new mxentityfactory;
    mx->scenario = (mxscenario*)lom_scenario;
    
    // set initial feature flags
    mx->scenario->features = SF_MOONRING|SF_ICEFEAR  ;
    
    return MX_OK ;
}

MXRESULT lom_x::UnRegister ( mxengine* midnightx )
{
    SAFEDELETE ( mx->gameover ) ;
    SAFEDELETE ( mx->text ) ;
    SAFEDELETE ( mx->night ) ;
    SAFEDELETE ( mx->battle ) ;
    SAFEDELETE ( mx->entityfactory );
    
    // mx will delete the scenario, so just lose our
    // reference to it
    lom_scenario = nullptr ;
    return MX_OK ;
}

    
mxentity* lom_entityfactory::Create ( id_type_t type )
{
    switch ( type ) {
        case IDT_STRONGHOLD:
            return new lom_stronghold;
        default:
            break;
    }

    return mxentityfactory::Create ( type );
}


mxcharacter* lom_x::IceCrownCarrier( void ) const
{
    FOR_EACH_CHARACTER(character) {
        if ( character->IsAllowedIcecrown() && character->IsAlive() )
            return character ;
    }
    return nullptr ;
}


// LOM implementation of the ICE FEAR
//
u32 lom_x::CalcFearAdjuster( mxlocinfo* locinfo ) const
{
    int adj_fear = 127;

    if ( towerofdoom == nullptr )
        return 0;

    if ( morkin->IsAlive() ) {

        // morkins distance from the tower of doom
        adj_fear = morkin->DistanceFromLoc( towerofdoom->Location() );

        // is morkin at this location?
        // if so then this is the ice fear to use
        if ( morkin->DistanceFromLoc(locinfo->Location()) == 0 ) {
            adj_fear = 0x01ff - ( adj_fear * 4);
            return adj_fear;
        }
        
    }

    // work out how much influence luxor 
    // will have on this location
    adj_fear += ( luxor->IsDead() ) ? 127 : luxor->DistanceFromLoc(locinfo->Location()) ;

    // work out how much affect doomdark
    // will have on this location
    adj_fear += 48 + locinfo->adj_stronghold;

    return adj_fear ;
}

//
// Adjustments to the map to make certain locations accessible
// when impassable mountains is enabled. Controlled by RF_ADD_MOUNTAIN_PASSES
//
static int mountain_pass_adjustments[][3] = {
    { 36,11, TN_DOWNS  }, // Tower of Ugrorn (37,10)
    { 12,26, TN_FOREST }, // Tower of Coroth (13,25)
    { 12,27, TN_FOREST },
    { 20,33, TN_PLAINS }, // Tower of Dorak (19,32)
    { 18,31, TN_PLAINS },
    {  9,47, TN_DOWNS  }, // Keep of Torkren (8,48)
    {  7,32, TN_PLAINS }, // Cavern of Ogrim (8,31)
    { 38,49, TN_FOREST }, // Tower or Morning (39,50)
    { 38,51, TN_FOREST },
    { 57,54, TN_DOWNS  }, // A Keep in the domain of Corelay (56,55)
    { 57,56, TN_PLAINS },
                          // Village in the domain of Corelay (49,54)
    { 49,56, TN_DOWNS  }, // Lake in the domain of Corelay (50,55)
                          // Ruin in the domain of Corelay (48,56)
    { 57,58, TN_DOWNS  }, // Henge in the domain of Corlay (58,59)
};


void lom_x::initialise(u32 version)
{
    towerofdoom = static_cast<mxplace*>(mx->EntityByName( "PL_ICE_FEAR_CENTRE", IDT_PLACE ));
    morkin = mx->CharacterBySymbol("CH_MORKIN");
    
    mxscenario::initialise(version);
}

void lom_x::initialiseAfterCreate(u32 version)
{
    // FIX: Database has warriors and riders success the wrong way around
    if(version<9999){
        auto warriors = mx->UnitById(1);
        auto riders = mx->UnitById(2);
        swap(riders->success, warriors->success);
    }
    
    if(mx->isRuleEnabled(RF_ADD_MOUNTAIN_PASSES)) {
        for( auto adj : mountain_pass_adjustments ) {
            mx->gamemap->GetAt(mxgridref(adj[0], adj[1])).terrain = adj[2];
        }
    }
    
    mxscenario::initialiseAfterCreate(version);
}

void lom_x::updateAfterLoad(u32 version)
{
    mxscenario::updateAfterLoad(version);
}

    
mxregiment* lom_x::FindEmptyRegiment()
{
    FOR_EACH_REGIMENT(regiment) {
        if ( regiment->Total() == 0 ) return regiment ;
    }
    return nullptr ;
}

void lom_x::NightStart(void)
{
    luxorAlive = luxor->IsAlive();
    morkinAlive = morkin->IsAlive();
}


void lom_x::NightStop(void)
{
    
    if ( luxor->IsDead() ) {
        // if luxor is dead and the moonring has not been found
        // the morkin should be the current character
        // unless we were looking at luxor already
        mxcharacter* moonringcarrier = CurrentMoonringWearer();
        if ( moonringcarrier==nullptr ) {
            if ( mx->CurrentChar() != luxor ) {
                mx->CurrentChar(morkin);
            }
        }
        
        // if morkin is also dead
        // check if he was alive at the start of the night
        // if he was then he stays the current character
        // otherwise report on luxor
        if (morkin->IsDead()) {
            if ( morkinAlive )
                mx->CurrentChar(morkin);
            else
                mx->CurrentChar(luxor);
        }
    }
}
    
    
    

    //}
    // namespace scenarios

}
// namespace tme

#endif // _LOM_




/*
        u32 base::CalcFearAdjuster(mxCLocInfo* locinfo) const
        {
        mxcharacter* character;
        bool        icecrowncarrierhere;
        int            ii;
        gridref        icefearbaseloc(0,0);

            int adj_fear = 127;

            // is there someone alive who can carry the ice crown?
            // if not then the fear will be severe
            if ( IceCrownCarrier() ) {


                mxplace* TowerOfDoom = (mxplace*)mx->EntityByName( "PL_TOWER_OF_DOOM", IDT_PLACE );
                if ( TowerOfDoom )
                    icefearbaseloc = TowerOfDoom->Location() ;

                //if ( mx->ReadBool("_ICEFEAR_FROM_ICECROWN_") ) {
                //    character = mx->WhoHasObject(OB_ICECROWN) ;
                //    if ( character ) {
                //        icefearbaseloc=character->Location();
                //    } else {
                //        // need to find the ice crown
                //    }
                //}

                // 
                //now find the nearest Ice Crown carryer
                //to the tower of doom
                //
                adj_fear = 9999;
                for ( ii=0; ii<sv_characters; ii++ ) {
                    character = mx->CharacterById(ii+1);
                    if ( character->IsAllowedIcecrown() && character->IsAlive() )
                        adj_fear = MIN ( character->DistanceFromLoc( icefearbaseloc ), adj_fear );
                }
                if ( adj_fear == 9999 )
                    adj_fear=0;
                
                //
                //is there anyone at the current location that can carry the
                //ice crown?
                //
                icecrowncarrierhere=FALSE;
                for ( ii=0; ii<locinfo->objCharacters.Count(); ii++ ) {
                    mxcharacter* character = (mxcharacter*)locinfo->objCharacters[ii];
                    if ( character->IsAllowedIcecrown() ) {
                        icecrowncarrierhere=TRUE;
                        break;
                    }
                }
                
                // is morkin at this location?
                // if so then this is the ice fear to use
                if ( icecrowncarrierhere ) {
                    adj_fear = 0x01ff - ( adj_fear * 4);
                    return adj_fear;
                }
                
            }

            // work out how much influence the moon ring
            // will have on this location
            object* moonring = (object*)mx->EntityByName("OB_MOONRING");
            character = WhoHasObject(moonring) ;
            adj_fear += ( (character==NULL) || character->IsDead() ) ? 127 : character->DistanceFromLoc(locinfo->Location()) ;

            // work out how much affect doomdark
            // will have on this location
            adj_fear += 48 + locinfo->adj_stronghold;

            return adj_fear ;
        }
*/
