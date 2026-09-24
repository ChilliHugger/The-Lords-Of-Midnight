//
//  tsvresolve.h
//  midnight
//
//  Typed, type-checked symbol resolution for LoadTsv - one function per
//  result type, mirroring how chilli::lib::archive has one operator per
//  type. Each function resolves a symbol through the given TsvSymbolTable
//  and validates it names an entity of the expected type before handing
//  back a usable pointer/enum, tracing and returning a safe default (null
//  / *_NONE) instead of silently using a mismatched reference.
//

#ifndef _TSVRESOLVE_H_INCLUDED_
#define _TSVRESOLVE_H_INCLUDED_

#include <string>
#include "tsvsymboltable.h"

namespace tme {

    class mxcharacter;
    class mxobject;
    class mxstronghold;
    class mxroutenode;
    class mxitem;

    // validates that 'name' resolves to an entity of 'expectedType' and
    // returns its raw mxid (type+id encoded), or IDT_NONE if the symbol is
    // blank, unknown, or names a different entity type - for fields typed
    // as a raw mxid rather than a pointer (e.g. mxvictory::mission)
    mxid ResolveTypedId ( const TsvSymbolTable& symbols, const std::string& name, id_type_t expectedType );

    mxcharacter*  ResolveCharacter ( const TsvSymbolTable& symbols, const std::string& name );
    mxobject*     ResolveObject ( const TsvSymbolTable& symbols, const std::string& name );
    mxstronghold* ResolveStronghold ( const TsvSymbolTable& symbols, const std::string& name );
    mxroutenode*  ResolveRouteNode ( const TsvSymbolTable& symbols, const std::string& name );

    // generic entity reference (e.g. mxobject::carriedby) - deliberately
    // untyped, matching operator>>(archive&, mxitem*&)'s own contract
    mxitem* ResolveItem ( const TsvSymbolTable& symbols, const std::string& name );

    // small compile-time enums, each backed by its own info table
    mxrace_t    ResolveRace ( const TsvSymbolTable& symbols, const std::string& name );
    mxgender_t  ResolveGender ( const TsvSymbolTable& symbols, const std::string& name );
    mxdir_t     ResolveDirection ( const TsvSymbolTable& symbols, const std::string& name );
    mxterrain_t ResolveTerrain ( const TsvSymbolTable& symbols, const std::string& name );
    mxunit_t    ResolveUnitType ( const TsvSymbolTable& symbols, const std::string& name );
    mxthing_t   ResolveThing ( const TsvSymbolTable& symbols, const std::string& name );

#if defined(_DDR_) || defined(_CITADEL_)
    mxobjtype_t  ResolveObjectType ( const TsvSymbolTable& symbols, const std::string& name );
    mxobjpower_t ResolveObjectPower ( const TsvSymbolTable& symbols, const std::string& name );
#endif

}
// namespace tme

#endif //_TSVRESOLVE_H_INCLUDED_
