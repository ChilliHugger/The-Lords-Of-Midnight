/*
 * FILE:    citadel_area.cpp
 *
 * PROJECT: citadel
 *
 * PURPOSE: The regions of the Blood March and which of them border one another.
 *
 */

#include "../../baseinc/tme_internal.h"
#include "scenario_citadel_internal.h"
#include "../../tsv/tsvfields.h"
#include "../../tsv/tsvresolve.h"

#include <algorithm>

#if defined(_CITADEL_)
namespace tme {

void citadel_area::Serialize ( archive& ar )
{
    mxarea::Serialize(ar);
    
    if ( ar.IsStoring() ) {
        ar << neighbours;
    } else {
        ar >> neighbours;
    }
}

void citadel_area::LoadTsv ( const TsvRow& row )
{
    mxarea::LoadTsv(row);

    neighbours.clear();
    for ( auto& symbol : row.GetSymbolList(TsvField::Area::Neighbours, ',') ) {
        auto id = ResolveTypedId(row.Symbols(), symbol, IDT_AREAINFO);
        if ( id != IDT_NONE )
            neighbours.push_back(id);
    }
}

bool citadel_area::Borders ( const mxarea* area ) const
{
    return std::find(neighbours.begin(), neighbours.end(), mxentity::SafeIdt(area)) != neighbours.end();
}

} // namespace tme
#endif // _CITADEL_
