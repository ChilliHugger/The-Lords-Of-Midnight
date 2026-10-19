/*
 * FILE:    citadel_entityfactory.cpp
 *
 * PROJECT: citadel
 *
 * PURPOSE: Creates the Citadel's own kind of each entity as a database or saved game is loaded.
 *
 */

#include "../../baseinc/tme_internal.h"
#include "scenario_citadel_internal.h"

#if defined(_CITADEL_)
namespace tme {

mxroutebranch* citadel_entityfactory::CreateRouteBranch ()
{
    return new citadel_routebranch;
}

mxentity* citadel_entityfactory::Create ( id_type_t type )
{
    switch ( type ) {
        case IDT_CHARACTER:
            return new citadel_character;
        case IDT_STRONGHOLD:
            return new citadel_stronghold;
        case IDT_OBJECT:
            return new citadel_object;
        case IDT_AREAINFO:
            return new citadel_area;
        case IDT_RACEINFO:
            return new citadel_race;
        default:
            break;
    }

    return mxentityfactory::Create ( type );
}

} // namespace tme
#endif // _CITADEL_
