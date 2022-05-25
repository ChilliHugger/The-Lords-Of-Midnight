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
#include "scenario_lom.h"
#include "scenario_lom_internal.h"
#include <string>

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

lom_x::lom_x()
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
    mx->battle = new mxbattle;
    mx->scenario = (mxscenario*)lom_scenario;
    
    // set initial feature flags
    mx->scenario->features = SF_MOONRING|SF_ICEFEAR  ;
    
    return MX_OK ;
}

MXRESULT lom_x::UnRegister ( mxengine* midnightx )
{
    SAFEDELETE ( mx->text ) ;
    SAFEDELETE ( mx->night ) ;
    SAFEDELETE ( mx->battle ) ;
    
    // mx will delete the scenario, so just lose our
    // reference to it
    lom_scenario = NULL ;
    return MX_OK ;
}

    
    
mxentity* lom_x::CreateEntity ( id_type_t type )
{
    switch ( type ) {
        case IDT_STRONGHOLD:
            return (mxentity*) new lom_stronghold ;
        default:
            break;
    }

    return mxscenario::CreateEntity ( type );
}


mxcharacter* lom_x::IceCrownCarrier( void ) const
{
    for ( int ii=0; ii<sv_characters; ii++ ) {
        mxcharacter* character = mx->CharacterById(ii+1);
        if ( character->IsAllowedIcecrown() && character->IsAlive() )
            return character ;
    }
    return NULL ;
}


// LOM implementation of the ICE FEAR
//
u32 lom_x::CalcFearAdjuster( mxlocinfo* locinfo ) const
{

mxcharacter*    morkin;
mxcharacter*    luxor;
mxplace*        TowerOfDoom;

    int adj_fear = 127;

    TowerOfDoom = (mxplace*)mx->EntityByName( "PL_ICE_FEAR_CENTRE", IDT_PLACE );
    if ( TowerOfDoom == NULL )
        return 0;

    // get morkin
    morkin = (mxcharacter*)mx->EntityByName("CH_MORKIN");
    if ( morkin->IsAlive() ) {

        // morkins distance from the tower of doom
        adj_fear = morkin->DistanceFromLoc( TowerOfDoom->Location() );

        // is morkin at this location?
        // if so then this is the ice fear to use
        if ( morkin->DistanceFromLoc(locinfo->Location()) == 0 ) {
            adj_fear = 0x01ff - ( adj_fear * 4);
            return adj_fear;
        }
        
    }

    // work out how much influence luxor 
    // will have on this location
    luxor = (mxcharacter*)mx->EntityByName("CH_LUXOR");
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
    
mxregiment* lom_x::FindEmptyRegiment()
{
    for (int ii = 0; ii < sv_regiments; ii++) {
        mxregiment* r = mx->RegimentById(ii+1);
        if ( r->Total() == 0 ) return r ;
    }
    return NULL ;
}

void lom_x::NightStart(void)
{
    mxcharacter* luxor = (mxcharacter*)mx->EntityByName("CH_LUXOR");
    luxorAlive = luxor->IsAlive();
    
    mxcharacter* morkin = (mxcharacter*)mx->EntityByName("CH_MORKIN");
    morkinAlive = morkin->IsAlive();
}


void lom_x::NightStop(void)
{
    
    mxcharacter* luxor = (mxcharacter*)mx->EntityByName("CH_LUXOR");
    if ( luxor->IsDead() ) {
        mxcharacter* morkin = (mxcharacter*)mx->EntityByName("CH_MORKIN");
        
        // if luxor is dead and the moonring has not been found
        // the morkin should be the current character
        // unless we were looking at luxor already
        mxcharacter* moonringcarrier = CurrentMoonringWearer();
        if ( moonringcarrier==NULL ) {
            if ( mx->CurrentChar()!=luxor ) {
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

        
        /*
        mxplace* p=NULL ;
        mxcharacter* c=NULL;
        
        // 1. Is the ice crown still active
        //mxobject* icecrown = (mxobject*)mx->EntityByName("OB_ICECROWN");
        //if ( icecrown == NULL )
        //    return;
        
        // icecrown destroyed?
        //if ( icecrown->IsDisabled() )
        //    return ;
        
        mxregiment* r = FindEmptyRegiment();
        if ( r == NULL )
            return;
        
        c = IceCrownCarrier();
        if ( c == NULL)
            return;
        
        //c = WhoHasObject(icecrown) ;
        // 2. is it being carried
        
        // respawn 1500 doomguard riders at ushgarak
        p = (mxplace*)mx->EntityByName( "PL_CITADEL_USHGARAK", IDT_PLACE );
        r->Location( p->Location() );
        r->Total(1500) ;
        r->Race(RA_DOOMGUARD);
        r->Type(UT_RIDERS);
        r->Success(4) ;
        r->Orders(OD_ROUTE);
        r->Target(p);
        
        // if the icecrown is being carried
        // then we either target the carrier
        // morkin, or luxor
        
        
        int type = mxrandom(1,5);
        
        // ushgarak is not important if the icecrown is down
        if ( c && type == 5 )
            type = 1;
        
        // target the carrier
        if ( type == 1 && c ) {
            r->Target(c);
            r->Orders(OD_FOLLOW);
            return;
        }
        // ushgarak
        if ( type == 5 ) {
            p = (mxplace*)mx->EntityByName( "PL_CITADEL_USHGARAK", IDT_PLACE );
            r->Target(p);
            r->Orders(OD_GOTO);
            return;
        }
        // xajorkith
        if ( type == 2 ) {
            p = (mxplace*)mx->EntityByName( "PL_CITADEL_XAJORKITH", IDT_PLACE );
            r->Target(p);
            r->Orders(OD_GOTO);
            return;
        }
        
        // luxor
        if ( type == 3 ) {
            c = (mxcharacter*)mx->EntityByName("CH_LUXOR");
            if ( c && c->IsAlive() ) {
                r->Target(c);
                r->Orders(OD_FOLLOW);
                return;
            }
            type = 4 ;
        }
        
        // morkin
        if ( type == 4 ) {
            c = (mxcharacter*)mx->EntityByName("CH_MORKIN");
            if ( c && c->IsAlive() ) {
                r->Target(c);
                r->Orders(OD_FOLLOW);
                return;
            }
        }
        */
    }
    
    
    

    //}
    // namespace scenarios

}
// namespace tme






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
