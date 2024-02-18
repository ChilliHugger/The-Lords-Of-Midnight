//
//  lom_processor_gameover.cpp
//  midnight
//
//  Created by Chris Wild on 13/10/2023.
//
#include "../../baseinc/tme_internal.h"
#include "lom_gameover.h"

namespace tme {
        lom_gameover::lom_gameover()
        {
        }

        lom_gameover::~lom_gameover()
        {
        }

        m_gameover_t lom_gameover::Process ( bool night )
        {
#ifndef _TME_DEMO_MODE_
            // scan the mission tables
            auto missions = mx->objMissions.Clone();
            missions.Sort(mxmission::SORT::PRIORITY);
            for ( auto m : missions ) {
//                MXTRACE("%s[%d] %d Complete '%s'", m->Symbol().c_str(), (int)m->Id(), (int)m->Priority(),  m->IsComplete() ?  "YES" : "NO" );
                CONTINUE_IF ( !night && m->Condition() != MC_CHARACTER_DEAD );
                m->CheckComplete();
//                MXTRACE("Complete '%s'", m->IsComplete() ?  "YES" : "NO" );
            }

            // scan the victory tables
            auto victories = mx->objVictories.Clone();
            victories.Sort(mxvictory::SORT::PRIORITY);
                        
//            for ( auto v : victories ) {
//                MXTRACE("%s[%d] %d Complete='%s' '%s'",
//                    v->Symbol().c_str(),
//                    (int)v->Id(),
//                    (int)v->Priority(),
//                    v->IsComplete() ?  "YES" : "NO",
//                    mx->text->CookedSystemString(v->Message()).c_str() );
//                v->CheckComplete();
//            }
            
            for ( auto v : victories ) {
//                MXTRACE("%s[%d] %d Complete='%s'", v->Symbol().c_str(), (int)v->Id(), (int)v->Priority(), v->IsComplete() ?  "YES" : "NO" );
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
        
}
// namespace tme
