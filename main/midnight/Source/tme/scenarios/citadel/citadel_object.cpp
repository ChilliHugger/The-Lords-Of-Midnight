/*
 * FILE:    citadel_object.cpp
 *
 * PROJECT: citadel
 *
 * PURPOSE: The objects of the Citadel: the artefacts a lord can be sent to find, take or destroy.
 *
 */

#include "../../baseinc/tme_internal.h"
#include "scenario_citadel_internal.h"
#include "../../tsv/tsvfields.h"
#include "../../tsv/tsvresolve.h"

#if defined(_CITADEL_)
namespace tme {

citadel_object::citadel_object()
{
}

void citadel_object::Serialize ( archive& ar )
{
    mxobject::Serialize ( ar );
}

void citadel_object::LoadTsv ( const TsvRow& row )
{
    mxobject::LoadTsv(row);
}

bool citadel_object::IsOnMap () const
{
    if ( !IsArtefact() || IsDestroyed() || IsCarried() )
        return false;
    if ( !mx->gamemap->IsLocOnMap(Location()) )
        return false;
    return true;
}

} // namespace tme
#endif // _CITADEL_
