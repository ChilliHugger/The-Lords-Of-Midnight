//
//  main.cpp
//  citadel export
//
//  Created by Chris Wild on 25/07/2017.
//  Copyright © 2017 Chris Wild. All rights reserved.
//

#include <iostream>
#include "types.h"
#include "file.h"
#include "lom3_structs.h"

using namespace std;

FILE* output = NULL ;

void OutputData ( );
int OutputText ( );
void OutputChars ( void );
void OutputObjects ( void);
void OutputBuildings ( void);
void OutputBeasts ( void);
void OutputArmies ( void);
void OutputBoats ( void);

//auto root_path = "/users/chris/Dropbox/Midnight/The Citadel/data/black/";
auto root_path = "/dosbox/lom3/master/";




typedef struct flags_t {
    LPCSTR	name;
    long		value;
} flags_t ;


flags_t char_traits1[] = {
    { "brave",				1 },
    { "far-sighted",		2 },
    { "selfless",			4 },
    { "bold",				8 },
    { "kind",				16 },
    { "warm-hearted",		32 },
    { "gentle",				64 },
    { "generous",			128 },
    { "quick-witted",		256 },
    { "talkative",			512 },
    { "home-loving",		1024 },
    { "eager",				2048 },
    { "energetic",			4096 },
    { "tireless",			8192 },
    { "modest",				16384 },
    { "passionate",			32768 },
    { "reliable",			65536 },
    { "gallant",			131072 },
    { "charming",			262144 },
    { "loyal",				524288 },
    { "gregarious",			1048576 },
    { "peaceable",			2097152 },
    { "polite",				4194304 },
    { "submissive",			8388608 },
    { "patient",			16777216 },
    { "level-headed",		33554432 },
    { "persuasive",			67108864 },
    { "knowledgeable",		134217728 },
    { "naive",				268435456 },
    { "hot-tempered",		536870912 },
    { "a mighty warrior",	1073741824 },
    { "a superb leader",	2147483648 },
};

flags_t char_traits2[] = {
    { "cowardly",			1 },
    { "reckless",			2 },
    { "selfish",			4 },
    { "cautious",			8 },
    { "cruel",				16 },
    { "cold_hearted",		32 },
    { "vicious",			64 },
    { "greedy",				128 },
    { "slow-witted",		256 },
    { "tight-lipped",		512 },
    { "restless",			1024 },
    { "apathetic",			2048 },
    { "languid",			4096 },
    { "easilytired",		8192 },
    { "arrogant",			16384 },
    { "passionless",		32768 },
    { "headstrong",			65536 },
    { "malicious",			131072 },
    { "repulsive",			262144 },
    { "treacherous",		524288 },
    { "solitary",			1048576 },
    { "blood thirsty",		2097152 },
    { "provocative",		4194304 },
    { "domineering",		8388608 },
    { "impatient",			16777216 },
    { "mad",				33554432 },
    { "unconvincing",		67108864 },
    { "ignorant",			134217728 },
    { "sceptical",			268435456 },
    { "thick-skinned",		536870912 },
    { "a feeble warrior",	1073741824 },
    { "a pitiful leader",	2147483648 },
};

flags_t char_flags[] = {
    { "recruit",	1 },
    { "knowledge",	2 },
    { "dead",		4 },
    { "friendly",	8 },
    { "rescue",		16 },
    { "detail",		32 },
    { "interest",	64 },
    { "rest",		128 },
    { "visible",	256 },
    { "sink",		512 },
    { "major",		1024 },
    { "accel",		2048 },
    { "hidden",		4096 },
    { "control",	8192 },
    { "location",	16384 },
    { "watch",		32768 },
};

flags_t obj_flags[] = {
    { "height",			1 },
    { "static",			2 },
    { "obstacle",		4 },
    { "generated",		8 },
    { "anim",			16 },
    { "process",		32 },
    { "corridor",		64 },
    { "en_garde",		128 },
    { "combat",			256 },
    { "crowd_doorway",	512 },
    { "talking",		1024 },
    { "initiate",		2048 },
    { "inside",			4096 },
    { "seize",			8192 },
    { "available",		16384 },
    { "female",			32768 },
};

flags_t obj_flags2[] = {
    { "autoq",			1 },
    { "really_inside",	2 },
    { "shelter_stop",	4 },
    { "meet_autoq",		8 },
    { "seize_ready",	16 },
    { "no_process",		32 },
    { "near_collapse_mess",		64 },
    { "collapse_mess",	128 },
    { "seek_shelter",	256 },
    { "wait_party",		512 },
    { "was_hostage",	1024 },
};

LPCSTR text_weapon_type[] = {
     "none"
    ,"sword"
    ,"axe"
};

LPCSTR text_building[] = {
    "Citadel",
    "Castle",
    "Keep",
    "Tower"
};

LPCSTR data_type[] = {
      "character" // 0
    , "1" // 1
    , "2" // 2
    , "beast" //3
    , "castle" // 4
    , "5"
    , "citadel"
    , "7"
    , "8"
    , "9"
    , "10"
    , "army" // 11
    , "object" // 12
};


LPCSTR text_names[160][2] = {
    { "Wythran","the Weaver" },
    { "Arak","the Avenger" },
    { "Asunai","Ironwing" },
    { "Sadlak","the Merry" },
    { "Itar","the Green" },
    { "Melinoth","Larkstongue" },
    { "Rainar","the Besotted" },
    { "Amarin","Starchaser" },
    { "Kargrim","the Cautious" },
    { "Morathron","the Sorceror" },
    { "Guthrane","Oakfist" },
    { "Djalina","Snowheart" },
    { "Grumrud","Slowaxe" },
    { "Asholeth","the Fey" },
    { "Boroth","the Wolfheart" },
    { "Ildrar","the Stubborn" },
    { "Mogrik","the Witless" },
    { "Torgrim","Arrowhand" },
    { "Uthran","the Meddler" },
    { "Marik","Silktongue" },
    { "Haraglai","Stormgut" },
    { "Oglim","the Lonely" },
    { "Aloroth","the Fey" },
    { "Yrgreth","Deathbringer" },
    { "Stublog","Ironskull" },
    { "Orabrin","Lionsblood" },
    { "Sparthor","the Patient" },
    { "Cadron","the Bemused" },
    { "Brunak","the Wanderer" },
    { "Skarai","the Dreamer" },
    { "Rugrak","Firmaxe" },
    { "Morgreth","the Unsure" },
    { "Graleth","the Bitter" },
    { "Muglum","the Handsome" },
    { "Dorok","the Dour" },
    { "Scirane","the Swift" },
    { "Rorthron","the Wise" },
    { "Corlane","the Bear" },
    { "Moglai","Firewolf" },
    { "Norgrim","the Rock" },
    { "Jarleth","the Shining" },
    { "Urskreth","the Vile" },
    { "Borgalug","Bonecrusher" },
    { "Aranor","Boldsword" },
    { "Moongrim","Longneck" },
    { "Leonik","Leatherhand" },
    { "Storbold","the Scribbler" },
    { "Arbethor","Greenhand" },
    { "Luxor","the Moonprince" },
    { "Avila","the Cold" },
    { "Hoon","the Warrior" },
    { "Eothor","Sparehand" },
    { "Snorglum","Bighammer" },
    { "Orgrotha","the Persuader" },
    { "Forthar","the Hunter" },
    { "Joruk","Redfist" },
    { "Var","the Swordsman" },
    { "Morkin","Prince" },
    { "Carithila","Queen" },
    { "Garfin","Quicklip" },
    { "Zenethor","the Strong" },
    { "Gothrum","the Glum" },
    { "Skyrdreth","Iceheart" },
    { "Lanklin","the Boaster" },
    { "Holdar","Longeye" },
    { "Aramila","the Seer" },
    { "Arin","Lord Blood" },
    { "Mana","the Huntress" },
    { "Rorlbar","the Poet" },
    { "Garamor","the Fat" },
    { "Borlum","the Happy" },
    { "Akrith","Bloodhand" },
    { "Godrold","Heavyhand" },
    { "Darath","the Lion" },
    { "Parik","the Miser" },
    { "Merithel","of the Lake" },
    { "Aremela","Princess" },
    { "Farlik","Whiteknife" },
    { "Corleth","of Corelay" },
    { "Toraneth","the Cruel" },
    { "Dargrith","the Butcher" },
    { "Ogrin","Woodenblade" },
    { "Arag","Drythroat" },
    { "Justrik","the Hawk" },
    { "Sharila","Suresword" },
    { "Galagrim","of the Flame" },
    { "Jaranor","the Hasty" },
    { "Anderlane","of the Arakai" },
    { "Urgoreth","the Despiser" },
    { "Mirgrath","the Black" },
    { "Mograk","Rustaxe" },
    { "Taroleth","the Jester" },
    { "Arfold","Longtooth" },
    { "Dorgrun","Roughhand" },
    { "Molicor","the Kind" },
    { "Igral","Mouseheart" },
    { "Orimund","the Resplendent" },
    { "Crun","the Weasel" },
    { "Faramoth","the Solemn" },
    { "Ulgrim","the Weary" },
    { "Miranar","Fairhand" },
    { "Ulgrud","the Treacherous" },
    { "Elgrena","the Gracious" },
    { "Alagrim","Ironaxe" },
    { "Karahar","the Wistful" },
    { "Oragrane","the Fearless" },
    { "Marathor","the Splendid" },
    { "Barag","the Fierce" },
    { "Golgud","the Reluctant" },
    { "Ilareth","the Chosen" },
    { "Torbrith","Swiftfoot" },
    { "Slorum","the Smug" },
    { "Samara","Wildheart" },
    { "Hilgor","the Meek" },
    { "Zalnor","Sourspleen" },
    { "Melgran","Dragonsword" },
    { "Khalak","the Blue" },
    { "Boragrim","Sharpaxe" },
    { "Melkrith","Nightshade" },
    { "Oscruth","Loosehead" },
    { "Volgor","the Sure" },
    { "Talmar","the Quiet" },
    { "Olog","the Friendly" },
    { "Olagrum","the Steady" },
    { "Harumbar","the Unhappy" },
    { "Thorgran","Wildsword" },
    { "Aluthrim","the Bold" },
    { "Alargrith","Warhelm" },
    { "Gorolan","the Benign" },
    { "Ilvar","the Penitent" },
    { "Araleth","the White" },
    { "Arithel","the Joybringer" },
    { "Emedrel","of the Fire" },
    { "Andremar","the Starborn" },
    { "Galdreth","the Fair" },
    { "Sagrana","Goldenwing" },
    { "Elina","the Enchantress" },
    { "Tarella","the Intrepid" },
    { "Oraina","the Placid" },
    { "Karelda","the Carefree" },
    { "Imirel","Starblade" },
    { "Ylanda","the Wishful" },
    { "Morgrissa","Hammertongue" },
    { "Trantana","the Grand" },
    { "Oglissa","the Rose" },
    { "Melinissa","the Sweet" },
    { "Olthruda","the Bountiful" },
    { "Thalgrima","the Betrothed" },
    { "Elessa","of the Mists" },
    { "Ulene","Quietheart" },
    { "Sherinar","of Shadows" },
    { "Mirithel","the Warm" },
    { "Kiranda","the Wild" },
    { "Galahar","the Calm" },
};

int character_mapping[160];



LPCSTR text_races[] = {
      "Kith"
    , "Atheling"
    , "Eldrin"
    , "Long Dwarf"
    , "Arakai"
    , "Dragonlord"
    , "High Fey"
    , "Dawn Fey"
    , "Uskarg"
    , "Gelming"
    , "Deeping Dwarf"
    , "Giant"
    , "Dark Fey"
    , "Golden Fey"
    , "Corsair"
    , "Free",
};


LPCSTR text_realms[] = {
      "The Witherlands"
    , "The Lee"
    , "Eldmark"
    , "The Long Mountains"
    , "The Last Northing"
    , "Arungor"
    , "Wierdwood"
    , "Dawnwood"
    , "The Fallows"
    , "The Gelm"
    , "The Deeping"
    , "The Delve"
    , "The Marish"
    , "Glimormir"
    , "The Great Ocean"
    , "Midnight"
    , "Mists of Oblivion"
    , "Borders of Oblivion"
};


LPCSTR text_title[] = {
    "Kith",
    "Atheling",
    "Eldrin",
    "Long Dwarf",
    "Arakai",
    "Dragonlord",
    "High Fey",
    "Dawn Fey",
    "Uskarg",
    "Gelming",
    "Deeping Dwarf",
    "Giant",
    "Dark Fey",
    "Golden Fey",
    "Corsair",
    "Lord of Midnight",
};

LPCSTR text_areas[]= {
    "Imilvir","Eodral","Arelon","Aravik","Caradwin","Falthrang","Elinmor","Tarefyr",
    "Northern","Eastern","Southern","Western","Middle","Ulmor","Skulls","Shining",
    "Silfar","Grey","Eregoth","Weeping","Fire","Galorwain","Tharran","Mermaids",
    "Serpents","Immiel","Rilleon","Maranor","Corelay","Jade","Last","Erifel",
    "Sareon","Eomir","Rilnor","Thumar","Sharvik","Imildral","Erenim","Isilfrey",
    "Thordroth","Galorbard","Corlon","Farwain","Maralan","Beomir","Ceremelth","Carafel",
    "Saradwin","Emerthen","Scaradir","Meranor","Arelban","Arunvere","Erilan","Elinvar",
    "Arelmar","Shimmering","Forgotten","Whispering","Forever","Golden","Finfyr","Geremiel",
    "Samarand","Emergelm","Ildemar","Zaragorn","Gilgrath","Tharn","Sharenor","Ararak",
    "Thunder","Dawn","Merikith","Aradel","Valgrorn","Malgor","Dwardor","Death",
    "Barathor","Burning","Ravens","Iron","Tark","Athruk","Thordrin","Storms",
    "Big","Corth","Skirol","Othrym","Grorn","Gogrun","Finrod","Songs",
    "Crimson","Elorthord","Corithel","Alathor","Elinbrand","Gliwain","Athrudan","Skordroth",
    "Erilmark","Arabar","Theodel","Cerrelm","Roreon","Caramane","Romiel","Orcar",
    "Ravenfrey","Erefar","Elmir","Melibor","Sharmark","Sapphire","Elormane","Dragon",
    "Rildroth","Angelf","Erivik","Sarnoth","Shrygal","Faragor","Ashnar","Cormir",
    "Cerevere","Melthor","Varorn","The Borders of Oblivion","The Mists of Oblivion",
};


// purposes
LPCSTR text_purpose[]= {
      "NO_PURPOSE"
    , "DEFEND_HOMELAND"
    , "DARK_FEY"
    , "RANDOMLY_WANDER"
    , "BE_A_HOSTAGE"
};

// reactions
LPCSTR text_reactions[] = {
      "RETURN_HOME"
    , "STAND_FIRM"
    , "TAKE_BACK_STRONGHOLD"
    , "ATTACK_ENEMY"
    , "HELP_NEIGHBOUR"
    , "RETREAT"
    , "GATHER_STRENGTH"
    , "COUNTER_THREAT"
    , "LEND_SERVICE"
};


LPCSTR text_ls_war_states[] = {
      "LS_AT_PEACE"
    , "LS_TROUBLED"
    , "LS_THREATENED"
    , "LS_MENACED"
    , "LS_IN_DANGER"
    , "LS_IN_PERIL"
    , "LS_UNDER_ATTACK"
    , "LS_SUBJUGATED"
};

LPCSTR text_rs_war_states[] = {
      "RW_AT_PEACE"
    , "RW_THREATENED"
    , "RW_UNDER_ATTACK"
    , "RW_SUBJUGATED"
};

LPCSTR text_rs_states[] = {
      "RS_UNFETTERED"
    , "RS_AT_RANSOM"
    , "RS_BLOOD_FEUD"
    , "RS_INTERREGNUM"
};

LPCSTR text_quests[] = {
      "RECRUIT"
    , "JOIN"
    , "KILL"
    , "RESCUE"
    , "FOLLOW"
    , "GOTO"
    , "GUARD"
    , "SEIZE"
    , "FIND"
    , "TAKE"
    , "DESTROY"
    , "REST"
    , "UNDER"
};

LPCSTR text_quest_type[] = {
    "-1"
    , "qt_goto"
    , "1"
    , "qt_character"
    , "3"
    , "4"
    , "5"
    
};

LPCSTR text_direction[] = {
    "NORTH"
    , "NORTH_EAST"
    , "EAST"
    , "SOUTH_EAST"
    , "SOUTH"
    , "SOUTH_WEST"
    , "WEST"
    , "NORTH_WEST"
    
};

LPCSTR text_object_names[] = {
      "the sword stormblade"
    , "the sword bloodbringer"
    , "the axe widowmaker"
    , "the sword aranath"
    , "the axe persuader"
    , "the axe skullcrusher"
    , "the sword swiftwing"
};


int CalcX( u32 x )
{
    x=((x/65535)/64);
    f32 x1 = (((f32)x)/1024.0)*256;
    return (int)x1;
}
int CalcY( u32 y )
{
    y=1024-((y/65535)/64);
    f32 y1 = (((f32)y)/1024.0)*256;
    return (int)y1;
}


void OpenFile( string id )
{
    auto filename = root_path + string("citadel_") + id + string(".txt");
    
    output = fopen( filename.c_str(), "wt");
    
}

void CloseFile()
{
    fprintf( output, "\n</lom3>");
    
}



void Element ( LPCSTR name, int value, flags_t* flags, int count )
{
    if ( value == 0 ) {
        fprintf ( output, "\t<%s value='%d'/>\n", name, value);
        return;
    }
    
    //fprintf ( output, "\t<%s value='%d'>\n", name, value);
    fprintf ( output, "\t<%s value='%d'>", name, value);
    for ( int ii=0; ii<count; ii++ ) {
        if ( value & flags[ii].value )
            //fprintf ( output, "\t\t+%s\n", flags[ii].name);
            fprintf ( output, "+%s", flags[ii].name);
    }
    //fprintf ( output, "\t</%s>\n", name);
    fprintf ( output, "</%s>\n", name);
}

void Element ( LPCSTR name, int value )
{
    fprintf ( output, "\t<%s>%d</%s>\n", name, value, name );
}

void Element ( LPCSTR name, u32 value )
{
    fprintf ( output, "\t<%s>%u</%s>\n", name, value, name );
}

void Element ( LPCSTR name, LPCSTR value )
{
    fprintf ( output, "\t<%s>%s</%s>\n", name, value, name );
}

void OutputGraphic( graphic_t* graphic )
{
    char name[MAX_PATH];
    
    sprintf( name, "shield_%d", graphic->obj_shield+1 );
    Element("shield", name );
    
    sprintf( name, "head_%d", graphic->obj_port+1 );
    Element("portrait", name );
    
}


void OutputCommon_t ( common_t* mem )
{
    Element("id", mem->obj_num );
    Element("type", data_type[mem->obj_type] );
    
    //Element("prev_link", mem->obj_prev_link );
    //Element("next_link", mem->obj_next_link );
    
    //Element("char_flags", mem->obj_char_flags );
    Element ( "char_flags_txt", mem->obj_char_flags, char_flags, NUMELE(char_flags) );
    
    //Element("flags", mem->obj_flags );
    Element ( "flags_txt", mem->obj_flags, obj_flags, NUMELE(obj_flags) );
    
    Element("x", CalcX(mem->obj_x)  );
    Element("y", CalcY(mem->obj_y)  );
    //Element("z", mem->obj_z );
    
    Element("direction", text_direction[(mem->obj_dir/256)/32]);
    
    //Element("sprite", mem->obj_sprite );
    //Element("cladding", mem->obj_cladding );
    //Element("print", mem->obj_print );
    
    //Element("x_scale", mem->obj_x_scale );
    //Element("y_scale", mem->obj_y_scale );
    
    //Element("x_size", mem->obj_x_size );
    //Element("y_size", mem->obj_y_size );
    //Element("z_size", mem->obj_z_size );
    
    //Element("cam_off", mem->obj_cam_off );
    
    fprintf ( output, "\n" );
}




void OutputMobile_t ( mobile_obj_t* mem )
{
    OutputCommon_t(mem);
    
    OutputGraphic(&mem->graphic);
    
    Element("movetype", mem->obj_movetype );
    Element("movemode", mem->obj_movemode );
    
    Element("speed", mem->obj_speed );
    Element("int_speed", mem->obj_int_speed );
    
    //Element("turnrate", mem->obj_max_turn );
    
    //Element("target_x", CalcX(mem->obj_targ_x) );
    //Element("target_y", CalcY(mem->obj_targ_y) );
    //Element("target_sq_x", mem->obj_tsq_x );
    //Element("target_sq_y", mem->obj_tsq_y );
    
    //Element("guide_type", mem->obj_guide_type );
    //Element("route", mem->obj_route );
    
    //Element("link_type", mem->obj_link_type );
    //Element("link_num", mem->obj_link_num );
    
    //Element("move_control", mem->obj_move_cnt );
    //Element("move_x", mem->obj_move_x );
    //Element("move_y", mem->obj_move_y );
    
    Element("energy", mem->obj_energy/65535 );
    Element("energy_max", mem->obj_max_limit );
    Element("energy_med", mem->obj_mid_limit );
    
    Element("rest", mem->obj_rest );
    Element("rest_count", mem->obj_rest_count );
    
    Element("timeout", mem->obj_timeout );
    
    Element("combat_opponent", mem->obj_opponent );
    Element("combat_str", mem->obj_strength );
    Element("combat_power", mem->obj_power );
    Element("combat_limit", mem->obj_limit );
    
    //Element("wander_state", mem->obj_state );
    //Element("wander_dir", mem->obj_wander_dir/255 );
    
    fprintf ( output, "\n" );
    
}

void OutputParty_t ( party_obj_t* mem )
{
    OutputMobile_t(mem);
    
    Element("quest", text_quests[mem->quest.obj_quest] );
    Element("quest_stage", mem->quest.obj_qstate );
    Element("quest1", mem->quest.obj_qdat1 );
    Element("quest2", mem->quest.obj_qdat2 );
    Element("quest3", mem->quest.obj_qdat3 );
    Element("quest4", mem->quest.obj_qdat4 );
    
    //Element("prev_party", mem->obj_prev_party );
    //Element("next_party", mem->obj_next_party );
}


void OutputBaseArmy_t ( base_army_t* mem )
{
    OutputParty_t(mem);

    Element("shelter_State", mem->shelter.obj_rstate );
    Element("shelter_X", mem->shelter.obj_shelter_x );
    Element("shelter_Y", mem->shelter.obj_shelter_y );
    Element("shelter_Dir", mem->shelter.obj_shelter_dir );
    Element("shelter_Num", mem->shelter.obj_shelter_num );
    Element("shelter_Type", mem->shelter.obj_shelter_typ );
    
    Element("lstblk_x", mem->obj_lstblk_x );
    Element("lstblk_y", mem->obj_lstblk_y );
    
    Element("rank", mem->obj_rank );
    
    Element("army_race", text_races[mem->obj_army_race] );
    Element("army_size", mem->obj_army_size );
    Element("army_lost", mem->obj_army_lost );
    Element("army_kill", mem->obj_army_kill );
    Element("army_flags", mem->obj_army_flag );
    //Element ( "army_flags_txt", mem->obj_army_flag, army_flags, NUMELE(army_flags) );
    
    Element("start_realm", text_realms[mem->obj_start_realm] );
    Element("start_land", text_areas[mem->obj_start_land]);
    
    Element("sleep_time", mem->obj_sleep_time );
    Element("wake_time", mem->obj_wake_time );
    
    Element("weapons", mem->obj_weapons );
    Element("weapon_details", mem->obj_weap2 );
    Element("weapons_link", mem->obj_weap_link );
    
    Element("find_status", mem->obj_find_status );
    Element("quest_type1", mem->obj_qtype1 );
    Element("quest_type2", mem->obj_qtype2 );
    
    Element("time", mem->obj_time );
    Element("wait", mem->obj_wait );
    
   // Element("flags_2", mem->obj_flags2 );
    Element ( "flags_2_txt", mem->obj_flags2, obj_flags2, NUMELE(obj_flags2) );
    
 
}

void OutputArmy_t ( army_t* mem )
{
    OutputBaseArmy_t(mem);
    Element("shelter_y_backup", mem->obj_shelt_y );
}


u8* LoadFile( LPCSTR filename, int& size )
{
    auto path = root_path + string(filename) ;
    return (u8*) FILE_Load( path.c_str(), size);
}

char symbol[MAX_PATH];

LPCSTR GetCharSymbol(int id)
{
    string str = string(text_names[character_mapping[id]][0]);
    transform(str.begin(), str.end(),str.begin(), ::toupper);
    
    sprintf(symbol, "CH_%s", str.c_str()  );
    return symbol;
}



void OutputChars ( void)
{
    OpenFile("character");
    
    
    int size;
    u8* data = LoadFile("charac",size);
    
    u32* fat = (u32*) data ;
    
    int start=0;
    int count = fat[start]/4;
    size = fat[start+1]-fat[start];
    
    
    for ( int ii=start; ii<count-1; ii++ ) {
        character_t* mem = (character_t*) &data[ fat[ii] ];
        character_mapping[ii] = mem->graphic.obj_port ;
    }

    
    //fprintf( output, "%d  (size=%d)\n", count,size);
    for ( int ii=start; ii<count-1; ii++ ) {
        character_t* mem = (character_t*) &data[ fat[ii] ];
        
        fprintf ( output, "<character id='%d' symbol='%s'>\n", mem->obj_num, GetCharSymbol(ii) );
        
        Element("Name", text_names[mem->graphic.obj_port][0] );
        Element("Title", text_names[mem->graphic.obj_port][1] );
        
     
        OutputBaseArmy_t(mem);
        
        //Element("boat_state", mem->obj_boat_state );
        Element("purpose", text_purpose[mem->obj_purpose+1] );
        Element("reaction", text_reactions[mem->obj_reaction] );
        
        //Element("clothing1", mem->obj_clothing[0] );
        //Element("clothing2", mem->obj_clothing[1] );
        //Element("clothing3", mem->obj_clothing[2] );
        
        if ( mem->obj_allegiance != -1 )
            Element("allegiance", text_races[mem->obj_allegiance] );
        else
            Element("allegiance", "" );
        
        Element("land", mem->obj_curr_land );
        Element("realm", mem->obj_curr_realm );
        
        //Element("qualities1", mem->obj_qualities1 );
        Element ( "qualities1_txt", mem->obj_qualities1, char_traits1, NUMELE(char_traits1) );
        
        //Element("qualities2", mem->obj_qualities2 );
        Element ( "qualities2_txt", mem->obj_qualities2, char_traits2, NUMELE(char_traits2) );
        
        
        Element("xi", mem->obj_xi );
        Element("yi", mem->obj_yi );
        
        Element("lives", mem->obj_lives );
        Element("strength", mem->obj_start_strength );
        Element("armies", mem->obj_start_armies );
        
        fprintf ( output, "</character>\n" );
        
        fprintf(output, "\n");
    }
    fprintf( output, "\n");
    
    FILE_Unload(data);
    
    CloseFile();
    
    
}



void OutputObjects ( void)
{
    int size;
    
    OpenFile("objects");
    
    
    u8* data = LoadFile("object",size);
    
    u32* fat = (u32*) data ;
    
    int start=0;
    int count = fat[start]/4;
    size = fat[start+1]-fat[start];
    
    
    //fprintf( output, "%d  (size=%d)\n", count,size);
    for ( int ii=start; ii<count-1; ii++ ) {
        object_t* mem = (object_t*) &data[ fat[ii] ];
        
        fprintf ( output, "<object id='%d'>\n", mem->obj_num );
        
        Element("name", text_object_names[ii] );
        
        
        OutputCommon_t(mem);
        OutputGraphic(&mem->graphic);
        
        Element("owner", mem->obj_weapon_owner );
        Element("weapon_type", text_weapon_type[mem->obj_weapon_type] );
        Element("details", mem->obj_weapon_details );
        
        
        fprintf ( output, "</object>\n" );
        
        fprintf(output, "\n");
    }
    fprintf( output, "\n");
    
    FILE_Unload(data);
    
    CloseFile();
    
}


void OutputBuildings ( void)
{
    
    int size;
    
    OpenFile("buildings");
    
    u8* data = LoadFile("citadel",size);
    u8* file = data ;
    
    data+=4;
    u32* fat = (u32*) data ;
    
    int start=0;
    int count = fat[start]/4;
    size = fat[start+1]-fat[start];
    
    //fprintf( output, "%d  (size=%d)\n", count,size);
    for ( int ii=start; ii<count-1; ii++ ) {
        building_t* mem = (building_t*) &data[ fat[ii] ];
        
        fprintf ( output, "<building id='%d' >\n", mem->obj_num );
        
        OutputCommon_t(mem);
        
        OutputGraphic(&mem->graphic);
 
        Element("land", text_areas[mem->obj_num]);
        
        //fprintf( output, "\t<warriors>\n");
        for (int i=0; i<LOM3_MAX_RACE; i++ ) {
            //fprintf ( output, "\t\t<warrior race='%s'>%d</warrior>\n", ii, text_races[i], mem->obj_race_size[i]);
            fprintf ( output, "\t\t<warrior_%d>%d</warrior_%d>\n", i, mem->obj_race_size[i], i);
        }
        //fprintf( output, "\t<warriors>\n");
        
        Element("owner", text_races[mem->obj_owner]);
        
        Element("land_ruler", GetCharSymbol(mem->obj_land_ruler) );
        Element("land_war_state", text_ls_war_states[mem->obj_land_war_state] );
        Element("land_realm", text_realms[mem->obj_land_realm] );
        Element("land_owner", GetCharSymbol(mem->obj_land_owner ));
        Element("land_enemies", mem->obj_land_enemies );
        //Element("link", mem->obj_cit_link );
        
        fprintf ( output, "</building>\n" );
        
        fprintf(output, "\n");
    }
    fprintf( output, "\n");
    
    FILE_Unload(file);
    
    CloseFile();
}


void OutputBeasts ( void)
{
    
    int size;

    OpenFile("beasts");
    
    u8* data = LoadFile("beast",size);
    
    u32* fat = (u32*) data ;
    
    int start=0;
    int count = fat[start]/4;
    size = fat[start+1]-fat[start];
    
    
    //fprintf( output, "%d  (size=%d)\n", count,size);
    for ( int ii=start; ii<count-1; ii++ ) {
        beast_t* mem = (beast_t*) &data[ fat[ii] ];
        
        fprintf ( output, "<beast id='%d'>\n", mem->obj_num );
        
        OutputMobile_t(mem);
        
        Element("anim1", mem->obj_anim1 );
        Element("anim2", mem->obj_anim2 );
        Element("anim3", mem->obj_anim3 );
        Element("anim4", mem->obj_anim4 );
        
        fprintf ( output, "</beast>\n" );
        
        fprintf(output, "\n");
    }
    fprintf( output, "\n");
    
    FILE_Unload(data);
    
    CloseFile();
    
}

void OutputArmies ( void)
{
    
    int size;
    
    OpenFile("armies");
    
    u8* data = LoadFile("army",size);
    
    u32* fat = (u32*) data ;
    
    int start=0;
    int count = fat[start]/4;
    size = fat[start+1]-fat[start];
    
    
    //fprintf( output, "%d  (size=%d)\n", count,size);
    for ( int ii=start; ii<count-1; ii++ ) {
        army_t* mem = (army_t*) &data[ fat[ii] ];
        
        fprintf ( output, "<army id='%d'>\n", mem->obj_num );
        
        OutputArmy_t(mem);

        fprintf ( output, "</army>\n" );
        
        fprintf(output, "\n");
    }
    fprintf( output, "\n");
    
    FILE_Unload(data);
    
    
    CloseFile();
}

//void OutputBoats ( void)
//{
//    
//    int size;
//    u8* data = LoadFile("boat",size);
//    
//    u32* fat = (u32*) data ;
//    
//    int start=0;
//    int count = fat[start]/4;
//    size = fat[start+1]-fat[start];
//    
//    
//    //fprintf( output, "%d  (size=%d)\n", count,size);
//    for ( int ii=start; ii<count-1; ii++ ) {
//        mobile_obj_t* mem = (mobile_obj_t*) &data[ fat[ii] ];
//        
//        fprintf ( output, "<boat id='%d'>\n", mem->obj_num );
//        
//        OutputMobile_t(mem);
//
//        
//        fprintf ( output, "</boat>\n" );
//        
//        fprintf(output, "\n");
//    }
//    fprintf( output, "\n");
//    
//    FILE_Unload(data);
//    
//}

int main(int argc, const char * argv[]) {

    
    OutputChars();
    
    OutputBeasts();
    
    OutputObjects();
    
    OutputArmies();
    
    OutputBuildings();
    
    //OutputBoats();
    
    return 0;
}







