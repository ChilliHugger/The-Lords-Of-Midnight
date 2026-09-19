//
//  tsvsymboltable.cpp
//  midnight
//

#include "../baseinc/tme_internal.h"
#include "tsvsymboltable.h"

namespace tme {

void TsvSymbolTable::Add ( mxid id, const std::string& symbol )
{
    if ( symbol.empty() )
        return;

    m_symbols[symbol] = id;
}

mxid TsvSymbolTable::Resolve ( const std::string& symbol ) const
{
    if ( symbol.empty() )
        return IDT_NONE;

    auto it = m_symbols.find(symbol);
    return it != m_symbols.end() ? it->second : (mxid)IDT_NONE;
}

u32 TsvSymbolTable::ResolveStringIndex ( const std::string& symbol ) const
{
    u32 id = GET_ID(Resolve(symbol));
    return id > 0 ? id - 1 : 0;
}

}
// namespace tme
