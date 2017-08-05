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
		int				ii;
		ddr_character*		character;
        mxstronghold*       stronghold;

			mx->scenario->NightStart();
			mx->battle->ResetBattlesFought();
			mx->scenario->RemoveMapArmies();
            
            // place all lords at dawn
			for (ii = 0; ii < sv_characters; ii++) {
				character = static_cast<ddr_character*>(mx->CharacterById(ii+1)) ;
                if ( character->IsDead())
                    continue;
                
				character->StartDawn();
#if !defined( _TME_DEMO_MODE_ )
                character->Turn();
#endif
			}
            
            
            static_cast<ddr_battle*>(mx->battle)->War();
            
            // put critters on the map
            mx->gamemap->PutThingsOnMap();

            // move mist
            mx->gamemap->MoveMists();
            
            // tidy up

            // characters
			for (ii = 0; ii < sv_characters; ii++) {
                character = static_cast<ddr_character*>(mx->CharacterById(ii+1));
                if ( character->IsDead())
                    continue;
                
                
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
                for ( u32 ii=0; ii<sv_characters; ii++ ) {
                    mxcharacter* c = mx->CharacterById(ii+1);
                    if ( (c->Location().x != character->Location().x) || (c->Location().y != character->Location().y) )
                        continue;
                    if ( c->Loyalty() != character->Loyalty() )
                        enemies++;
                }
                mxstronghold* stronghold = mx->scenario->StrongholdFromLocation(character->Location());
                if ( stronghold && stronghold->Loyalty()!=character->Loyalty())
                    enemies++;
                if ( enemies == 0 )
                    character->Flags().Reset(cf_preparesbattle/*|cf_inbattle*/);
            }

            // strongholds
            for (ii = 0; ii < sv_strongholds; ii++) {
                stronghold = mx->StrongholdById(ii+1);
                stronghold->OnRespawn();
            }

            // check morkin dead
            // check shareth dead
            checkImportantCharactersDead();
            
            MoveMidwinter();
            
            mx->scenario->SetMapArmies();
            mx->scenario->NightStop();
		}
    
        void ddr_night::checkImportantCharactersDead(void)
        {
            ddr_character* ch_shareth = static_cast<ddr_character*>( mx->EntityByName("CH_SHARETH") );
            //ch_shareth->Cmd_Dead();
            //ch_shareth->killedbyobject = (mxobject*)mx->EntityByName("OB_WOLVES");
            
            if ( ch_shareth->IsDead() ) {
                chilli::lib::strcat ( mx->LastActionMsg(), mx->text->CookedSystemString(SS_CHARACTER_DEAD,ch_shareth) );
                mx->NightCallback(NULL);
            }
            
            ddr_character* ch_morkin= static_cast<ddr_character*>( mx->EntityByName("CH_MORKIN") );
            //ch_morkin->Cmd_Dead();
            //ch_morkin->fighting_against=ch_shareth;
            if ( ch_morkin->IsDead() ) {
                chilli::lib::strcat ( mx->LastActionMsg(), mx->text->CookedSystemString(SS_CHARACTER_DEAD,ch_morkin) );
                mx->NightCallback(NULL);
            }
        }
    
    
        void ddr_night::MoveMidwinter ( void )
        {
            static tme::loc_t locations[] = { loc_t(38,18), loc_t(41,21), loc_t(42,15), loc_t(46,14), loc_t(49,13) }; // SHOULD BE USING PLACE SYMBOLS
        
            int p = random(NUMELE(locations)-1);
            
            mxcharacter* c = (mxcharacter*)mx->EntityByName("CH_MIDWINTER");
            if ( c==NULL )
                return;
        
            c->Location(locations[p]);
            
        }
    

        void ddr_night::describeVictory ( void )
        {
            if ( victoryMode == WIN_NONE )
                return;
            
            mxcharacter* ch_luxor = static_cast<mxcharacter*>(mx->EntityByName("CH_LUXOR"));
            mxcharacter* ch_morkin =  static_cast<mxcharacter*>(mx->EntityByName("CH_MORKIN"));
            mxcharacter* ch_tarithel =  static_cast<mxcharacter*>(mx->EntityByName("CH_TARITHEL"));
            mxcharacter* ch_rorthron =  static_cast<mxcharacter*>(mx->EntityByName("CH_RORTHRON"));
            mxcharacter* ch_shareth =  static_cast<mxcharacter*>(mx->EntityByName("CH_SHARETH"));

            
            // SS_VICTORY2
            // After {special:days} {plural:day:days} the warriors of Midnight return to the frozen gates.
            chilli::lib::strcpy ( mx->LastActionMsg(), mx->text->CookedSystemString(SS_VICTORY2) );

            if (victoryFlags.Is(wf_luxor_home)) {
                // luxor is here
                chilli::lib::strcat( mx->LastActionMsg(), mx->text->CookedSystemString(SS_CHARACTER_HERE,ch_luxor) );
            }
      
            if (victoryFlags.Is(wf_morkin_home)) {
                // morkin is here
                chilli::lib::strcat( mx->LastActionMsg(), mx->text->CookedSystemString(SS_CHARACTER_HERE,ch_morkin) );
            }

            if (victoryFlags.Is(wf_tarithel_home)) {
                // tarithel is here
                chilli::lib::strcat( mx->LastActionMsg(), mx->text->CookedSystemString(SS_CHARACTER_HERE,ch_tarithel) );
            }
            
            if (victoryFlags.Is(wf_rorthron_home)) {
                // and rorthron is here
                chilli::lib::strcat( mx->LastActionMsg(), mx->text->CookedSystemString(SS_CHARACTER_HERE,ch_rorthron) );
            }
            
            if (victoryFlags.Is(wf_shareth_dead)) {
                // shareth is dead
                chilli::lib::strcat( mx->LastActionMsg(), mx->text->CookedSystemString(SS_SHARETH_DEAD,ch_shareth) );

            }
            
            // describe victory
            ddr_text* text = static_cast<ddr_text*>(mx->text);
            chilli::lib::strcat( mx->LastActionMsg(), text->DescribeVictory(victoryTargets, victoryMode) );
            
            if ( victoryMode == WIN_NOBLE ) {
                // the victory is noble
                // SS_VICTORY4

            }else if ( victoryMode == WIN_WORTHY ) {
                // the victory is worthy
                // SS_VICTORY4
                
            } else if ( victoryMode == WIN_OVERWHELMING ) {
                // the victory is overwhelming
                // SS_VICTORY4
                // SS_VICTORY5
                chilli::lib::strcat( mx->LastActionMsg(),mx->text->CookedSystemString(SS_VICTORY5));
            }
            
            
        }
    
        m_gameover_t ddr_night::CheckWinLoseConditions ( BOOL night )
        {
            victoryTargets=WIN_MAX_TARGETS;
            victoryFlags.Clear();
            victoryMode=WIN_NONE;
            
            
            //0 - utterly
            //1 - extremely
            //2 - very
            //3 - quite
            //4 - somewhat
            //5 - slightly
            //6 - not
            //7 - not at all
            
            mxcharacter* ch_luxor = static_cast<mxcharacter*>(mx->EntityByName("CH_LUXOR"));
            mxcharacter* ch_morkin =  static_cast<mxcharacter*>(mx->EntityByName("CH_MORKIN"));
            mxcharacter* ch_tarithel =  static_cast<mxcharacter*>(mx->EntityByName("CH_TARITHEL"));
            mxcharacter* ch_rorthron =  static_cast<mxcharacter*>(mx->EntityByName("CH_RORTHRON"));
            mxcharacter* ch_shareth =  static_cast<mxcharacter*>(mx->EntityByName("CH_SHARETH"));

            if ( ch_luxor->IsDead() ) {
                chilli::lib::strcpy ( mx->LastActionMsg(), mx->text->CookedSystemString(SS_DEFEAT2,ch_luxor) );
                return MG_LOSE;
            }
            
            
            c_ptr characters;
            characters.Add(ch_luxor);             // bit 4
            characters.Add(ch_morkin);            // bit 3
            characters.Add(ch_tarithel);          // bit 2
            characters.Add(ch_rorthron);          // bit 1
            
            c_ptr objects;
            objects.Add( mx->EntityByName("OB_CROWN_VARENAND"));
            objects.Add( mx->EntityByName("OB_CROWN_CARUDRIUM"));
            objects.Add( mx->EntityByName("OB_SPELL_THIGRORN"));
            objects.Add( mx->EntityByName("OB_RUNES_FINORN"));
            objects.Add( mx->EntityByName("OB_CROWN_IMIRIEL"));
            
            victoryTargets=WIN_MAX_TARGETS;
            victoryFlags.Clear();
            victoryMode=WIN_WORTHY; // worthy
            
            mxgridref gate_varenorn = mxgridref(6,92); // SHOULD BE USING PLACE SYMBOL
            
            for ( u32 ii=0; ii<NUMELE(characters); ii++ ) {
                mxcharacter* c = static_cast<mxcharacter*>( characters[ii] ) ;
                
                if ( c->IsAlive() && c->Loyalty() == RA_MOONPRINCE) {
                    if ( c->Location() == gate_varenorn ) {
                        
                        // is main character home?
                        victoryTargets--;
                        victoryFlags.Set(1);
                        
                        // are we carrying a special object?
                        if ( c->Carrying() && objects.IsInList(c->Carrying()) ) {
                            victoryTargets--;
                        }
                    }
                }
                victoryFlags.ShiftLeft();
            }
            
            // we want shareth dead
            if ( ch_shareth->IsDead() ) {
                victoryTargets--;
                victoryFlags.Set(1);
                victoryMode=WIN_NOBLE; // noble
            }
            
            // if we overflow, then we are damn good!
            if ( victoryTargets<0 )
                victoryTargets=0;
            
            // all main characters home and shareth is dead
            if ( victoryFlags == wf_all ) {
                victoryMode = WIN_OVERWHELMING;
                describeVictory();
                return MG_WIN;
            }
            
            //
            // unless luxor is home, then we can't win?
            if ( !victoryFlags.Is(wf_luxor_home) ) {
                victoryMode=WIN_NONE;
                return MG_NONE;
            }

            // is morkin not home, shareth alive
            if ( !victoryFlags.Is(wf_morkin_home|wf_shareth_dead) ) {
                victoryMode=WIN_NONE;
                return MG_NONE;
            }

            // either morkin is home
            // or shareth is dead
            describeVictory();
            return MG_WIN;
        }
    
#ifdef _TEST_WINLOSE_CONDITIONS_
        void ddr_night::testWinLoseConditions ( void )
        {
            
            ddr_character* ch_luxor = static_cast<ddr_character*>(mx->EntityByName("CH_LUXOR"));
            ddr_character* ch_morkin =  static_cast<ddr_character*>(mx->EntityByName("CH_MORKIN"));
            ddr_character* ch_tarithel =  static_cast<ddr_character*>(mx->EntityByName("CH_TARITHEL"));
            ddr_character* ch_rorthron =  static_cast<ddr_character*>(mx->EntityByName("CH_RORTHRON"));
            ddr_character* ch_shareth =  static_cast<ddr_character*>(mx->EntityByName("CH_SHARETH"));
       
//            objects.Add( mx->EntityByName("OB_CROWN_VARENAND"));
//            objects.Add( mx->EntityByName("OB_CROWN_CARUDRIUM"));
//            objects.Add( mx->EntityByName("OB_SPELL_THIGRORN"));
//            objects.Add( mx->EntityByName("OB_RUNES_FINORN"));
//            objects.Add( mx->EntityByName("OB_CROWN_IMIRIEL"));

            mxgridref gate_varenorn = mxgridref(6,92);
            
            int test = 3;
            
            mx->battle->ResetBattlesFought();
            
            switch ( test ) {
                case 1:
                {
                    ch_luxor->Cmd_Dead();
                    ch_luxor->killedbyobject = static_cast<mxobject*>(mx->EntityByName("OB_WOLVES"));
                    break;
                }
                case 2:
                {
                    ch_luxor->Cmd_Dead();
                    ch_luxor->killedbyobject = NULL;
                    break;
                }
                case 3:
                {
                    ch_luxor->Cmd_Dead();
                    ch_luxor->fighting_against=ch_shareth;
                    break;
                }
                    
                case 4:
                {
                    // all main lords back to gate
                    // shareth dead
                    ch_shareth->Cmd_Dead();
                    ch_shareth->killedbyobject = static_cast<mxobject*>(mx->EntityByName("OB_WOLVES"));
                    
                    ch_morkin->Recruited(ch_tarithel);
                    ch_luxor->Location(gate_varenorn);
                    ch_morkin->Location(gate_varenorn);
                    ch_tarithel->Location(gate_varenorn);
                    ch_rorthron->Location(gate_varenorn);
                    
                    
                    break;
                }
                case 5:
                {
                    // all main lords back to gate
                    // shareth alive
                    
                    ch_morkin->Recruited(ch_tarithel);
                    ch_luxor->Location(gate_varenorn);
                    ch_morkin->Location(gate_varenorn);
                    ch_tarithel->Location(gate_varenorn);
                    ch_rorthron->Location(gate_varenorn);
                    
                    break;
                }
                case 6:
                {
                    // 3 main lords back to gate
                    // luxor not home
                    // shareth alive
                    // NO VICTORY
                    ch_morkin->Recruited(ch_tarithel);
                    ch_luxor->Location(mxgridref(0,0));
                    ch_morkin->Location(gate_varenorn);
                    ch_tarithel->Location(gate_varenorn);
                    ch_rorthron->Location(gate_varenorn);
                    
                    break;
                }
                case 7:
                {
                    // 3 main lords back to gate
                    // morkin not hom
                    // shareth alive
                    // NO VICTORY
                    ch_morkin->Recruited(ch_tarithel);
                    ch_luxor->Location(gate_varenorn);
                    ch_morkin->Location(mxgridref(0,0));
                    ch_tarithel->Location(gate_varenorn);
                    ch_rorthron->Location(gate_varenorn);
                    break;
                }
                case 8:
                {
                    // 3 main lords back to gate
                    // morkin not home
                    // shareth dead
                    ch_shareth->Cmd_Dead();
                    ch_shareth->killedbyobject = static_cast<mxobject*>(mx->EntityByName("OB_WOLVES"));
                    
                    ch_morkin->Recruited(ch_tarithel);
                    ch_luxor->Location(gate_varenorn);
                    ch_morkin->Location(mxgridref(0,0));
                    ch_tarithel->Location(gate_varenorn);
                    ch_rorthron->Location(gate_varenorn);
                    break;
                }
            }
            
            
        }
#endif //_TEST_WINLOSE_CONDITIONS_
    
	//}
	// namespace processors
}
// namespace tme

#endif // _DDR_
