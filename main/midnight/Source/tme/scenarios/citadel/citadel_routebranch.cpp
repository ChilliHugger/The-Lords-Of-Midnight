/*
 * FILE:    citadel_routebranch.cpp
 *
 * PROJECT: citadel
 *
 * PURPOSE: A branch of a route node: the direction it leaves in and how hard the way is.
 *
 */

#include "../../baseinc/tme_internal.h"
#include "scenario_citadel_internal.h"

#if defined(_CITADEL_)
namespace tme {

void citadel_routebranch::SetRoute ( mxdir_t dir, u32 w )
{
    direction = dir;
    weight = w;
}

void citadel_routebranch::Serialize ( archive& ar )
{
    mxroutebranch::Serialize ( ar );

    if ( ar.IsStoring() ) {
        WRITE_ENUM(direction);
        ar << weight;
    } else {
        READ_ENUM(direction);
        ar >> weight;
    }
}

} // namespace tme
#endif // _CITADEL_
