//
//  tsvflags.h
//  midnight
//
//  Name <-> bit / value tables for the flag words and small compile-time
//  enums that appear as plain text in the .tsv files (e.g. Flags column
//  "RIDING+ALIVE+RECRUITED", or a Mission's Condition "StrongholdTaken").
//  Symbol-table driven references (Race, Gender, Direction, Terrain,
//  Command, UnitType, and any entity cross-reference) are resolved via
//  TsvSymbolTable instead - these are only for values with no backing
//  entity table.
//

#ifndef _TSVFLAGS_H_INCLUDED_
#define _TSVFLAGS_H_INCLUDED_

#include <string>
#include "../../library/inc/mxtypes.h"
#include "../baseinc/lomxtypes.h"

namespace tme {

    // base et_disabled bit shared by every entity type
    u32 ParseEntityFlags ( const std::string& text );

    u32 ParseCharacterFlags ( const std::string& text );
    u32 ParseCharacterTraits ( const std::string& text );

    u64 ParseCharacterQualities ( const std::string& text );

    s32 QualityAffinity ( u64 a, u64 b );
    u32 ParseObjectFlags ( const std::string& text );
    u32 ParseMissionFlags ( const std::string& text );
    u32 ParseVictoryFlags ( const std::string& text );
    u32 ParseTerrainInfoFlags ( const std::string& text );

    m_objective_t ParseMissionObjective ( const std::string& text );
    m_condition_t ParseMissionCondition ( const std::string& text );
    m_action_t    ParseMissionAction ( const std::string& text );

    // shared by regiments and characters
    mxorders_t ParseOrders ( const std::string& text );

}
// namespace tme

#endif //_TSVFLAGS_H_INCLUDED_
