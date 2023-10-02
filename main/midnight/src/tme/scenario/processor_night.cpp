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
            mx->battle->ResetBattlesFought();

            // place all lords at dawn
            FOR_EACH_CHARACTER(character) {
                character->StartDawn();
            }

#ifdef _LOM_
            // check all the lord that are not hidden and alive
            // and mark their locations as being of special interest
            // by setting the special bit on in the map data
     
            FOR_EACH_CHARACTER(character) {
                if ( character->IsAlive() && !character->IsHidden() && character->race!=RA_MIDWINTER ) {
                    mx->gamemap->SetLocationSpecial(character->Location(),1);
                }
            }

            // checking all the strongholds that don't belong to doomdark
            // and mark then on the map as being of special interest
            FOR_EACH_STRONGHOLD(stronghold) {
                if ( stronghold->OccupyingRace() != RA_DOOMGUARD )
                    mx->gamemap->SetLocationSpecial(stronghold->Location(),1);
            }

#ifndef _TME_DEMO_MODE_
            // process all the regiments
            std::unique_ptr<RegimentTurnProcessor> processor ( new RegimentTurnProcessor() );
            FOR_EACH_REGIMENT(regiment) {
                processor->Process(regiment);
            }
#endif
            // remove mark where all the characters are
            FOR_EACH_CHARACTER(character) {
                ResetLocationSpecial(character->Location());
            }

            // remove mark where the keeps and citadels armies are
            FOR_EACH_STRONGHOLD(stronghold) {
                ResetLocationSpecial(stronghold->Location());
            }
            
            MoveMidwinter();
            
#endif

            mx->scenario->NightStop();
        }

        void mxnight::ResetLocationSpecial ( mxgridref loc )
        {
            if ( mx->gamemap->IsLocationSpecial(loc) )
                mx->battle->KickOffAtLocation(loc);
            mx->gamemap->SetLocationSpecial(loc,0);
        }

        m_gameover_t mxnight::CheckWinLoseConditions ( bool night )
        {
#ifndef _TME_DEMO_MODE_
            // scan the mission tables
            auto missions = mx->objMissions.Clone();
            missions.Sort(mxmission::SORT::PRIORITY);
            for ( auto m : missions ) {
                //MXTRACE("%s[%d] %d", m->Symbol().c_str(), (int)m->Id(), (int)m->Priority() );
                CONTINUE_IF ( !night && m->Condition() != MC_CHARACTER_DEAD );
                m->CheckComplete();
            }

            // scan the victory tables
            auto victories = mx->objVictories.Clone();
            victories.Sort(mxvictory::SORT::PRIORITY);
            for ( auto v : victories ) {
                //MXTRACE("%s[%d] %d", v->Symbol().c_str(), (int)v->Id(), (int)v->Priority() );
                CONTINUE_IF( !v->CheckComplete() );
                
                // display message
                mx->SetLastActionMsg(mx->text->CookedSystemString(v->Message()));
     
                // game over?
                m_gameover_t gameover = MG_NONE ;
                if ( v->IsGameOver() ) {
                    gameover = v->Priority() == 3 ? MG_LOSE : MG_WIN ;
                }
                if ( gameover != MG_NONE ) {
                    return gameover ;
                }
            }
#endif
            return MG_NONE ;
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
    
#ifdef _TEST_WINLOSE_CONDITIONS_
    void mxnight::testWinLoseConditions ( void )
    {
        auto ch_luxor = mx->CharacterBySymbol("CH_LUXOR");
        auto ch_morkin = mx->CharacterBySymbol("CH_MORKIN");
        auto ch_xajorkith = mx->CharacterBySymbol("CH_XAJORKITH");
        auto ch_farflame = mx->CharacterBySymbol("CH_FARFLAME");
        auto ch_lorgim = mx->CharacterBySymbol("CH_LORGRIM");
        auto ch_fawkrin = mx->CharacterBySymbol("CH_FAWKRIN");
        auto pl_mirrow = static_cast<mxplace*>(mx->EntityByName("PL_LAKE_MIRROW", IDT_PLACE));
        auto sh_ushgarak = static_cast<mxstronghold*>(mx->EntityByName("SH_CITADEL_USHGARAK", IDT_STRONGHOLD));
        auto sh_xajorkith = static_cast<mxstronghold*>(mx->EntityByName("SH_CITADEL_XAJORKITH", IDT_STRONGHOLD));
    
        
        int test = 5;
        
        morkin->carrying = static_cast<mxobject*>(mx->EntityByName("OB_ICECROWN", IDT_OBJECT));

        mx->battle->ResetBattlesFought();
        
        switch ( test ) {
            case 1:
                // morkin & farflame
                ch_farflame->Location( ch_morkin->Location() ) ;
                break;
            case 2:
                // morkin & logrim
                ch_lorgim->Location( ch_morkin->Location() ) ;
            break;
            case 3:
                // morkin & fawkrin
                ch_fawkrin->Location( ch_morkin->Location() ) ;
                break;
            case 4:
                // morkin & lake mirrow
                ch_morkin->Location( ch_mirrow->Location() ) ;
                break;
                
            case 5:
                // ushgarak fallen
                //
                FOR_EACH_REGIMENT(r) {
                    r->Total(0);
                }
                
                ch_luxor->warriors.Total(1200);
                ch_luxor->riders.Total(1200);
                ch_luxor->Location( sh_ushgarak->Location() );
                
                mx->battle->KickOffAtLocation(ch_luxor->Location());
                
                break;

            case 6:
                // ushgarak fallen
                //
                FOR_EACH_REGIMENT(r) {
                    r->Total(0);
                }
                
                ch_luxor->warriors.Total(1200);
                ch_luxor->riders.Total(1200);
                ch_luxor->Location( sh_ushgarak->Location() );
                
                sh_ushgarak->TotalTroops(10);
                
                mx->battle->KickOffAtLocation(ch_luxor->Location());
                
                // morkin & farflame
                ch_farflame->Location( ch_morkin->Location() ) ;
                
                break;
        
            case 7:
                // xajorkith fallen
                //
                
                mx->objRegiments.First()->Location( sh_xajorkith->Location() );
                
                sh_xajorkith->TotalTroops(10);

                ch_morkin->Cmd_Dead();
                
                ch_xajorkith->Location( ch_luxor->Location() );
                mx->battle->KickOffAtLocation(ch_luxor->Location());
                break;
                
            case 8:
                // luxor and morkin dead
                ch_luxor->Cmd_Dead();
                ch_morkin->Cmd_Dead();
                break;
                
                
        }
     
    }
#endif //_TEST_WINLOSE_CONDITIONS_
    
    //}
    // namespace processors
}
// namespace tme
