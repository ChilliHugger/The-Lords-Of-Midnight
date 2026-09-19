//
//  tsvsymboltable.h
//  midnight
//
//  Global Symbol -> MXId lookup, built once across every bundled .tsv table
//  before any entity's fields are populated, so that a symbol referenced
//  from any table (e.g. a character's "Liege") can be resolved regardless
//  of which table actually defines it.
//

#ifndef _TSVSYMBOLTABLE_H_INCLUDED_
#define _TSVSYMBOLTABLE_H_INCLUDED_

#include <string>
#include <unordered_map>
#include "../../library/inc/mxtypes.h"

namespace tme {

    class TsvSymbolTable
    {
    public:
        void Add ( mxid id, const std::string& symbol );

        // returns 0 (IDT_NONE) if the symbol is blank or unknown
        mxid Resolve ( const std::string& symbol ) const;

        // resolves a strings.tsv symbol to the raw 0-based index that
        // mxtext::SystemString()/CookedSystemString() expect - NOT the
        // same as GET_ID(Resolve(symbol)), which is 1-based (see
        // mxtext::StringByName). Returns 0 if the symbol is blank/unknown.
        u32 ResolveStringIndex ( const std::string& symbol ) const;

    private:
        std::unordered_map<std::string,mxid> m_symbols;
    };

}
// namespace tme

#endif //_TSVSYMBOLTABLE_H_INCLUDED_
