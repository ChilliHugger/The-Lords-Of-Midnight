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

    u32 count = (u32)neighbours.size();
    if ( ar.IsStoring() ) {
        ar << count;
        for ( auto id : neighbours )
            ar << id;
    } else {
        ar >> count;
        neighbours.resize(count);
        for ( auto& id : neighbours )
            ar >> id;
    }
}

void citadel_area::LoadTsv ( const TsvRow& row )
{
    mxarea::LoadTsv(row);

    neighbours.clear();
    for ( auto& symbol : row.GetSymbolList(TsvField::Area::Neighbours, ',') ) {
        auto id = ResolveTypedId(row.Symbols(), symbol, IDT_AREAINFO);
        if ( id != IDT_NONE )
            neighbours.push_back(GET_ID(id));
    }
}

bool citadel_area::Borders ( const mxarea* area ) const
{
    return area != nullptr
        && std::find(neighbours.begin(), neighbours.end(), area->Id()) != neighbours.end();
}

} // namespace tme
#endif // _CITADEL_
