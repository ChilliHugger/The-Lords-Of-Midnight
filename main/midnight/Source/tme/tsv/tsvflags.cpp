//
//  tsvflags.cpp
//  midnight
//

#include "../baseinc/tme_internal.h"
#include "tsvflags.h"

namespace tme {

struct NamedBit { const char* name; u32 bit; };
struct NamedValue { const char* name; u32 value; };

static u32 ParseFlagWord ( const std::string& text, const NamedBit* table, size_t count )
{
    u32 result = 0;
    if ( text.empty() )
        return result;

    chilli::collections::c_string tokens;
    chilli::lib::StringExtensions::split(text, '+', tokens);

    for ( auto& token : tokens ) {
        for ( size_t ii=0; ii<count; ii++ ) {
            if ( chilli::lib::c_stricmp(token.c_str(), table[ii].name) == 0 ) {
                result |= table[ii].bit;
                break;
            }
        }
    }

    return result;
}

static u32 ParseNamedValue ( const std::string& text, const NamedValue* table, size_t count, u32 def )
{
    if ( text.empty() )
        return def;

    for ( size_t ii=0; ii<count; ii++ ) {
        if ( chilli::lib::c_stricmp(text.c_str(), table[ii].name) == 0 )
            return table[ii].value;
    }

    return def;
}

static const NamedBit EntityFlagBits[] = {
    { "DISABLED", (u32)et_disabled },
};

static const NamedBit CharacterFlagBits[] = {
    { "HIDDEN",              cf_hidden },
    { "RIDING",              cf_riding },
    { "ALIVE",               cf_alive },
    { "RECRUITED",           cf_recruited },
    { "ARMY",                cf_army },
    { "HIDE",                cf_hide },
    { "HORSE",               cf_horse },
    { "MOONRING",            cf_moonring },
    { "ICECROWN",            cf_icecrown },
    { "CANDESTROYICECROWN",  cf_destroyicecrown },
    { "ALLOWEDWARRIORS",     cf_allowedwarriors },
    { "ALLOWEDRIDERS",       cf_allowedriders },
    { "AI",                  cf_ai },
    { "KILLEDFOE",           cf_killed_foe },
    { "RESTING",             cf_resting },
    { "INBATTLE",            cf_inbattle },
    { "WONBATTLE",           cf_wonbattle },
    { "TUNNEL",              cf_tunnel },
    { "USEDOBJECT",          cf_usedobject },
    { "HASFOLLOWERS",        cf_followers },
    { "PREPARESBATTLE",      cf_preparesbattle },
    { "APPROACHING",         cf_approaching },
};

static const NamedBit CharacterTraitBits[] = {
    { "GOOD",        ct_good },
    { "STRONG",      ct_strong },
    { "FORCEFUL",    ct_forceful },
    { "GENEROUS",    ct_generous },
    { "STUBBORN",    ct_stubborn },
    { "BRAVE",       ct_brave },
    { "SWIFT",       ct_swift },
    { "LOYAL",       ct_loyal },
    { "EVIL",        ct_evil },
    { "WEAK",        ct_weak },
    { "RETICENT",    ct_reticent },
    { "GREEDY",      ct_greedy },
    { "FAWNING",     ct_fawning },
    { "COWARD",      ct_coward },
    { "SLOW",        ct_slow },
    { "TREACHEROUS", ct_treacherous },
};

static const NamedBit ObjectFlagBits[] = {
    { "FIGHT",   of_fight },
    { "PICKUP",  of_pickup },
    { "DROP",    of_drop },
    { "WEAPON",  of_weapon },
    { "SEE",     of_see },
    { "REMOVE",  of_remove },
    { "UNIQUE",  of_unique },
    { "ENABLED", of_enabled },
};

static const NamedBit MissionFlagBits[] = {
    { "COMPLETE", mf_complete },
    { "ENABLED",  MXBIT(1) },    // mf_enabled - reserved slot, not read by current game logic
    { "ANY",      mf_any },
};

static const NamedBit VictoryFlagBits[] = {
    { "GAMEOVER", vf_gameover },
    { "ENABLED",  MXBIT(1) },    // vf_enabled - reserved slot, not read by current game logic
    { "COMPLETE", vf_complete },
};

static const NamedBit TerrainInfoFlagBits[] = {
    { "PLURAL",      tif_plural },
    { "BLOCK",       tif_block },
    { "INTERESTING", tif_interesting },
    { "ARMY",        tif_army },
};

static const NamedValue MissionObjectiveValues[] = {
    { "None",  MO_NONE },
    { "Minor", MO_MINOR },
    { "Major", MO_MAJOR },
    { "Flag",  MO_FLAG },
};

static const NamedValue MissionConditionValues[] = {
    { "None",             MC_NONE },
    { "StrongholdTaken",  MC_STRONGHOLD_TAKEN },
    { "DestroyObject",    MC_DESTROY_OBJECT },
    { "CharacterDead",    MC_CHARACTER_DEAD },
    { "MissionComplete",  MC_MISSION_COMPLETE },
    { "StrongholdFallen", MC_STRONGHOLD_FALLEN },
    { "SameLocation",     MC_SAME_LOCATION },
};

static const NamedValue MissionActionValues[] = {
    { "None",    MA_NONE },
    { "Disable", MA_DISABLE },
    { "Enable",  MA_ENABLE },
};

static const NamedValue OrdersValues[] = {
    { "None",           OD_NONE },
    { "Goto",           OD_GOTO },
    { "Wander",         OD_WANDER },
    { "Follow",         OD_FOLLOW },
    { "Route",          OD_ROUTE },
    { "DelayedWander",  OD_DELAYED_WANDER },
    { "Hold",           OD_HOLD },
};

u32 ParseEntityFlags ( const std::string& text )       { return ParseFlagWord(text, EntityFlagBits, NUMELE(EntityFlagBits)); }
u32 ParseCharacterFlags ( const std::string& text )    { return ParseFlagWord(text, CharacterFlagBits, NUMELE(CharacterFlagBits)); }
u32 ParseCharacterTraits ( const std::string& text )   { return ParseFlagWord(text, CharacterTraitBits, NUMELE(CharacterTraitBits)); }
u32 ParseObjectFlags ( const std::string& text )       { return ParseFlagWord(text, ObjectFlagBits, NUMELE(ObjectFlagBits)); }
u32 ParseMissionFlags ( const std::string& text )      { return ParseFlagWord(text, MissionFlagBits, NUMELE(MissionFlagBits)); }
u32 ParseVictoryFlags ( const std::string& text )      { return ParseFlagWord(text, VictoryFlagBits, NUMELE(VictoryFlagBits)); }
u32 ParseTerrainInfoFlags ( const std::string& text )  { return ParseFlagWord(text, TerrainInfoFlagBits, NUMELE(TerrainInfoFlagBits)); }

m_objective_t   ParseMissionObjective ( const std::string& text )  { return (m_objective_t)ParseNamedValue(text, MissionObjectiveValues, NUMELE(MissionObjectiveValues), MO_NONE); }
m_condition_t   ParseMissionCondition ( const std::string& text )  { return (m_condition_t)ParseNamedValue(text, MissionConditionValues, NUMELE(MissionConditionValues), MC_NONE); }
m_action_t      ParseMissionAction ( const std::string& text )     { return (m_action_t)ParseNamedValue(text, MissionActionValues, NUMELE(MissionActionValues), MA_NONE); }
mxorders_t      ParseOrders ( const std::string& text )            { return (mxorders_t)ParseNamedValue(text, OrdersValues, NUMELE(OrdersValues), OD_NONE); }

}
// namespace tme
