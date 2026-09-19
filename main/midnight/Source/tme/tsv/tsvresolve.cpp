//
//  tsvresolve.cpp
//  midnight
//

#include "../baseinc/tme_internal.h"
#include "tsvresolve.h"

namespace tme {

mxid ResolveTypedId ( const TsvSymbolTable& symbols, const std::string& name, id_type_t expectedType )
{
    if ( name.empty() )
        return IDT_NONE;

    mxid id = symbols.Resolve(name);
    if ( id == IDT_NONE )
        return IDT_NONE;

    if ( ID_TYPE(id) != expectedType ) {
        MXTRACE("TsvResolve: symbol '%s' is type %d, expected type %d", name.c_str(), (int)ID_TYPE(id), (int)expectedType);
        return IDT_NONE;
    }

    return id;
}

mxcharacter* ResolveCharacter ( const TsvSymbolTable& symbols, const std::string& name )
{
    return mx->CharacterById(GET_ID(ResolveTypedId(symbols, name, IDT_CHARACTER)));
}

mxobject* ResolveObject ( const TsvSymbolTable& symbols, const std::string& name )
{
    return mx->ObjectById(GET_ID(ResolveTypedId(symbols, name, IDT_OBJECT)));
}

mxstronghold* ResolveStronghold ( const TsvSymbolTable& symbols, const std::string& name )
{
    return mx->StrongholdById(GET_ID(ResolveTypedId(symbols, name, IDT_STRONGHOLD)));
}

mxroutenode* ResolveRouteNode ( const TsvSymbolTable& symbols, const std::string& name )
{
    return mx->RouteNodeById(GET_ID(ResolveTypedId(symbols, name, IDT_ROUTENODE)));
}

mxitem* ResolveItem ( const TsvSymbolTable& symbols, const std::string& name )
{
    if ( name.empty() )
        return nullptr;
    return (mxitem*)mx->EntityByIdt(symbols.Resolve(name));
}

mxrace_t ResolveRace ( const TsvSymbolTable& symbols, const std::string& name )
{
    return (mxrace_t)GET_ID(ResolveTypedId(symbols, name, IDT_RACEINFO));
}

mxgender_t ResolveGender ( const TsvSymbolTable& symbols, const std::string& name )
{
    return (mxgender_t)GET_ID(ResolveTypedId(symbols, name, IDT_GENDERINFO));
}

mxdir_t ResolveDirection ( const TsvSymbolTable& symbols, const std::string& name )
{
    return (mxdir_t)GET_ID(ResolveTypedId(symbols, name, IDT_DIRECTIONINFO));
}

mxterrain_t ResolveTerrain ( const TsvSymbolTable& symbols, const std::string& name )
{
    return (mxterrain_t)GET_ID(ResolveTypedId(symbols, name, IDT_TERRAININFO));
}

mxunit_t ResolveUnitType ( const TsvSymbolTable& symbols, const std::string& name )
{
    return (mxunit_t)GET_ID(ResolveTypedId(symbols, name, IDT_UNITINFO));
}

mxthing_t ResolveThing ( const TsvSymbolTable& symbols, const std::string& name )
{
    return (mxthing_t)GET_ID(ResolveTypedId(symbols, name, IDT_OBJECT));
}

#if defined(_DDR_)
mxobjtype_t ResolveObjectType ( const TsvSymbolTable& symbols, const std::string& name )
{
    return (mxobjtype_t)GET_ID(ResolveTypedId(symbols, name, IDT_OBJECT_TYPE));
}

mxobjpower_t ResolveObjectPower ( const TsvSymbolTable& symbols, const std::string& name )
{
    return (mxobjpower_t)GET_ID(ResolveTypedId(symbols, name, IDT_OBJECT_POWER));
}
#endif

}
// namespace tme
