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

void citadel_battle::CharacterLosesEnergy ( mxcharacter* character )
{
    auto lord = static_cast<citadel_character*>(character);

    if ( lord != nullptr && lord->WeaponPower() == OP_BATTLE_TIRELESS )
        return;

    lom_battle::CharacterLosesEnergy(character);
}

} // namespace tme
#endif // _CITADEL_
