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


//#define _DDR_

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
		int				ii;
		mxcharacter*		character;

			mx->scenario->NightStart();
			mx->battle->ResetBattlesFought();

			// place all lords at dawn
			for (ii = 0; ii < sv_characters; ii++) {
				character = mx->CharacterById(ii+1) ;
				character->StartDawn();
			}

#ifdef _LOM_
            mxstronghold*	stronghold;
			// check all the lord that are not hidden and alive
			// and mark their locations as being of special interest
			// by setting the special bit on in the map data
     
			for (ii = 0; ii < sv_characters; ii++) {
				character = mx->CharacterById(ii+1) ;
				if ( character->IsAlive() && !character->IsHidden() && character->race!=RA_MIDWINTER ) {
					mx->gamemap->SetLocationSpecial(character->Location(),1);
                }
			}

			// checking all the strongholds that don't belong to doomdark
			// and mark then on the map as being of special interest
			for (ii = 0; ii < sv_strongholds; ii++) {
				stronghold=mx->StrongholdById(ii+1);
				if ( stronghold->OccupyingRace() != RA_DOOMGUARD )
					mx->gamemap->SetLocationSpecial(stronghold->Location(),1);
			}

#ifndef _TME_DEMO_MODE_
			// process all the regiments
			for (ii = 0; ii < sv_regiments; ii++)
				mx->RegimentById(ii+1)->Cmd_Night();
#endif
			// remove mark where all the characters are
			for (ii = 0; ii < sv_characters; ii++)
				ResetLocationSpecial(mx->CharacterById(ii+1)->Location());

			// remove mark where the keeps and citadels armies are
			for (ii = 0; ii < sv_strongholds; ii++)
				ResetLocationSpecial(mx->StrongholdById(ii+1)->Location());
            
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

		m_gameover_t mxnight::CheckWinLoseConditions ( BOOL night )
		{
#ifndef _TME_DEMO_MODE_
			collections::entities collection;

			// scan the mission tables		
			collection = mx->objMissions;
			collection.Sort(1);
			for ( u32 ii=0; ii<collection.Count(); ii++ ) {
				mxmission* m = (mxmission*)collection[ii];
                if ( !night && m->Condition() != MC_CHARACTER_DEAD )
                    continue;
				m->CheckComplete();
			}

			// scan the victory tables
			collection = mx->objVictories;
			collection.Sort(1);
			for ( u32 ii=0; ii<collection.Count(); ii++ ) {
				mxvictory* v = (mxvictory*)collection[ii];
				if ( v->CheckComplete() ) {
					// display message
					chilli::lib::strcpy ( mx->LastActionMsg(), mx->text->CookedSystemString(v->Message()) );
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
        
            int p = random(NUMELE(locations)-1);
            
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
    
        
        int test = 6;
        
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
                for ( u32 ii=0; ii<mx->objRegiments.Count(); ii++ )
                    ((mxregiment*)mx->objRegiments[ii])->Total(0);
                
                luxor->warriors.Total(1200);
                luxor->riders.Total(1200);
                luxor->Location( sh_ushgarak->Location() );
                
                mx->battle->KickOffAtLocation(luxor->Location());
                //sh_ushgarak->Total(10);
                
                break;

            case 6:
                // ushgarak fallen
                //
                for ( u32 ii=0; ii<mx->objRegiments.Count(); ii++ )
                    ((mxregiment*)mx->objRegiments[ii])->Total(0);
                
                luxor->warriors.Total(1200);
                luxor->riders.Total(1200);
                luxor->Location( sh_ushgarak->Location() );
                
                sh_ushgarak->Total(10);
                
                mx->battle->KickOffAtLocation(luxor->Location());
                
                // morkin & farflame
                farflame->Location( morkin->Location() ) ;
                
                break;
        
            case 7:
                // xajorkith fallen
                //
                //for ( u32 ii=0; ii<mx->objRegiments.Count(); ii++ )
                //    ((mxregiment*)mx->objRegiments[ii])->Total(0);
                
                ((mxregiment*)mx->objRegiments[0])->Location( sh_xajorkith->Location() );
                
                //luxor->warriors.Total(1200);
                //luxor->riders.Total(1200);
                //luxor->Location( ushgarak->Location() );
                
                sh_xajorkith->Total(10);

                morkin->Cmd_Dead();
                
                xajorkith->Location( luxor->Location() );
                mx->battle->KickOffAtLocation(luxor->Location());
                break;
                
                
        }
     
    }
#endif //_TEST_WINLOSE_CONDITIONS_
    
	//}
	// namespace processors
}
// namespace tme
