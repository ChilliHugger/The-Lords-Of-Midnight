//
//  tsvfields.h
//  midnight
//
//  Column-name constants for every .tsv table the runtime loader reads.
//  Single source of truth shared by each entity's LoadTsv (so a typo is a
//  compile error, not a silently-blank field) and by TsvDatabaseLoader's
//  required-column checks against each file's header row.
//

#ifndef _TSVFIELDS_H_INCLUDED_
#define _TSVFIELDS_H_INCLUDED_

namespace tme {
namespace TsvField {

    // present on every table (entity and info alike)
    constexpr const char* Version  = "Version";
    constexpr const char* Id       = "Id";
    constexpr const char* Symbol   = "Symbol";
    constexpr const char* Flags    = "Flags";
    constexpr const char* Location = "Location";
    constexpr const char* Name     = "Name";

    namespace Character {
        constexpr const char* LongName    = "Long Name";
        constexpr const char* ShortName   = "Short Name";
        constexpr const char* Recruit     = "Recruit";
        constexpr const char* Group       = "Group";
        constexpr const char* Looking     = "Looking";
        constexpr const char* Time        = "Time";
        constexpr const char* Race        = "Race";
        constexpr const char* Gender      = "Gender";
        constexpr const char* Loyalty     = "Loyalty";
        constexpr const char* Energy      = "Energy";
        constexpr const char* Reckless    = "Reckless";
        constexpr const char* Strength    = "Strength";
        constexpr const char* Cowardly    = "Cowardly";
        constexpr const char* Courage     = "Courage";
        constexpr const char* Despondency = "Despondency";
        constexpr const char* Fear        = "Fear";
        constexpr const char* Orders      = "Orders";
        constexpr const char* Carrying    = "Carrying";
        constexpr const char* Warriors    = "Warriors";
        constexpr const char* Riders      = "Riders";
        constexpr const char* Following   = "Following";
        constexpr const char* Foe         = "Foe";
        constexpr const char* Liege       = "Liege";
        constexpr const char* Traits      = "Traits";
        // DDR only
        constexpr const char* Home           = "Home";
        constexpr const char* DesiredObject  = "Desired Object";
        // CITADEL only. The 1995 personality attributes, as a '+' separated word
        // ("BRAVE+BOLD+EAGER+MIGHTYWARRIOR"). NOT the same vocabulary as Traits:
        // Traits is LoM's 16 (ct_good … ct_treacherous), Qualities is the Citadel's
        // 62. See tsvflags.cpp CharacterQualityBits.
        constexpr const char* Qualities      = "Qualities";
    }

    namespace Object {
        constexpr const char* Kills          = "Kills";
        constexpr const char* Description    = "Description";
        constexpr const char* UseDescription = "Use Description";
        constexpr const char* CarriedBy      = "Carried By";
        // DDR only
        constexpr const char* Type           = "Type";
        constexpr const char* Power          = "Power";
    }

    namespace Regiment {
        constexpr const char* Race    = "Race";
        constexpr const char* Type    = "Type";
        constexpr const char* Total   = "Total";
        constexpr const char* Target  = "Target";
        constexpr const char* Orders  = "Orders";
        constexpr const char* Success = "Success";
        constexpr const char* Loyalty = "Loyalty";
        constexpr const char* Delay   = "Delay";
    }

    namespace Stronghold {
        constexpr const char* OccupyingRace      = "Occupying Race";
        constexpr const char* Race               = "Race";
        constexpr const char* Type               = "Type";
        constexpr const char* Total               = "Total";
        constexpr const char* Min                = "Min";
        constexpr const char* Max                = "Max";
        constexpr const char* StrategicalSuccess = "Strategical Success";
        constexpr const char* OwnerSuccess       = "Owner Success";
        constexpr const char* EnemySuccess       = "Enemy Success";
        constexpr const char* Influence          = "Influence";
        constexpr const char* Respawn            = "Respawn";
        constexpr const char* Occupier           = "Occupier";
        constexpr const char* Owner              = "Owner";
        constexpr const char* Terrain            = "Terrain";
        constexpr const char* Energy             = "Energy"; // DDR only
    }

    namespace RouteNode {
        constexpr const char* RouteNodes = "RouteNodes";
    }

    namespace Mission {
        constexpr const char* Priority   = "Priority";
        constexpr const char* Objective  = "Objective";
        constexpr const char* Condition  = "Condition";
        constexpr const char* References = "References";
        constexpr const char* Points     = "Points";
        constexpr const char* Scorer     = "Scorer";
        constexpr const char* Action     = "Action";
        constexpr const char* ActionId   = "Action Id";
    }

    namespace Victory {
        constexpr const char* Priority = "Priority";
        constexpr const char* Mission  = "Mission";
        constexpr const char* String   = "String";
    }

    namespace Command {
        constexpr const char* SuccessTime = "Success Time";
        constexpr const char* FailureTime = "Failure Time";
    }

    namespace Area {
        constexpr const char* Prefix = "Prefix";
    }

    namespace Gender {
        constexpr const char* PersonalPronoun   = "Personal Pronoun";
        constexpr const char* PossessivePronoun = "Possessive Pronoun";
        constexpr const char* SingularPronoun   = "Singular Pronoun";
    }

    namespace Race {
        constexpr const char* DefaultSoldiersName       = "Default Soldiers Name";
        constexpr const char* Success                   = "Success";
        constexpr const char* InitialMovement           = "Initial Movement";
        constexpr const char* DiagonalMovement          = "Diagonal Movement";
        constexpr const char* RidingMultiplier          = "Riding Multiplier";
        constexpr const char* MovementMax               = "Movement Max";
        constexpr const char* RestAmount                = "Rest Amount";
        constexpr const char* StrongholdStartups        = "Stronghold Startups";
        constexpr const char* MistTimeAdjustment        = "Mist Time Adjustment";
        constexpr const char* MistDespondencyAdjustment = "Mist Despondency Adjustment";
        constexpr const char* EnergyAmount              = "Energy Amount";
        constexpr const char* EnergyAmountRiding        = "Energy Amount Riding";
    }

    namespace Terrain {
        constexpr const char* Preposition  = "Preposition";
        constexpr const char* Description  = "Description";
        constexpr const char* Success      = "Success";
        constexpr const char* Visibility   = "Visibility";
        constexpr const char* Obstruction  = "Obstruction";
        constexpr const char* MovementCost = "MovementCost";
    }

    namespace Unit {
        constexpr const char* Success      = "Success";
        constexpr const char* RestModifier = "Rest Modifier";
    }

    namespace DatabaseString {
        constexpr const char* Text = "Text";
    }

    namespace DatabaseVariable {
        constexpr const char* Value = "Value";
    }

}
// namespace TsvField
}
// namespace tme

#endif //_TSVFIELDS_H_INCLUDED_
