/*
 * FILE:    ddr_character.cpp
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



#if defined(_DDR_)

#include <memory>
#include "../../baseinc/tme_internal.h"
#include "scenario_ddr.h"
#include "scenario_ddr_internal.h"
#include "ddr_processor_battle.h"

using namespace tme::scenarios ;

namespace tme {
    
    ddr_character::ddr_character()
    {
    }

    ddr_character::~ddr_character()
    {
    }

    void ddr_character::Serialize ( archive& ar )
    {
        mxcharacter::Serialize(ar);

        if ( ar.IsStoring() ) {
            ar << lastlocation ;
            ar << home_stronghold;
            ar << desired_object;
            ar << fighting_against;
            ar << battlelost;
        }else{
            if ( tme::mx->SaveGameVersion()> 10 && tme::mx->isSavedGame() )
                ar >> lastlocation;
            else
                lastlocation = Location();
            
            ar >> home_stronghold;
            ar >> desired_object;
            
            mxcharacter* temp=NULL;
            
            if ( mx->isSavedGame() )
                ar >> temp;
            
            fighting_against = static_cast<ddr_character*>(temp);
       
            if ( mx->isSavedGame() )
                ar >> battlelost ;
            
        }

    }
    
    MXRESULT ddr_character::FillExportData ( info_t* data )
    {
        using export_t = tme::scenarios::ddr::exports::character_t;
        
        if ( mxcharacter::FillExportData(data) == MX_OK ) {
            
            if ( (u32)ID_TYPE(data->id) != (u32)INFO_ARMY ) {
                
                auto out = static_cast<export_t*>(data);
                
                VALIDATE_INFO_BLOCK(out, INFO_CHARACTER, export_t);

                out->lastlocation = lastlocation ;
                out->orders = orders ;
                out->lastlocation = lastlocation;
                out->targetlocation=targetLocation;
                if ( home_stronghold )
                    out->homelocation = home_stronghold->Location();
            }
            return MX_OK;
        }
        return MX_FAILED;
    }
    
    
    mxlocinfo* ddr_character::GetLocInfo()
    {
        mxlocinfo* info = mxcharacter::GetLocInfo();
    
        // check if we can give object
        if ( !IsInBattle() ) {
            if ( IsCarryingObject() && Carrying()->CanDrop() && traits.Is(ct_generous) ) {
                for (u32 ii=0; ii<info->objCharacters.Count(); ii++) {
                    mxcharacter* ch = static_cast<mxcharacter*>(info->objCharacters[ii]);
                    if ( ch!=this && !ch->IsCarryingObject()) {
                        info->someone_to_give_to = ch ;
                        info->flags.Set(lif_give);
                        break;
                    }
                }
            }
        }
        
        // can we use object
        if ( !HasUsedObject() ) {
            if ( IsCarryingDesiredObject() )
                info->flags.Set(lif_use);
        }
        
        // we shouldn't leave a location that has enemies in
        if( IsAIControlled() ) {
            if( info->foe.armies || info->foe.characters ) {
                info->flags.Reset(lif_moveforward);
            }
        }
        
        if ( IsNight() ) {
            info->flags.Reset( lif_enter_tunnel
                              |lif_enterbattle
                              |lif_fight
                              |lif_guardmen
                              |lif_recruitchar
                              |lif_recruitmen
                              |lif_seek
                              |lif_rest
                              |lif_take
                              |lif_use
                              |lif_give
                              |lif_hide
                              );
        }
        
        
        return info;
    }
    
    mxobject* ddr_character::Cmd_Fight()
    {
        SetLastCommand ( CMD_FIGHT, IDT_NONE );
        
        std::unique_ptr<mxlocinfo> info ( GetLocInfo() );
        
        auto fightobject = static_cast<ddr_object*>(mx->ObjectById(info->fightthing)) ;
        
        // is there anything to fight
        if ( fightobject == nullptr )
            return nullptr ;
        
        SetLastCommand ( CMD_FIGHT, mxentity::SafeIdt(fightobject)) ;

        if ( ! sv_cheat_always_win_fight )
        {
            u32 r = mxrandom(255) & 15 ;
            s32 loses = (r/2)*5;
            
            // soldiers -= loses ;
            warriors.Loses(loses);
            riders.Loses(loses);
            
            static u8 critter_success[] = { 0, 9, 8, 10, 11 };
            
            if ( r > critter_success[fightobject->Id()]  ) {
                static_cast<ddr_battle*>(mx->battle)->loseFight(this,r);
                if ( IsDead() ) {
                    killedbyobject = fightobject;
                    return fightobject;
                }
            }
        }
        
        // we have killed the enemy
        //
        mx->text->oinfo = fightobject;
        mx->SetLastActionMsg( mx->text->CookedSystemString( SS_FIGHT, this) );
        
        CommandTakesTime(true);
        
        // this does an auto write to the map!
        mx->gamemap->GetAt( Location() ).RemoveObject();
        
        return fightobject ;
        
    }
    
    void ddr_character::LostFight ( void )
    {
        // stubber out
    }


    void ddr_character::DecreaseEnergy ( s32 amount )
    {
    mxrace*        rinfo = mx->RaceById(Race());
    int                EnergyCost = 3;

        if ( (Traits().Is(ct_strong)) )
            EnergyCost = 2 ;
        else if ( Traits().Is(ct_weak) )
            EnergyCost = 4 ;

        if ( IsRiding() )
            EnergyCost += rinfo->BaseEnergyCostHorse();
        else
            EnergyCost += rinfo->BaseEnergyCost();
        
        // CHEAT
        // ** CHEATS
        if ( sv_cheat_movement_cheap )
            EnergyCost=1;
        if ( sv_cheat_movement_free )
            EnergyCost=0;
        // ** END CHEATS
        
        
        energy = BSub(energy,EnergyCost,0);
        riders.Energy(energy);
        warriors.Energy(energy);
    }

   void ddr_character::IncreaseEnergy ( s32 amount )
    {
        mxrace* rinfo = mx->RaceById(Race());
        
        s32 baserest = rinfo->BaseRestAmount() ;
        
        energy =
        BAdd ( energy, amount + baserest, (u32)sv_character_max_energy );
        
        riders.Energy(energy);
        warriors.Energy(energy);
        
    }
    
    mxunit_t ddr_character::getArmyType() const
    {
        if ( flags.Is(cf_allowedriders))
            return UT_RIDERS;
        else if ( flags.Is(cf_allowedwarriors))
            return UT_WARRIORS;
        return UT_NONE;
    }
    
    mxunit* ddr_character::getUnit()
    {
        static mxunit blank;
        switch ( getArmyType() ) {
            case UT_RIDERS:
                return &riders;
            case UT_WARRIORS:
                return &warriors;
            default:
                return &blank;
        };
    }
    
    s32 ddr_character::getArmySize()
    {
        return getUnit()->Total();
    }
    
    void ddr_character::setArmySize(s32 value)
    {
        getUnit()->Total(value);
    }
    
    void ddr_character::setArmyLost(s32 value)
    {
        mxunit* unit = getUnit();
        unit->Lost( unit->Lost()+value );
    }
    
    void ddr_character::setArmyKilled(s32 value)
    {
        mxunit* unit = getUnit();
        unit->Killed( unit->Killed()+value );
    }
    
    void ddr_character::IncreaseDespondency( s32 amount )
    {
        despondency = BAdd(despondency, amount, MAX_DESPONDENCY);
    }
    
    void ddr_character::DecreaseDespondency( s32 amount )
    {
        despondency = BSub(despondency, amount, 0);
    }
    
    static mxterrain_t GetRacePeferredTerrainOriginal( mxrace_t race, bool riding )
    {
        // are we on our preferred terrain 0x7354
        // This replicates the original spectrum BUG
        // L7344 DB 001h,008h,004h,003h,002h,002h,002h,002h
        // DB 002h,002h,002h,002h,002h,002h,002h,002h
        if(!riding) {
            if( race==RA_MOONPRINCE )
                return TN_FORTRESS;
            if( race==RA_WISE )
                return TN_HILLS;
        }else{
            if( race==RA_MOONPRINCE )
                return TN_MOUNTAIN2;
            if( race==RA_WISE )
                return TN_GATE;
        }
        return TN_FOREST2;
    }

    static mxterrain_t GetRacePeferredTerrainSpectrum( mxrace_t race )
    {
//    L7354  DB TERRAIN_CITY,   TERRAIN_CITY        ;moonprince
//           DB TERRAIN_PLAINS, TERRAIN_PLAINS      ;free
//           DB TERRAIN_TOWER,  TERRAIN_TOWER       ;wise
//           DB TERRAIN_FOREST, TERRAIN_FOREST      ;fey
//           DB TERRAIN_HILLS,  TERRAIN_HILLS       ;barbarian
//           DB TERRAIN_PLAINS, TERRAIN_PLAINS      ;icelord
//           DB TERRAIN_FOREST                      ;tarithel
//           DB TERRAIN_MOUNTAINS                   ;giant
//           DB TERRAIN_PLAINS                      ;heartstealer
//           DB TERRAIN_FROZENWASTE                 ;dwarf

        switch( race ) {
            case RA_MOONPRINCE:
                return TN_CITY;
            case RA_FREE:
                return TN_PLAINS2;
            case RA_WISE:
                return TN_WATCHTOWER;
            case RA_FEY:
            case RA_TARITHEL:
                return TN_FOREST2;
            case RA_BARBARIAN:
                return TN_HILLS;
            case RA_ICELORD:
                return TN_PLAINS2;
            case RA_GIANT:
                return TN_MOUNTAIN2;
            case RA_HEARTSTEALER:
                return TN_PLAINS;
            case RA_DWARF:
                return TN_ICYWASTE; // tunnels
            default:
                return TN_PLAINS;
// NOT DDR
//            case RA_DOOMGUARD:
//            case RA_TARG:
//            case RA_MORKIN:
//            case RA_SKULKRIN:
//            case RA_DRAGON:
//            case RA_TROLL:
//            case RA_DOOMDARK:
//            case RA_MIDWINTER:
        }
    }
    
    static mxterrain_t GetRacePeferredTerrainC64( mxrace_t race )
    {
//    L7354A    DB TERRAIN_MOUNTAINS, TERRAIN_MOUNTAINS ;moonprince
//              DB TERRAIN_PLAINS,    TERRAIN_PLAINS    ;free
//              DB TERRAIN_FOREST,    TERRAIN_FOREST    ;wise
//              DB TERRAIN_FOREST,    TERRAIN_FOREST    ;fey
//              DB TERRAIN_FOREST,    TERRAIN_FOREST    ;barbarian
//              DB TERRAIN_PLAINS,    TERRAIN_PLAINS    ;icelord
//              DB TERRAIN_FOREST                       ;tarithel
//              DB TERRAIN_MOUNTAINS                    ;giant
//              DB TERRAIN_PLAINS                       ;heartstealer
//              DB TERRAIN_FROZENWASTE                  ;dwarf

        switch( race ) {
            case RA_MOONPRINCE:
                return TN_MOUNTAIN2;
            case RA_FREE:
            case RA_ICELORD:
            case RA_HEARTSTEALER:
                return TN_PLAINS2;
            case RA_WISE:
            case RA_FEY:
            case RA_TARITHEL:
            case RA_BARBARIAN:
                return TN_FOREST2;
            case RA_GIANT:
                return TN_MOUNTAIN2;
            case RA_DWARF:
                return TN_ICYWASTE; // tunnels
            default:
                return TN_PLAINS2;
// NOT DDR
//            case RA_DOOMGUARD:
//            case RA_TARG:
//            case RA_MORKIN:
//            case RA_SKULKRIN:
//            case RA_DRAGON:
//            case RA_TROLL:
//            case RA_DOOMDARK:
//            case RA_MIDWINTER:
        }
    }

    static mxterrain_t GetRacePeferredTerrain( mxrace_t race, bool riding )
    {
        if(mx->isRuleEnabled(RF_DDR_MOVEMENT_SPECTRUM)) {
            return GetRacePeferredTerrainSpectrum(race);
        }
        else if(mx->isRuleEnabled(RF_DDR_MOVEMENT_C64)) {
            return GetRacePeferredTerrainC64(race);
        }
        return GetRacePeferredTerrainOriginal(race,riding);
    }


    MXRESULT ddr_character::Cmd_WalkForward ( bool perform_seek )
    {
    int             TimeCost;
    mxrace*         rinfo;
    mxterrain*      tinfo;
    mxterrain_t     t;
    bool            fastTunnels = false;

        // NOTE: A time amount of 1 = 15 minutes
        //
        rinfo = mx->RaceById(Race());

        // remove army from location
        mx->gamemap->SetLocationArmy(Location(),0);
        mx->gamemap->SetLocationCharacter(Location(),0);
        
        t = (mxterrain_t)mx->gamemap->GetAt ( location+looking ).terrain ;
        t = mx->scenario->toScenarioTerrain(t);
        
        if ( IsInTunnel() ) {
            if(mx->isRuleEnabled(RF_FAST_TUNNELS)) {
                fastTunnels = true;
            }
            t=TN_ICYWASTE;
        }
        
        // start with intial terrain
        tinfo = mx->TerrainById( t );
        TimeCost = tinfo->MovementCost();
        
        // adjust for race/terrain
        auto preferredTerrain = GetRacePeferredTerrain(Race(),IsRiding()) ;
        if( t == preferredTerrain ) {
            TimeCost = 0;
        }
        
        // adjust for race base
        // are we on horseback?
        int raceAdjustment = !IsRiding()
            ? rinfo->InitialMovementValue()
            : rinfo->RidingMovementMultiplier();

        if (fastTunnels) {
            if(Race()==RA_DWARF || Race() == RA_GIANT) {
                raceAdjustment = 2;
            }
        }
        
        TimeCost += raceAdjustment;
        
        // are we moving diagonally?
        if ( Looking()&1 )
            TimeCost += rinfo->DiagonalMovementModifier();

        // check mist
        // this looks like a bug to me, why be affected by mist when in a tunnel?
        //
        if (!fastTunnels) {
            if ( mx->gamemap->GetAt ( location+looking ).IsMisty() ) {
                if ( rinfo->MistTimeAffect() ) {
                    DecreaseEnergy(0);
                    TimeCost += rinfo->MistTimeAffect();
                    DecreaseDespondency( rinfo->MistDespondecyAffect() ) ;
                }
            }
        }

        // check monster
        mxthing_t thing = mx->gamemap->getLocationObject(this,location+looking);
        if ( thing >= OB_WOLVES && thing <= OB_SKULKRIN ) {
            DecreaseEnergy(0);
            TimeCost += 2;
        }

        // check not swift
        if ( !(Traits().Is(ct_swift)) )
            TimeCost += 1;

        // check slow
        if ( Traits().Is(ct_slow) )
            TimeCost += 1;

        if(fastTunnels) {
            if(Race()==RA_DWARF || Race() == RA_GIANT) {
                TimeCost /= 3;
            }else{
                TimeCost /= 2;
            }
        }

        // mountains sap more energy for everyone except giants
        if ( t == TN_MOUNTAIN || t == TN_MOUNTAIN2 ) {
            DecreaseEnergy(0);
            if ( Race() != RA_GIANT ) 
                DecreaseEnergy(0);
        }

        DecreaseEnergy(0);
        
        if ( !IsAIControlled() ) {
            // CHEAT
            // ** CHEATS
            if ( sv_cheat_movement_cheap )
                TimeCost=1;
            if ( sv_cheat_movement_free )
                TimeCost=0;
            // ** END CHEATS
        }
        
        // time
        time = (mxtime_t) chilli::lib::BSub(time,TimeCost,0);

        // DDR moves after the calc
        // and move us there
        location = location + Looking();

        // add armies and characters back to the map
        // this is overkill!!!!
        mx->scenario->SetMapArmies();

        if ( !IsAIControlled() )
            EnterLocation ( location );

        // if this location has an exit then we must exit
        bool exit_tunnel = false;
        if ( mx->gamemap->GetAt ( location ).HasTunnelExit() )
            exit_tunnel=true;

        if ( !IsAIControlled() )
            mx->scenario->LookInDirection ( Location(), Looking(), IsInTunnel() );
        
        if ( !IsAIControlled() )
            mx->scenario->MakeMapAreaVisible(Location(), this);
        
        CommandTakesTime(true);

        // if we have moved, we are no longer in battle
        flags.Reset(cf_inbattle|cf_preparesbattle);
        
        // 
        if ( !IsAIControlled() && perform_seek ) {
            // quick fix
            if ( exit_tunnel ) flags.Reset ( cf_tunnel );
            if ( mx->gamemap->getLocationObject(this, Location())!=OB_NONE ) {
                Cmd_Seek();
            }
            if ( exit_tunnel ) flags.Set ( cf_tunnel );
        }
        
        WalkFollowersForward();
        
        // if this location has an exit then we must exit
        if ( exit_tunnel )
            Cmd_ExitTunnel();

        return MX_OK ;
    }
    
    MXRESULT ddr_character::Cmd_EnterTunnel ( void )
    {
        if ( IsFollowing() )
            return MX_FAILED ;
        
        if ( IsInTunnel() )
            return MX_FAILED ;
        
        mxloc& mapsqr = mx->gamemap->GetAt ( Location() );
        if ( !mapsqr.HasTunnelEntrance() )
            return MX_FAILED ;
        
        // remove army from location
        mx->gamemap->SetLocationArmy(Location(),0);
        mx->gamemap->SetLocationCharacter(Location(),0);
        
        flags.Set ( cf_tunnel );
        EnterLocation(Location());
        
        
        
        // TODO:
        // we now need to cycle round the locations
        // looking for the location that connections to us
        // and drop us in there
        for ( int ii=DR_NORTH; ii<=DR_NORTHWEST; ii+=2 ) {
            mxdir_t d = (mxdir_t) ii ;
            loc_t l = Location() + d ;
            if ( mx->gamemap->GetAt ( l ).HasTunnel() ) {
                location = l ;
                looking=d;
                break;
            }
            
        }
        
        // new location
        EnterLocation(Location());
        
        // group enter tunnel
        
        if ( HasFollowers() ) {
            entities followers;
            mx->scenario->GetCharacterFollowers(this, followers);
            for ( u32 ii=0; ii<followers.Count(); ii++ ) {
                mxcharacter* follower = (mxcharacter*) followers[ii];
                follower->Flags().Set(cf_tunnel);
                follower->looking = Looking();
                follower->Location(Location());
            }
        }
        
        mx->scenario->SetMapArmies();
        
        CommandTakesTime(TRUE);
        
        return MX_OK ;
    }
    
    MXRESULT ddr_character::Cmd_ExitTunnel ( void )
    {
        if ( IsFollowing() )
            return MX_FAILED ;
        
        if ( !IsInTunnel() )
            return MX_FAILED ;
        
        mxloc& mapsqr = mx->gamemap->GetAt ( Location() );
        
        if ( !mapsqr.HasTunnelExit() )
            return MX_FAILED ;
        
        flags.Reset ( cf_tunnel );
        
        // group exit tunnel
        
        if ( HasFollowers() ) {
            entities followers;
            mx->scenario->GetCharacterFollowers(this, followers);
            for ( u32 ii=0; ii<followers.Count(); ii++ ) {
                mxcharacter* follower = (mxcharacter*) followers[ii];
                follower->Flags().Reset(cf_tunnel);
                follower->looking = Looking();
                follower->Location( Location() );
            }
        }
        
        CommandTakesTime(TRUE);
        
        return MX_OK ;
    }
    
    MXRESULT ddr_character::Cmd_Take ( void )
    {
        return Cmd_PickupObject()==NULL ? MX_FAILED : MX_OK ;
    }
    
    MXRESULT ddr_character::Cmd_Give ( mxcharacter* character )
    {
        // no character
        if ( character == nullptr )
            return MX_FAILED ;
        
        // character is already carrying something
        if ( character->IsCarryingObject() )
            return MX_FAILED ;
        
        character->carrying = Carrying();
        
        carrying = nullptr ;
        
        CommandTakesTime(true);
        
        return MX_OK ;
    }

    mxcharacter* ddr_character::GetNextFoe() const
    {
        mxcharacter* character = Foe();
        while ( character!=NULL && character->IsDead() ) {
            if (character->Liege() == character)  {
                //
                MXTRACE("GetNextFoe: Error - Circular Liege: %s - breaking out", character->Longname().c_str());
                return NULL;
            }
            character = character->Liege();
        }
        return character;
    }
    mxcharacter* ddr_character::GetNextLiege() const
    {
        mxcharacter* character = Liege();
        while ( character!=NULL && character->IsDead() ) {
            if (character->Liege() == character)  {
                //
                MXTRACE("GetNextLiege: Error - Circular Liege: %s - breaking out", character->Longname().c_str());
                return NULL;
            }
            character = character->Liege();
        }
        return character;
    }
    
    bool ddr_character::CheckRecruitChar ( mxcharacter* character )  const
    {
        if ( character == this )
            return false ;

        if ( character->HasFollowers() )
            return false;
        
        if ( character->IsFollowing() )
            return false;
        
        //if (!IsAIControlled())
        //    return true;
        
        // Only Tarithel can recruit Morkin
        if ( character->IsSymbol("CH_MORKIN") )
            return IsSymbol("CH_TARITHEL");
        
        else if ( character->IsSymbol("CH_TARITHEL"))
            return false;
        else if ( character->IsSymbol("CH_RORTHRON"))
            return false;
        else if ( character->IsSymbol("CH_LUXOR"))
            return false;
        else if ( character->IsSymbol("CH_SHARETH") )
            return false ;

        u8 us_attribute_good = traits & 0xff ;
        u8 us_attribute_bad =  (traits >> 8 ) & 0xff;

        u8 them_attribute_good = character->traits & 0xff ;
        u8 them_attribute_bad =  (character->traits >> 8 ) & 0xff;
        
        
        int approachKey = ( (them_attribute_bad & us_attribute_bad) |
                           (them_attribute_good & us_attribute_good) ) & 0xbf;  // 1011 1111
        int i;
        int success = 0;
        
        // loop over the 8 bits of approach key, increase chance each time there is a match
        for (i = 0; i < 8; i++) {
            if (approachKey & 1)
                success++;
            approachKey >>= 1;
        }
        
        if (!character->Traits().Is(ct_loyal))
            success++;
        
        if (character->Traits().Is(ct_treacherous) )
            success *= 2;
        
        if (character->Liege() == this)
            success+=3;
        
        auto object = static_cast<ddr_object*>(Carrying());
        if ( object != nullptr && object->CanHelpRecruitment() )
            success+=2;
        
        if (success < 4) // failed
            return false;
        // succeeded
        return true;

    }

    bool ddr_character::Recruited ( mxcharacter* recruiter )
    {
        // morkin in DDR is special
        if ( IsSymbol("CH_MORKIN") ) {
            return RecruitMorkin(recruiter);
        }
      
        if ( !recruiter->IsAIControlled() ) {
            // set loyalty?
            flags.Set ( cf_recruited );
            flags.Reset(cf_ai);
        }else{
            flags.Reset(cf_recruited);
            flags.Set(cf_ai);
        }

        // 1. set time of day to recruiting character
        SetRecruitmentTime(recruiter);

        // 2. our liege becomes the recruiting character
        if ( recruiter == this ) {
            MXTRACE("Recruited: Error Setting Liege to self! %s", Longname().c_str());
        }
        liege = recruiter; //->Liege() ;
        
        // 3. our loyalty race becomes recruiting character loyalty race
        loyalty = recruiter->NormalisedLoyalty() ;
        
        // 4. get the my foe's loyalty, if they are loyal to the recruiting char's loyalty
        // then we need the recruiting characters foe
        //
        if ( foe->NormalisedLoyalty() == loyalty ) {
            foe = recruiter->Foe() ;
        }

        return true ;
    }
        
    MXRESULT ddr_character::EnterBattle ()
    {
        Flags().Set(cf_preparesbattle); // force the battle flag early!
        
        time = sv_time_night;
        
        if ( HasFollowers() ) {
            // we need to do a merge
            
            entities followers;
            mx->scenario->GetCharacterFollowers(this, followers);
            
            for ( u32 ii=0; ii<followers.Count(); ii++ ) {
                mxcharacter* follower = (mxcharacter*) followers[ii];
                follower->EnterBattle();
            }
        }
        
        return MX_OK;
    }

    bool ddr_character::RecruitMorkin ( mxcharacter* recruiter )
    {
        // set loyalty?
        flags.Set ( cf_recruited );
        flags.Reset( cf_ai );
        // 1. set time of day to recruiting character
        SetRecruitmentTime(recruiter);
        // 2. our liege becomes the recruiting character
        liege = recruiter->Liege() ;
        // 3. our loyalty race becomes recruiting character loyalty race
        loyalty = recruiter->NormalisedLoyalty() ;
        // 4. our foe becomes recruiting character's foe
        foe = recruiter->Foe();
        // 5. traits
        traits.Reset(ct_evil|ct_weak|ct_reticent|ct_greedy|ct_fawning|ct_coward|ct_slow|ct_treacherous);
        traits.Set(ct_good|ct_strong|ct_forceful|ct_generous|ct_stubborn|ct_brave|ct_swift|ct_loyal);
        return true;
    }
    
    void ddr_character::CheckKilledFoe ( void )
    {
        if ( Flags().Is(cf_killed_foe) )
            IncreaseDespondency(DESPONDENCY_BOOST_KILLED_FOE);
    }
    
    //Object 0-4 special objects
    //All others 255 to Tired and Despondency
    //0 - OB_CROWN_VARENAND  - Bring all luxor loyal lords to this location ( in/out tunnel )
    //1 - OB_CROWN_CARUDRIUM - Bring all luxor loyal lords to this location ( in/out tunnel )
    //2 - OB_SPELL_THIGRORN  - Spell of swiftness - Bring morkin to current location
    //3 - OB_RUNES_FINORN  - Runes of Protection - 255 to Tired and Despondency of all loyal characters
    //4 - OB_CROWN_IMIRIEL - Bring all luxor loyal lords to this location ( in/out tunnel )
    
    
    void ddr_character::UseCrownOfPersuassion()
    {
        for ( int ii=0; ii<sv_characters; ii++ ) {
            auto c = mx->CharacterById(ii+1);
            if ( c->IsAlive() && !c->IsAIControlled() ) {
                c->Location(Location());
                c->Flags().Reset( cf_tunnel );
                if ( IsInTunnel() )
                    c->Flags().Set( cf_tunnel );
            }
        }
        // luxor the moonprince places the crown of carudrium on his head,
        // the power contained within brings all lords loyal to him.
        mx->SetLastActionMsg( mx->text->CookedSystemString( SS_OBJECT_USE_1, this) );
    }
    
    void ddr_character::UseSpellOfSwiftness()
    {
        // take tarithel to morkin
        // set time of day
        //
        auto morkin = static_cast<mxcharacter*>(mx->EntityByName("CH_MORKIN"));
        
        Location(morkin->Location());
        Flags().Reset( cf_tunnel );
        
        if ( morkin->IsInTunnel() )
            Flags().Set( cf_tunnel );
        
        // Tarithel the fey casts the Spell of Thigor, Morkin is transported to be with her.
        mx->SetLastActionMsg( mx->text->CookedSystemString( SS_OBJECT_USE_2, this) );
    }
    
    void ddr_character::UseRunesOfProtection()
    {
        for ( int ii=0; ii<sv_characters; ii++ ) {
            auto c = mx->CharacterById(ii+1);
            if ( c->IsAlive() && !c->IsAIControlled() ) {
                c->despondency=MAX_DESPONDENCY;
                c->energy=MAX_ENERGY;
            }
        }
        // rorthron throws the runes of finorn, all lords loyal to the moon prince are refreshed and envigorated
        mx->SetLastActionMsg( mx->text->CookedSystemString( SS_OBJECT_USE_3, this) );
    }
    
    void ddr_character::UseAllOtherObjects()
    {
        despondency=MAX_DESPONDENCY;
        energy=MAX_ENERGY;
        // rorthron uses the %s, he now feels refreshed and envigorated.
        mx->SetLastActionMsg( mx->text->CookedSystemString( SS_OBJECT_USE_4, this) );
    }

    MXRESULT ddr_character::Cmd_Use ( void )
    {
        if ( Flags().Is(cf_usedobject) )
            return MX_FAILED;
        
        if ( !IsCarryingDesiredObject() )
            return MX_FAILED;
        
        //
        mx->text->oinfo = Carrying() ;

        // check power!
        // check type
        switch (Carrying()->Id()) {
            
            case OB_CROWN_VARENAND:     // 0 - Luxor
            case OB_CROWN_CARUDRIUM:    // 1 - Morkin
            case OB_CROWN_IMIRIEL:      // 4 - Shareth
                // shareth - Note: Only shareth can use Imiriel, but AI lords cannot use their objects
                // so this is a little redundant for that object.
                UseCrownOfPersuassion();
                break;
                
            case OB_SPELL_THIGRORN:     // 2 - Tarithel
                UseSpellOfSwiftness();
                break;
                
            case OB_RUNES_FINORN:       // 3 - rorthron
                UseRunesOfProtection();
                break;
                
            default:
                UseAllOtherObjects();
                break;
        }
        
        Flags().Set(cf_usedobject);
        CommandTakesTime(true);
        return MX_OK ;
    }
    
    void ddr_character::AICheckRecruitSoldiers ( void )
    {
        if ( !IsAIControlled() )
            return;

        // get stronghold at the location I'm at
        auto scenario = static_cast<ddr_x*>(mx->scenario);
        auto stronghold = static_cast<ddr_stronghold*>(scenario->StrongholdFromLocation(Location()));
        
        if ( stronghold == nullptr )
            return;
        
        if ( !stronghold->IsFriend(this) )
            return;
        
        if ( stronghold->OccupyingRace() != Race() )
            return;
        
        mxunit_t armytype = IsAllowedWarriors() ? UT_WARRIORS : UT_RIDERS ;
        u32 total = armytype==UT_WARRIORS ? warriors.Total() : riders.Total() ;
        
        // 20 soldiers at a time
        int stronghold_total = stronghold->TotalTroops() ;
        
        int diff = stronghold_total - total ;
        if ( diff <= 0 )
            return;
        
        diff -= SOLDIERS_RECRUIT_GAP ;
        if ( diff <= 0 )
            return;
        
        total += diff ;
        
        if ( armytype == UT_WARRIORS )
            warriors.Total(total);
        else
            riders.Total(total);
        
        stronghold_total -= diff ;
        stronghold->TotalTroops(std::max<int>(stronghold_total,0));
        
        //
        
    }
    

void ddr_character::Target ( const mxitem* newtarget )
{
    if ( newtarget == NULL ) {
        //MXTRACE("NULL Target from %s", Longname());
        newtarget=this;
    }
    targetId = mxentity::SafeIdt(newtarget);
    targetLocation = newtarget->Location();
}

mxorders_t ddr_character::pickNewOrders () const
{
    static mxorders_t order_map[] = { OD_FOLLOW_LIEGE, OD_FOLLOW_FOE, OD_FIND_OBJECT, OD_HOME, OD_NONE };
    
    int temp = mxrandom(15);

    if ( temp <= 4 )
        return order_map[temp];
    
    return orders;
        
}

void ddr_character::whatIsCharacterDoing ( void )
{
    mxorders_t      new_orders = OD_NONE ;
    mxcharacter*    follow_character=NULL;

#ifdef MX_DEBUG_ON
    LPCSTR          commanded="";
#endif
    
    if ( Liege() == NULL ) {
        new_orders = pickNewOrders();
    }else{
        mxorders_t liege_orders = static_cast<ddr_character*>(Liege())->orders ;
        if ( liege_orders == OD_FOLLOW_LIEGE  || liege_orders == OD_FOLLOW_FOE ) {
            new_orders=OD_FOLLOW_LIEGE;
#ifdef MX_DEBUG_ON
            commanded=" by command ";
#endif
        } else {
            new_orders=pickNewOrders();
        }
    
    }
    
    // stay here
    if ( new_orders == OD_NONE ) {
        MXTRACE("%s is doing nothing.", Longname().c_str());
        Target(this);
        return;
    }
    
    
    orders = new_orders ;
    
    mxcharacter* ch_luxor = static_cast<mxcharacter*>(mx->EntityByName("CH_LUXOR"));
    
    switch ( orders )
    {
        case OD_FOLLOW_LIEGE:
            follow_character=Liege();
            if ( follow_character == NULL ) {
                follow_character = ch_luxor;
            } else {
                if ( follow_character->IsDead() ) {
                    follow_character = follow_character->Liege(); //GetNextLiege();
                    
                    //if ( follow_character == this ) {
                    //    MXTRACE("whatIsCharacterDoing: Error Setting Liege to self! %s", (LPSTR) Longname());
                    //    follow_character = NULL ;
                    //}

                    
                    if (follow_character==NULL)
                        follow_character=ch_luxor;
                    else {
                        liege = follow_character;
                    }
                }
            }
            
#ifdef MX_DEBUG_ON
            if ( Liege() == follow_character ) {
                MXTRACE("%s is following liege %s%s.", Longname().c_str(), follow_character->Longname().c_str(), commanded);
            }else{
                MXTRACE("%s is following %s%s.", Longname().c_str(), follow_character->Longname().c_str(), commanded);
            }
#endif
            
            Target(follow_character);
            break;
            
        case OD_FOLLOW_FOE:
            follow_character=Foe();
            if ( follow_character == NULL ) {
                follow_character = ch_luxor;
            } else {
                if ( follow_character->IsDead() ) {
                    follow_character = follow_character->Liege(); //GetNextFoe();
                    if (follow_character==NULL)
                        follow_character=ch_luxor;
                    else
                        foe=follow_character;
                }
            }
            MXTRACE("%s is following foe %s.", Longname().c_str(), follow_character->Longname().c_str());
            Target(follow_character);
            break;
        
        case OD_FIND_OBJECT:
            // object
            if ( !desired_object->IsCarried() ) {
                MXTRACE("%s is looking for %s", Longname().c_str(), mx->text->DescribeObject(desired_object).c_str());
                Target(desired_object);
                break;
            }
            
            // fall through when
            // object is carried, so start heading home
            
        case OD_HOME:
            if ( this == mx->EntityByName("CH_SHARETH")) {
                Target( static_cast<mxitem*>(mx->EntityByName("SH_CITY_GLIREON")) );
            }else{
                Target(home_stronghold);
            }
            MXTRACE("%s is heading home.", Longname().c_str());

            break;
            
        default:
            Target(this);
            break;
    }
    
}


const mxdir_t directions[] = {   DR_NORTH
                                ,DR_EAST
                                ,DR_NORTHEAST
                                ,DR_SOUTH
                                ,DR_NORTH
                                ,DR_SOUTHEAST
                                ,DR_NORTH
                                ,DR_WEST
                                ,DR_NORTHWEST
                                ,DR_NORTH
                                ,DR_NORTH
                                ,DR_SOUTHWEST};

mxdir_t calcDirection ( mxgridref loc1, mxgridref loc2 )
{
    s32 direction=0;
    
    if ( loc1.y > loc2.y )
        direction|=1;
    else if ( loc1.y < loc2.y )
        direction|=4;
    
    if ( loc1.x < loc2.x )
        direction|=2;
    else if ( loc1.x > loc2.x )
        direction|=8;
    
    if ( direction == 0 )
        return DR_NONE ;
 
    return directions[ direction-1 ];

}
/*
1. Decide what the lord should do
  If there is no liege then choose new orders
  else if the liege is following his liege or foe then follow liege
  else pick new orders
2. Check if can pickup object
3. Choose Direction
  If can't move forward, maybe because someone is here or blocked
    If way forward is blocked and it's not dawn then stop turn
    else 50% chance to stop turn (8)
  If reached target the stop turn (8)
4. Walk forward
5. If night then stop turn (8)
6. If tired then stop turn (8)
7. repeat from [2]
8. If there is just one enemy at the current location, and me, then approach

[Pick new orders]
    Pick random number between 0 and 15
    if 0 then FOLLOW_LIEGE
    if 1 then FOLLOW_FOE
    if 2 then FOLLOW_OBJECT
    if 3 then HOME
    if 4 then temporarily DO NOTHING
    else do whatever they were doing

[FOLLOW_LIEGE]
    If no liege then follow luxor
    else if liege is alive then follow
    else if liege is dead then get their liege
      if still no liege then  follow luxor
      else become loyal to and follow new liege

[FOLLOW_FOE]
    If no foe then follow luxor
    else if foe is alive then follow
    else if foe is dead then get their foe
      if still no foe then follow luxor
      else become foe and follow new foe
    
[FOLLOW_OBJECT]
    Goto object location if not carried
    
[HOME]
    Goto home location
 */


void ddr_character::moveCharacterSomewhere ( void )
{
    if ( !IsAIControlled() ) {
        MXTRACE("NOT Processing %s - NOT AI", Longname().c_str());
        return;
    }
    
    if ( IsDead() ) {
        MXTRACE("NOT Processing %s - DEAD", Longname().c_str());
        return;
    }
        
    MXTRACE("Processing %s at location [%d,%d]", Longname().c_str(), location.x, location.y);
    MXTRACE("  START");
    
    whatIsCharacterDoing();
    
    MXTRACE("  Target Location is [%d,%d]", targetLocation.x, targetLocation.y);

    bool reachedTarget = false;
    
    for (;;) {
        
        if( !IsCarryingObject() )
            Cmd_PickupObject();
        
        mxdir_t direction = calcDirection(Location(),targetLocation);
    
        if ( direction==DR_NONE ) {
            MXTRACE("  No Direction.");
            break;
        }
        
        auto dinfo = mx->DirectionById(direction);
        MXTRACE("  Heading: %s", dinfo->Name().c_str());
        
        looking = direction;
        
        std::unique_ptr<mxlocinfo> info ( GetLocInfo() );
        
        if ( !(info->flags&lif_moveforward) ) {
            if ( !IsDawn() || info->flags&lif_blocked ) {
                MXTRACE("  Blocked (not dawn).");
                reachedTarget=true;
            }else{
                if ( mxrandom(255)&1 ) {
                    MXTRACE("  Decided to stop for a turn.");
                    reachedTarget=true;
                }
            }
        }

        if ( reachedTarget ) {
            MXTRACE("  Reached Target.");
            break;
        }
        
        Cmd_WalkForward(false);
        MXTRACE("  Moved to [%d,%d]", location.x, location.y);

        if ( Time()==(mxtime_t)sv_time_night ) {
            MXTRACE("Stopping for the day.");
            break;
        }

        if (energy<MIN_AI_MOVEMENT_ENERGY) {
            MXTRACE("Stopping because tired.");
            break;
        }
        
    }
    
    // anyone here?
    
    // Approach?
    
    // is there 1 enemy here?
    // are we at a stronghold?
    
    std::unique_ptr<mxlocinfo> info ( GetLocInfo() );
   
    bool atStronghold = info->objStrongholds.Count() > 0 ;
    
    if ( !atStronghold ) {
        int enemies=0;
        mxcharacter* recruit=NULL;

        // TODO: Check this info against foe.enemies and foe.characters
        for (u32 ii = 0; ii <info->objCharacters.Count(); ii++) {
            mxcharacter* character = (mxcharacter*)info->objCharacters[ii];
            if ( !IsFriend(character) ) {
                recruit=character;
                enemies++;
            }
        }
        
        MXTRACE("  At location with %d enemies and %d lords.", enemies, info->objCharacters.Count());

        
        if ( (enemies==1) && (info->objCharacters.Count()==2) ) {
            MXTRACE("  Approaching %s", recruit->Longname().c_str());
            AI_Approach(recruit);
        }
    }
    
    MXTRACE("  END");
}

mxcharacter* ddr_character::AI_Approach ( mxcharacter* character )
{
    if ( CheckRecruitChar( character ) ) {
        if ( character->Recruited ( this ) ) {
            MXTRACE("%s has recruited %s", Longname().c_str(),  character->Longname().c_str());
            return character ;
        }
    }
    MXTRACE("%s has failed to recruit %s", Longname().c_str(),  character->Longname().c_str());
    return NULL ;
}

void ddr_character::StartDawn ( void )
{
    if ( !IsDead() ) {
        
        if ( IsResting() )
            IncreaseEnergy(RESTING_ENERGY_BOOST);
        
        if ( IsAIControlled() )
            IncreaseEnergy( DAWN_ENERGY_BOOST_AI );
        else
            IncreaseEnergy( DAWN_ENERGY_BOOST );
        
        time = sv_time_dawn;
  
        flags.Reset ( cf_resting|cf_inbattle|cf_wonbattle|cf_killed_foe|cf_preparesbattle );
        battleloc= mxgridref(-1,-1);
        battleslew = 0;
        battlelost = 0;
        riders.Lost(0);
        riders.Killed(0);
        warriors.Lost(0);
        warriors.Killed(0);
        fighting_against=NULL;
    }
    
    // reset the waiting mode
    if ( WaitMode() == WM_OVERNIGHT )
        Cmd_Wait(WM_NONE);
}

void ddr_character::Turn ( void )
{
    lastlocation = Location();
    
    if ( !IsAIControlled())
        return;
    
    moveCharacterSomewhere();
    
}

void ddr_character::Displace()
{
    RULEFLAGS rule = IsAIControlled() ? RF_AI_IMPASSABLE_MOUNTAINS : RF_IMPASSABLE_MOUNTAINS ;
    bool isImpassableRuleEnabled = mx->isRuleEnabled(rule);
              
    Flags().Reset(cf_tunnel|cf_preparesbattle);

    // if impassable mountains is enabled then we can't use the default
    // ddr displacement because it moves more than one location
    if (isImpassableRuleEnabled) {
        mxcharacter::Displace();
        return;
    }

    mxgridref loc;
    do {
        loc = Location();
        loc.x ^= (mxrandom(255)&3);
        loc.y ^= (mxrandom(255)&3);
    } while ( mx->scenario->isLocationImpassable(loc, this) );
    
    Location(loc);
    
    if ( IsRecruited() ) {
        EnterLocation ( loc );
        mx->scenario->MakeMapAreaVisible(Location(), this);
    }
    
}



mxcharacter* ddr_character::Cmd_Approach ( mxcharacter* character )
{
    if ( !mx->scenario->IsFeature(SF_APPROACH_DDR) ) {
        return mxcharacter::Cmd_Approach( character );
    }

    SetLastCommand ( CMD_APPROACH, IDT_NONE );

    // the leader of the group initiates the approach
    if ( IsFollowing() ) {
        // make the leader look to the same place as us!
        Following()->Cmd_LookDir(Looking());
        return Following()->Cmd_Approach(character);
    }
    
    // get location info
    std::unique_ptr<mxlocinfo> info ( GetLocInfo() );

    // are we allowed to approach ?
    if ( !info->flags.Is(lif_recruitchar) ) {
        return nullptr ;
    }

    // get the default character
    if ( character == nullptr ) {
        character = static_cast<mxcharacter*>(info->objRecruit.First());
        RETURN_IF_NULL(character) nullptr;
    }

    auto will_perform_recruit = CheckRecruitChar( character )
        ? static_cast<mxcharacter*>(this)
        : nullptr;
    
    // if we can't recruit, can any of my followers?
    if ( will_perform_recruit == nullptr && HasFollowers() ) {
        entities followers;
        mx->scenario->GetCharacterFollowers(this, followers);
        for ( u32 ii=0; ii<followers.Count(); ii++ ) {
            auto follower = static_cast<mxcharacter*>(followers[ii]);
            if ( follower->CheckRecruitChar( character ) ) {
                will_perform_recruit = follower ;
                break;
            }
        }
    }
    
    // 1. move all characters into new location
    Cmd_WalkForward (false);
    
    if ( will_perform_recruit ) {
        if ( character->Recruited ( will_perform_recruit ) ) {
            SetLastCommand ( CMD_APPROACH, mxentity::SafeIdt(character) );
            character->SetLastCommand ( CMD_APPROACHED, mxentity::SafeIdt(this) );
            CommandTakesTime(true);
            return character;
        }
    }else{
    // 2. Night
        EnterBattle();
        return nullptr ;
    }

    CommandTakesTime(false);
    return nullptr ;
}


} // namespace tme

#endif // _DDR_
