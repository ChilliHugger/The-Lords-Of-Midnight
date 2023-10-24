//
//  ddr_gameover.cpp
//  revenge
//
//  Created by Chris Wild on 13/10/2023.
//

#include "ddr_gameover.h"
#include "scenario_ddr_internal.h"
#include "ddr_processor_text.h"

#if defined(_DDR_)

namespace tme {
    ddr_gameover::ddr_gameover()
    {
    }

    ddr_gameover::~ddr_gameover()
    {
    }

    void ddr_gameover::checkImportantCharactersDead(void)
    {
    std::string buffer = mx->LastActionMsg();
    
        auto ch_shareth = DDR_SCENARIO(shareth);
        if ( ch_shareth->IsDead() ) {
            buffer += mx->text->CookedSystemString(SS_CHARACTER_DEAD,ch_shareth);
            mx->NightCallback(NULL);
        }
        
        auto ch_morkin = DDR_SCENARIO(morkin);
        if ( ch_morkin->IsDead() ) {
            buffer += mx->text->CookedSystemString(SS_CHARACTER_DEAD,ch_morkin);
            mx->NightCallback(NULL);
        }
        mx->SetLastActionMsg(buffer);
    }

    void ddr_gameover::describeVictory ( WINMODE mode, flags8 flags, s32 targets )
    {
        if ( mode == WIN_NONE )
            return;
        
        // SS_VICTORY2
        // After {special:days} {plural:day:days} the warriors of Midnight return to the frozen gates.
        
        auto buffer = mx->text->CookedSystemString(SS_VICTORY2);

        if (flags.Is(wf_luxor_home)) {
            // luxor is here
            buffer += mx->text->CookedSystemString(SS_CHARACTER_HERE,DDR_SCENARIO(luxor));
        }
  
        if (flags.Is(wf_morkin_home)) {
            // morkin is here
            buffer += mx->text->CookedSystemString(SS_CHARACTER_HERE,DDR_SCENARIO(morkin));
        }

        if (flags.Is(wf_tarithel_home)) {
            // tarithel is here
            buffer += mx->text->CookedSystemString(SS_CHARACTER_HERE,DDR_SCENARIO(tarithel));
        }
        
        if (flags.Is(wf_rorthron_home)) {
            // and rorthron is here
            buffer += mx->text->CookedSystemString(SS_CHARACTER_HERE,DDR_SCENARIO(rorthron));
        }
        
        if (flags.Is(wf_shareth_dead)) {
            // shareth is dead
            buffer += mx->text->CookedSystemString(SS_SHARETH_DEAD,DDR_SCENARIO(shareth));
        }
        
        // describe victory
        auto text = static_cast<ddr_text*>(mx->text);
        //
        // 0 utterly
        // 1 extremely << all home, shareth dead
        // 2 very
        // 3 quite
        // 4 somewhat << Min 6 - Luxor Home, Shareth Dead
        
        buffer += text->DescribeVictory(targets, mode);
        
        if ( mode == WIN_NOBLE ) {
            // the victory is noble
            // SS_VICTORY4

        }else if ( mode == WIN_WORTHY ) {
            // the victory is worthy
            // SS_VICTORY4
            
        } else if ( mode == WIN_OVERWHELMING ) {
            // the victory is overwhelming
            // SS_VICTORY4
            // The Watchwords of Midnight are revealed!
            // SS_VICTORY5
            buffer += mx->text->CookedSystemString(SS_VICTORY5);
        }
        
        mx->SetLastActionMsg(buffer);
        
    }

    m_gameover_t ddr_gameover::Process ( bool night )
    {
        s32         targets = WIN_MAX_TARGETS;
        flags8      flags;
    
        //targets = WIN_MAX_TARGETS;
        //flags.Clear();
        
        //0 - utterly
        //1 - extremely
        //2 - very
        //3 - quite
        //4 - somewhat
        //5 - slightly
        //6 - not
        //7 - not at all
        
        auto ch_luxor = DDR_SCENARIO(luxor);
        if ( ch_luxor->IsDead() ) {
            mx->SetLastActionMsg( mx->text->CookedSystemString( SS_DEFEAT2, ch_luxor) );
            return MG_LOSE;
        }
        
        auto ch_shareth = DDR_SCENARIO(shareth);
        
        c_character characters({
            ch_luxor,               // bit 4
            DDR_SCENARIO(morkin),   // bit 3
            DDR_SCENARIO(tarithel), // bit 2
            DDR_SCENARIO(rorthron)  // bit 1
        });

        c_object objects({
            DDR_SCENARIO(crownofvarenand),
            DDR_SCENARIO(crownofcarudrium),
            DDR_SCENARIO(spellofthigrorn),
            DDR_SCENARIO(runesoffinorn),
            DDR_SCENARIO(crownofimiriel)
        });
        
        WINMODE mode = WIN_WORTHY;
        
        mxgridref gate_varenorn = mxgridref(6,92); // SHOULD BE USING PLACE SYMBOL
        
        for ( auto c : characters ) {
            if ( c->IsAlive() && c->NormalisedLoyalty() == RA_MOONPRINCE) {
                if ( c->Location() == gate_varenorn ) {
                    
                    // is main character home?
                    targets--;
                    flags.Set(1);
                    
                    // are we carrying a special object?
                    if ( c->Carrying() && objects.IsInList(c->Carrying()) ) {
                        targets--;
                    }
                }
            }
            flags.ShiftLeft();
        }
        
        // we want shareth dead
        if ( ch_shareth->IsDead() ) {
            targets--;
            flags.Set(wf_shareth_dead);
            mode = WIN_NOBLE;
        }

        //
        // unless luxor is home, then we can't win?
        if ( !flags.Is(wf_luxor_home) ) {
            mode = WIN_NONE;
            return MG_NONE;
        }

        // is morkin not home, shareth alive
        if ( !flags.Is(wf_morkin_home) && !flags.Is(wf_shareth_dead) ) {
            mode = WIN_NONE;
            return MG_NONE;
        }

        // if we overflow, then we are damn good!
        if ( targets<0 )
            targets = 0;
        
        // all main characters home and shareth is dead
        if ( flags == wf_all ) {
            mode =  WIN_OVERWHELMING;
        }

        // either morkin is home
        // or shareth is dead
        describeVictory(mode, flags, targets);
        return MG_WIN;
    }
}

#endif

// namespace tme
