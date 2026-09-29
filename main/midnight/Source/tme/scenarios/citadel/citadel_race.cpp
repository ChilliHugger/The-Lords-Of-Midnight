/*
 * FILE:    citadel_race.cpp
 *
 * PROJECT: citadel
 *
 * PURPOSE: The peoples of the Blood March, and the old feuds that keep some of them out of
 *          each other's garrisons.
 *
 */

#include "../../baseinc/tme_internal.h"
#include "scenario_citadel_internal.h"
#include "../../tsv/tsvfields.h"
#include "../../tsv/tsvresolve.h"

#include <algorithm>

#if defined(_CITADEL_)
namespace tme {

void citadel_race::Serialize ( archive& ar )
{
    mxrace::Serialize(ar);

    u32 count = (u32)feuds.size();
    if ( ar.IsStoring() ) {
        ar << count;
        for ( auto race : feuds )
            WRITE_ENUM(race);
    } else {
        ar >> count;
        feuds.resize(count);
        for ( auto& race : feuds )
            READ_ENUM(race);
    }
}

void citadel_race::LoadTsv ( const TsvRow& row )
{
    mxrace::LoadTsv(row);

    feuds.clear();
    for ( auto& symbol : row.GetSymbolList(TsvField::Race::Feuds, ',') ) {
        auto id = ResolveTypedId(row.Symbols(), symbol, IDT_RACEINFO);
        if ( id != IDT_NONE )
            feuds.push_back((mxrace_t)GET_ID(id));
    }
}

bool citadel_race::IsFeudingWith ( mxrace_t race ) const
{
    return std::find(feuds.begin(), feuds.end(), race) != feuds.end();
}

} // namespace tme
#endif // _CITADEL_
