/*
 * FILE:    processor_night.cpp
 * 
 * PROJECT: MidnightEngine
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

#include "../baseinc/tme_internal.h"
#include "../baseinc/regiment_turn_processor.h"

#include <string>

namespace tme {

    //namespace processors {

        mxnight::mxnight()
        {
        }

        mxnight::~mxnight()
        {
        }

        void mxnight::Process( void )
        {
        int                ii;
        mxcharacter*        character;

            mx->scenario->NightStart();
            mx->battle->Reset();

            // place all lords at dawn
            FOR_EACH_CHARACTER(character) {
                character->InitNightProcessing();
            }

#ifdef _LOM_
            SetSpecialLocationsCharacter();
            SetSpecialLocationsStrongholds();

#ifndef _TME_DEMO_MODE_
            // process all the regiments
            std::unique_ptr<RegimentTurnProcessor> processor ( new RegimentTurnProcessor() );
            FOR_EACH_REGIMENT(regiment) {
                processor->Process(regiment);
            }
#endif
 
            ResetSpecialLocations();
            
            MoveMidwinter();
            
#endif

            mx->scenario->NightStop();
        }

        void mxnight::SetSpecialLocationsCharacter()
        {
            FOR_EACH_CHARACTER(character) {
                if ( character->IsAlive() && !character->IsHidden() && character->race!=RA_MIDWINTER ) {
                    mx->gamemap->SetLocationSpecial(character->Location(),1);
                }
            }
        }

        void mxnight::SetSpecialLocationsStrongholds()
        {
            FOR_EACH_STRONGHOLD(stronghold) {
                if ( stronghold->OccupyingRace() != RA_DOOMGUARD )
                    mx->gamemap->SetLocationSpecial(stronghold->Location(),1);
            }
        }

        void mxnight::ResetSpecialLocations()
        {
            // remove mark where all the characters are
            FOR_EACH_CHARACTER(character) {
                ResetLocationSpecial(character->Location());
            }

            // remove mark where the keeps and citadels armies are
            FOR_EACH_STRONGHOLD(stronghold) {
                ResetLocationSpecial(stronghold->Location());
            }
        }

        void mxnight::ResetLocationSpecial ( mxgridref loc )
        {
            if ( mx->gamemap->IsLocationSpecial(loc) )
                mx->battle->ProcessLocation(loc);
            mx->gamemap->SetLocationSpecial(loc,0);
        }
    
        void mxnight::MoveMidwinter ( void )
        {
            static tme::loc_t locations[] = {
                loc_t(38,18),
                loc_t(41,21),
                loc_t(42,15),
                loc_t(46,14),
                loc_t(49,13)
            };
        
            int p = mxrandom(NUMELE(locations) - 1);
            
            auto c = mx->CharacterBySymbol("CH_MIDWINTER");
            RETURN_IF_NULL(c);
        
            c->Location(locations[p]);
        }
    
    //}
    // namespace processors
}
// namespace tme
