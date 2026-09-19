//
//  tsvtable.h
//  midnight
//
//  Minimal tab-separated-value reader used to load the entity database
//  directly from the bundled .tsv source files at runtime.
//

#ifndef _TSVTABLE_H_INCLUDED_
#define _TSVTABLE_H_INCLUDED_

#include <string>
#include <vector>
#include <map>
#include "../../library/inc/mxtypes.h"
#include "../baseinc/lomxtypes.h"
#include "tsvsymboltable.h"

namespace tme {

    class mxcharacter;
    class mxobject;
    class mxstronghold;
    class mxitem;

    // one row of a .tsv file, addressable by column name. Carries the
    // symbol table it was loaded alongside, so any row can resolve its own
    // entity/enum references without every caller threading one through.
    class TsvRow
    {
    public:
        explicit TsvRow ( const TsvSymbolTable& symbols );

        void Set ( const std::string& column, const std::string& value );
        bool Has ( const std::string& column ) const;

        std::string GetString ( const std::string& column, const std::string& def = "" ) const;
        u32 GetU32 ( const std::string& column, u32 def = 0 ) const;
        s32 GetS32 ( const std::string& column, s32 def = 0 ) const;

        // "x,y" -> true and fills x/y, or false (and leaves x/y untouched) if blank/missing
        bool GetGridRef ( const std::string& column, u32& x, u32& y ) const;

        // splits a delimited list of symbols, e.g. "RN_A,RN_B" or "OB_A|RA_FREE"
        std::vector<std::string> GetSymbolList ( const std::string& column, char delim = ',' ) const;

        // public for the rare caller that needs to resolve a symbol that
        // didn't come from one of this row's own columns (e.g. a list entry)
        const TsvSymbolTable& Symbols () const { return *m_symbols; }

        // typed, type-checked entity/enum lookups - resolve the named
        // column's symbol, tracing and returning a safe default (null /
        // *_NONE) if it's blank, unknown, or names the wrong entity type.
        // See tsvresolve.h.
        mxcharacter*    GetCharacter ( const std::string& column ) const;
        mxobject*       GetObject ( const std::string& column ) const;
        mxstronghold*   GetStronghold ( const std::string& column ) const;
        mxitem*         GetItem ( const std::string& column ) const;
        mxrace_t        GetRace ( const std::string& column ) const;
        mxgender_t      GetGender ( const std::string& column ) const;
        mxdir_t         GetDirection ( const std::string& column ) const;
        mxterrain_t     GetTerrain ( const std::string& column ) const;
        mxunit_t        GetUnitType ( const std::string& column ) const;
        mxthing_t       GetThing ( const std::string& column ) const;
        mxorders_t      GetOrders ( const std::string& column ) const;


        // for fields typed as a raw mxid rather than a pointer (e.g. mxvictory::mission)
        mxid GetTypedId ( const std::string& column, id_type_t expectedType ) const;

        // untyped symbol resolution, for fields genuinely polymorphic across
        // entity types (e.g. mxregiment::targetid) - see tsvresolve.h
        mxid GetId ( const std::string& column ) const;

        // resolves a strings.tsv symbol to the raw 0-based system-string index
        u32 GetStringIndex ( const std::string& column ) const;

    private:
        const TsvSymbolTable*             m_symbols;
        std::map<std::string,std::string> m_cells;
    };

    // a parsed .tsv file: header-driven rows, in file order
    class TsvTable
    {
    public:
        bool Load ( const std::string& filename, const TsvSymbolTable& symbols );

        u32 Count() const { return (u32)m_rows.size(); }
        const TsvRow& Row ( u32 index ) const { return m_rows[index]; }

        // traces and returns false if the header row is missing any of
        // requiredColumns - catches a renamed/malformed column at load
        // time instead of it silently reading back as blank everywhere
        bool ValidateHeader ( const std::vector<std::string>& requiredColumns ) const;

        // traces and returns false if any row's Version column doesn't
        // equal expectedVersion - every row in an exported table should
        // carry the same version as the database it belongs to
        bool ValidateVersion ( u32 expectedVersion ) const;

        std::vector<TsvRow>::const_iterator begin() const { return m_rows.begin(); }
        std::vector<TsvRow>::const_iterator end() const { return m_rows.end(); }

    private:
        std::string                 m_filename;
        std::vector<std::string>   m_header;
        std::vector<TsvRow>        m_rows;
    };

}
// namespace tme

#endif //_TSVTABLE_H_INCLUDED_
