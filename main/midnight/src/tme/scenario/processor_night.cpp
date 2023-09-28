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
            auto processor = new RegimentTurnProcessor();
            FOR_EACH_REGIMENT(regiment) {
                processor->Process(regimemt);
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
            auto missions = mx->objMissions;
            missions.Sort(1);
            for ( auto m : missions ) {
                if ( !night && m->Condition() != MC_CHARACTER_DEAD )
                    continue;
                m->CheckComplete();
            }

            // scan the victory tables
            auto victories = mx->objVictories;
            victories.Sort(1);
            for ( auto v : victories ) {
                if ( v->CheckComplete() ) {
                    // display message
                    mx->SetLastActionMsg(mx->text->CookedSystemString(v->Message()));
     
                    // game over?
                    m_gameover_t gameover = MG_NONE ;
                    if ( v->IsGameOver() ) {
                        gameover = v->Priority()==3 ? MG_LOSE : MG_WIN ;
                    }
                    //mx->NightCallback(NULL);
                    if ( gameover != MG_NONE ) {
                        return gameover ;
                    }
                }
            }
#endif

            return MG_NONE ;
        }

    
        void mxnight::MoveMidwinter ( void )
        {
            static tme::loc_t locations[] = { loc_t(38,18), loc_t(41,21), loc_t(42,15), loc_t(46,14), loc_t(49,13) };
        
            int p = mxrandom(NUMELE(locations) - 1);
            
            mxcharacter* c = (mxcharacter*)mx->EntityByName("CH_MIDWINTER");
            if ( c==NULL )
                return;
        
            c->Location(locations[p]);
            
        }
    
#ifdef _TEST_WINLOSE_CONDITIONS_
    void mxnight::testWinLoseConditions ( void )
    {
        mxcharacter* luxor = (mxcharacter*)mx->EntityByName("CH_LUXOR");

        mxcharacter* morkin = (mxcharacter*)mx->EntityByName("CH_MORKIN");
        mxcharacter* xajorkith = (mxcharacter*)mx->EntityByName("CH_XAJORKITH");
        mxcharacter* farflame = (mxcharacter*)mx->EntityByName("CH_FARFLAME");
        mxcharacter* lorgim = (mxcharacter*)mx->EntityByName("CH_LORGRIM");
        mxcharacter* fawkrin = (mxcharacter*)mx->EntityByName("CH_FAWKRIN");
        mxplace* mirrow = (mxplace*)mx->EntityByName("PL_LAKE_MIRROW");
        mxstronghold* sh_ushgarak = (mxstronghold*)mx->EntityByName("SH_CITADEL_USHGARAK");
        mxstronghold* sh_xajorkith = (mxstronghold*)mx->EntityByName("SH_CITADEL_XAJORKITH");
    
        
        int test = 5;
        
        morkin->carrying = mx->ObjectById(OB_ICECROWN);

        mx->battle->ResetBattlesFought();
        
        switch ( test ) {
            case 1:
                // morkin & farflame
                farflame->Location( morkin->Location() ) ;
                break;
            case 2:
                // morkin & logrim
                lorgim->Location( morkin->Location() ) ;
            break;
            case 3:
                // morkin & fawkrin
                fawkrin->Location( morkin->Location() ) ;
                break;
            case 4:
                // morkin & lake mirrow
                morkin->Location( mirrow->Location() ) ;
                break;
                
            case 5:
                // ushgarak fallen
                //
                FOR_EACH_REGIMENT(r) {
                    r->Total(0);
                }
                
                luxor->warriors.Total(1200);
                luxor->riders.Total(1200);
                luxor->Location( sh_ushgarak->Location() );
                
                mx->battle->KickOffAtLocation(luxor->Location());
                //sh_ushgarak->Total(10);
                
                break;

            case 6:
                // ushgarak fallen
                //
                FOR_EACH_REGIMENT(r) {
                    r->Total(0);
                }
                
                luxor->warriors.Total(1200);
                luxor->riders.Total(1200);
                luxor->Location( sh_ushgarak->Location() );
                
                sh_ushgarak->TotalTroops(10);
                
                mx->battle->KickOffAtLocation(luxor->Location());
                
                // morkin & farflame
                farflame->Location( morkin->Location() ) ;
                
                break;
        
            case 7:
                // xajorkith fallen
                //
                
                ((mxregiment*)mx->objRegiments[0])->Location( sh_xajorkith->Location() );
                
                sh_xajorkith->TotalTroops(10);

                morkin->Cmd_Dead();
                
                xajorkith->Location( luxor->Location() );
                mx->battle->KickOffAtLocation(luxor->Location());
                break;
                
            case 8:
                // luxor and morkin dead
                luxor->Cmd_Dead();
                morkin->Cmd_Dead();
                break;
                
                
        }
     
    }
#endif //_TEST_WINLOSE_CONDITIONS_
    
    //}
    // namespace processors
}
// namespace tme
