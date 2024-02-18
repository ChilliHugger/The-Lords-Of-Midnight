/*
 * FILE:    ddr_processor_night.cpp
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

#include "../../baseinc/tme_internal.h"
#include "ddr_processor_night.h"
#include "ddr_processor_text.h"
#include "ddr_processor_battle.h"
#include "ddr_gameover.h"

#include "scenario_ddr_internal.h"

#if defined(_DDR_)

namespace tme {

    //namespace processors {

        ddr_night::ddr_night()
        {
        }

        ddr_night::~ddr_night()
        {
        }

        void ddr_night::Process( void )
        {
            mx->scenario->NightStart();
            mx->battle->Reset();
            mx->scenario->RemoveMapArmies();
            
            RemoveMidwinterFromMap();
            
            SetSpecialLocationsCharacter();
            SetSpecialLocationsStrongholds();
            
            LordsProcessStart();
            
            static_cast<ddr_battle*>(mx->battle)->Process();
            
            // put critters on the map
            mx->gamemap->PutThingsOnMap();

            // move mist
            mx->gamemap->MoveMists();
            
            // tidy up

            // characters
            LordsProcessEnd();

            // strongholds
            FOR_EACH_STRONGHOLD(s) {
                auto stronghold = static_cast<ddr_stronghold*>(s);
                stronghold->OnRespawn();
            }

            // check morkin dead
            // check shareth dead
            static_cast<ddr_gameover*>(mx->gameover)->checkImportantCharactersDead();

            ResetSpecialLocations();
            
            mx->scenario->SetMapArmies();
            mx->scenario->NightStop();
        }
    
        void ddr_night::SetSpecialLocationsStrongholds()
        {
            FOR_EACH_STRONGHOLD(stronghold) {
                mx->gamemap->SetLocationSpecial(stronghold->Location(),1);
            }
        }
    
        void ddr_night::ResetLocationSpecial ( mxgridref loc )
        {
            mx->gamemap->SetLocationSpecial(loc,0);
        }

        void ddr_night::LordsProcessStart()
        {
            // place all lords at dawn
            FOR_EACH_CHARACTER(c) {
                auto character = static_cast<ddr_character*>(c);
                
                CONTINUE_IF( character->IsDead());
                
                character->InitNightProcessing();
                
#if !defined( _TME_DEMO_MODE_ )
                character->Turn();
#endif
            }
        }

        void ddr_night::LordsProcessEnd()
        {
            FOR_EACH_CHARACTER(c) {
                auto character = static_cast<ddr_character*>(c);
                
                CONTINUE_IF(character->IsDead());
                                
                // remove critters from character locations
                mxloc& m = mx->gamemap->GetAt(character->Location());
                m.flags &= ~lf_creature ;
                m.object = OB_NONE ;
                
#if !defined( _TME_DEMO_MODE_ )
                //
                // airecruitemen
                //
                character->AICheckRecruitSoldiers();
                
                //
                character->CheckKilledFoe();
#endif
                
                // cleanup 'prepares to do battle'
                int enemies=0;
                FOR_EACH_CHARACTER(c) {
                    CONTINUE_IF( c->IsDead() );
                    CONTINUE_IF( c->Location() != character->Location() );
                    CONTINUE_IF( c->IsFriend(character) );
                    enemies++;
                }
                
                auto scenario = static_cast<ddr_x*>(mx->scenario);
                auto stronghold = dynamic_cast<ddr_stronghold*>(scenario->StrongholdFromLocation(character->Location()));
                if ( stronghold && !stronghold->IsFriend(character))
                    enemies++;
                    
                if ( enemies != 0 )
                    character->Flags().Set(cf_preparesbattle);
                    else
                    character->Flags().Reset(cf_preparesbattle);
            }
        }
    
        void ddr_night::MoveMidwinter ( void )
        {
        }
    
        void ddr_night::RemoveMidwinterFromMap()
        {
        }
    
    //}
    // namespace processors
}
// namespace tme

#endif // _DDR_
