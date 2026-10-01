/*
 * FILE:    citadel_processor_battle.cpp
 *
 * PROJECT: citadel
 *
 * PURPOSE: The Citadel's battle, which is Lords of Midnight's with the bystanders left out.
 *
 */

#include "../../baseinc/tme_internal.h"
#include "citadel_processor_battle.h"

#include <memory>
#include <vector>

#if defined(_CITADEL_)
namespace tme {

//
// Is anyone here to stand against Boroth's host? A garrison counts even when it has been emptied -
// that is how an undefended keep falls - but a lord who takes no part does not, and nor do his
// men. Without this, a place holding only the enemy and bystanders (the Dark Fey keeps, and the
// dungeons of Maranor) announced a battle every night.
//
bool citadel_battle::HasDefenders() const
{
    if ( !lom_battle::HasDefenders() )
        return false;

    for ( auto army : info->armies ) {
        if ( army->race != RA_ENEMY && TakesPart(army) )
            return true;
    }

    for ( auto character : info->objCharacters ) {
        if ( TakesPart(character) )
            return true;
    }

    return false;
}

bool citadel_battle::TakesPart ( const mxarmy* army ) const
{
    return army->armytype != AT_CHARACTER
        || TakesPart(static_cast<const mxcharacter*>(army->parent));
}

bool citadel_battle::TakesPart ( const mxcharacter* character ) const
{
    return character->TakesPartInBattle();
}

static u32 Walls ( const mxarmy* army )
{
    return static_cast<const citadel_stronghold*>(army->parent)->DefenceMultiplier();
}

void citadel_battle::PrepareArmies()
{
    for ( auto army : info->armies ) {
        if ( army->armytype == AT_STRONGHOLD )
            army->total *= Walls(army);
    }
}

void citadel_battle::UpdateStrongholdArmy ( mxarmy* army )
{
    auto walls = Walls(army);
    army->total = (army->total + walls - 1) / walls;

    lom_battle::UpdateStrongholdArmy(army);
}

mxcharacter* citadel_battle::Liberator() const
{
    mxcharacter* lord = nullptr;
    for ( auto character : info->objCharacters ) {
        CONTINUE_IF( !TakesPart(character) );
        if ( character->IsRecruited() )
            return lom_battle::Liberator();
        if ( lord == nullptr )
            lord = character;
    }
    return lord != nullptr ? lord : lom_battle::Liberator();
}

void citadel_battle::Duel ( mxcharacter* attacker, mxcharacter* defender )
{
    struct side_t {
        mxcharacter*                        lord;
        std::vector<std::unique_ptr<mxarmy>> armies;
        c_army                              standing;
    };

    auto muster = []( mxcharacter* lord ) {
        side_t side { lord, {}, {} };
        auto info = lord->GetLocInfo();
        auto enlist = [&]( u32 total, mxunit_t type, s32 success ) {
            if ( total == 0 )
                return;
            auto army = new mxarmy();
            army->armytype = AT_CHARACTER;
            army->parent = lord;
            army->race = lord->Race();
            army->type = type;
            army->total = total;
            army->success = success;
            army->killed = 0;
            army->loyalto = lord->NormalisedLoyalty();
            side.armies.emplace_back(army);
            side.standing.push_back(army);
        };
        enlist(lord->warriors.Total(), UT_WARRIORS, lord->warriors.BattleSuccess(*info, lord));
        enlist(lord->riders.Total(), UT_RIDERS, lord->riders.BattleSuccess(*info, lord));
        return side;
    };

    auto strike = [this]( side_t& from, side_t& at ) {
        from.lord->battleslew += Fight(from.lord->FightStrength(), from.lord->energy + 128, at.standing);
        for ( auto& army : from.armies ) {
            if ( army->total )
                army->killed += Fight(army->total / 5, army->success, at.standing);
        }
    };

    auto a = muster(attacker);
    auto d = muster(defender);

    for ( auto lord : { attacker, defender } ) {
        lord->EnterBattle();
        lord->battleloc = defender->Location();
    }

    strike(a, d);
    strike(d, a);

    for ( auto side : { &a, &d } ) {
        for ( auto& army : side->armies )
            UpdateCharacterArmy(army.get());
        CharacterLosesEnergy(side->lord);
    }

    mxcharacter* loser = nullptr;
    if ( !a.standing.empty() && d.standing.empty() )
        loser = defender;
    else if ( a.standing.empty() && !d.standing.empty() )
        loser = attacker;
    else if ( a.standing.empty() && d.standing.empty() ) {
        auto might = attacker->FightStrength() + defender->FightStrength();
        loser = might == 0 || mxrandom(0, (int)might - 1) < (int)attacker->FightStrength() ? defender : attacker;
    }

    if ( loser == nullptr ) {
        CharacterContinuesBattle(attacker);
        CharacterContinuesBattle(defender);
    } else {
        CharacterWinsBattle(loser == attacker ? defender : attacker);
        CharacterLosesBattle(loser);
    }

    Announce(defender->Location());
}

void citadel_battle::CharacterLosesEnergy ( mxcharacter* character )
{
    auto lord = CitadelLord(character);

    if ( lord != nullptr && lord->WeaponPower() == OP_BATTLE_TIRELESS )
        return;

    lom_battle::CharacterLosesEnergy(character);
}

} // namespace tme
#endif // _CITADEL_
