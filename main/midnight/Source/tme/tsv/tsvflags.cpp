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
        bool matched = false;
        for ( size_t ii=0; ii<count; ii++ ) {
            if ( chilli::lib::c_stricmp(token.c_str(), table[ii].name) == 0 ) {
                result |= table[ii].bit;
                matched = true;
                break;
            }
        }
        if ( !matched )
            MXTRACE("TsvFlags: unknown flag '%s' in '%s'", token.c_str(), text.c_str());
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
    { "BATTLEOVER",          cf_battleover },
    { "PRISONER",            cf_prisoner },
    { "MAJOR",               cf_major },
    { "LOCATION",            cf_location },
    { "FRIENDLY",            cf_friendly },
    { "INTEREST",            cf_interest },
    { "KNOWLEDGE",           cf_knowledge },
    { "CONTROL",             cf_control },
    { "WATCH",               cf_watch },
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

#define QB(n)   (((u64)1) << (n))

static const u64 QUALITY_PAIRED_LOW  = 0x0000000003FFFFFFull;   // bits 0-25
static const u64 QUALITY_PAIRED_HIGH = QUALITY_PAIRED_LOW << 32; // bits 32-57

struct NamedBit64 { const char* name; u64 bit; };

static const NamedBit64 CharacterQualityBits[] = {
    { "BRAVE",          QB( 0) },  { "COWARDLY",      QB(32) },
    { "LOYAL",          QB( 1) },  { "TREACHEROUS",   QB(33) },
    { "SELFLESS",       QB( 2) },  { "SELFISH",       QB(34) },
    { "QUICKWITTED",    QB( 3) },  { "SLOWWITTED",    QB(35) },
    { "KIND",           QB( 4) },  { "CRUEL",         QB(36) },
    { "GENEROUS",       QB( 5) },  { "GREEDY",        QB(37) },
    { "PATIENT",        QB( 6) },  { "IMPATIENT",     QB(38) },
    { "KNOWLEDGEABLE",  QB( 7) },  { "IGNORANT",      QB(39) },
    { "PERSUASIVE",     QB( 8) },  { "UNCONVINCING",  QB(40) },
    { "MIGHTYWARRIOR",  QB( 9) },  { "FEEBLEWARRIOR", QB(41) },
    { "MODEST",         QB(10) },  { "ARROGANT",      QB(42) },
    { "FARSIGHTED",     QB(11) },  { "NAIVE",         QB(43) },
    { "ENERGETIC",      QB(12) },  { "LANGUID",       QB(44) },
    { "CHARMING",       QB(13) },  { "REPULSIVE",     QB(45) },
    { "WARMHEARTED",    QB(14) },  { "COLDHEARTED",   QB(46) },
    { "TIRELESS",       QB(15) },  { "EASILYTIRED",   QB(47) },
    { "GENTLE",         QB(16) },  { "VICIOUS",       QB(48) },
    { "PEACEABLE",      QB(17) },  { "BLOODTHIRSTY",  QB(49) },
    { "POLITE",         QB(18) },  { "PROVOCATIVE",   QB(50) },
    { "PASSIONATE",     QB(19) },  { "PASSIONLESS",   QB(51) },
    { "LEVELHEADED",    QB(20) },  { "HOTTEMPERED",   QB(52) },
    { "CAUTIOUS",       QB(21) },  { "RECKLESS",      QB(53) },
    { "TALKATIVE",      QB(22) },  { "TIGHTLIPPED",   QB(54) },
    { "HOMELOVING",     QB(23) },  { "RESTLESS",      QB(55) },
    { "EAGER",          QB(24) },  { "APATHETIC",     QB(56) },
    { "DOMINEERING",    QB(25) },  { "SUBMISSIVE",    QB(57) },
    { "BOLD",           QB(26) },
    { "GALLANT",        QB(27) },
    { "HEADSTRONG",     QB(28) },
    { "RELIABLE",       QB(29) },
    { "SUPERBLEADER",   QB(30) },
    { "SOLITARY",       QB(31) },
    { "SCEPTICAL",      QB(58) },
    { "THICKSKINNED",   QB(59) },
    { "MAD",            QB(60) },
    { "MALICIOUS",      QB(61) },
};

static u64 ParseFlagWord64 ( const std::string& text, const NamedBit64* table, size_t count )
{
    u64 result = 0;
    if ( text.empty() )
        return result;

    chilli::collections::c_string tokens;
    chilli::lib::StringExtensions::split(text, '+', tokens);

    for ( auto& token : tokens ) {
        bool matched = false;
        for ( size_t ii=0; ii<count; ii++ ) {
            if ( chilli::lib::c_stricmp(token.c_str(), table[ii].name) == 0 ) {
                result |= table[ii].bit;
                matched = true;
                break;
            }
        }
        // Traced, never silently dropped - #4 records what a silent drop cost last time.
        if ( !matched )
            MXTRACE("TsvFlags: unknown quality '%s' in '%s'", token.c_str(), text.c_str());
    }

    return result;
}

static s32 CountBits ( u64 v )
{
    s32 n = 0;
    while ( v ) { v &= v - 1; n++; }
    return n;
}

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
#if defined(_DDR_)
    { "FollowLiege",    OD_FOLLOW_LIEGE },
    { "FollowFoe",      OD_FOLLOW_FOE },
    { "FindObject",     OD_FIND_OBJECT },
    { "Home",           OD_HOME },
#endif
};

u32 ParseEntityFlags ( const std::string& text )       { return ParseFlagWord(text, EntityFlagBits, NUMELE(EntityFlagBits)); }
u32 ParseCharacterFlags ( const std::string& text )    { return ParseFlagWord(text, CharacterFlagBits, NUMELE(CharacterFlagBits)); }
u32 ParseCharacterTraits ( const std::string& text )   { return ParseFlagWord(text, CharacterTraitBits, NUMELE(CharacterTraitBits)); }
u64 ParseCharacterQualities ( const std::string& text ) { return ParseFlagWord64(text, CharacterQualityBits, NUMELE(CharacterQualityBits)); }

s32 QualityAffinity ( u64 a, u64 b )
{
    // A quality's opposite is the same bit 32 places away, so one shift finds every
    // opposition at once (see the layout note above).
    const u64 opposed = ((b & QUALITY_PAIRED_LOW) << 32) | ((b & QUALITY_PAIRED_HIGH) >> 32);
    return CountBits(a & b) - CountBits(a & opposed);
}
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
