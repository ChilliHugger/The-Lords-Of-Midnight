//
//  tsvtable.cpp
//  midnight
//

#include "../baseinc/tme_internal.h"
#include "tsvtable.h"
#include "tsvresolve.h"
#include <algorithm>

namespace tme {

static std::string TrimCR ( const std::string& value )
{
    if ( !value.empty() && value.back() == '\r' )
        return value.substr(0, value.size()-1);
    return value;
}

static std::string TrimQuote ( const std::string& value )
{
    if ( !value.empty() && value.front() == '"' && value.back() == '"' )
        return value.substr(1, value.size()-2);
    return value;
}


TsvRow::TsvRow ( const TsvSymbolTable& symbols )
    : m_symbols(&symbols)
{
}

void TsvRow::Set ( const std::string& column, const std::string& value )
{
    m_cells[column] = value;
}

bool TsvRow::Has ( const std::string& column ) const
{
    return m_cells.find(column) != m_cells.end();
}

std::string TsvRow::GetString ( const std::string& column, const std::string& def ) const
{
    auto it = m_cells.find(column);
    return it != m_cells.end() ? it->second : def;
}

u32 TsvRow::GetU32 ( const std::string& column, u32 def ) const
{
    std::string value = GetString(column);
    return value.empty() ? def : (u32)chilli::lib::StringExtensions::atol(value);
}

s32 TsvRow::GetS32 ( const std::string& column, s32 def ) const
{
    std::string value = GetString(column);
    return value.empty() ? def : (s32)chilli::lib::StringExtensions::atol(value);
}

bool TsvRow::GetGridRef ( const std::string& column, u32& x, u32& y ) const
{
    std::string value = GetString(column);
    if ( value.empty() )
        return false;

    chilli::collections::c_string parts;
    chilli::lib::StringExtensions::split(value, ',', parts);
    if ( parts.size() < 2 )
        return false;

    x = (u32)chilli::lib::StringExtensions::atol(parts[0]);
    y = (u32)chilli::lib::StringExtensions::atol(parts[1]);
    return true;
}

std::vector<std::string> TsvRow::GetSymbolList ( const std::string& column, char delim ) const
{
    std::vector<std::string> result;

    std::string value = GetString(column);
    if ( value.empty() )
        return result;

    chilli::collections::c_string parts;
    chilli::lib::StringExtensions::split(value, delim, parts);
    for ( auto& part : parts ) {
        if ( !part.empty() )
            result.push_back(part);
    }
    return result;
}

mxcharacter* TsvRow::GetCharacter ( const std::string& column ) const
{
    return ResolveCharacter(*m_symbols, GetString(column));
}

mxobject* TsvRow::GetObject ( const std::string& column ) const
{
    return ResolveObject(*m_symbols, GetString(column));
}

mxstronghold* TsvRow::GetStronghold ( const std::string& column ) const
{
    return ResolveStronghold(*m_symbols, GetString(column));
}

mxitem* TsvRow::GetItem ( const std::string& column ) const
{
    return ResolveItem(*m_symbols, GetString(column));
}

mxrace_t TsvRow::GetRace ( const std::string& column ) const
{
    return ResolveRace(*m_symbols, GetString(column));
}

mxgender_t TsvRow::GetGender ( const std::string& column ) const
{
    return ResolveGender(*m_symbols, GetString(column));
}

mxdir_t TsvRow::GetDirection ( const std::string& column ) const
{
    return ResolveDirection(*m_symbols, GetString(column));
}

mxterrain_t TsvRow::GetTerrain ( const std::string& column ) const
{
    return ResolveTerrain(*m_symbols, GetString(column));
}

mxunit_t TsvRow::GetUnitType ( const std::string& column ) const
{
    return ResolveUnitType(*m_symbols, GetString(column));
}

mxthing_t TsvRow::GetThing ( const std::string& column ) const
{
    return ResolveThing(*m_symbols, GetString(column));
}

mxid TsvRow::GetTypedId ( const std::string& column, id_type_t expectedType ) const
{
    return ResolveTypedId(*m_symbols, GetString(column), expectedType);
}

mxid TsvRow::GetId ( const std::string& column ) const
{
    return m_symbols->Resolve(GetString(column));
}

u32 TsvRow::GetStringIndex ( const std::string& column ) const
{
    return m_symbols->ResolveStringIndex(GetString(column));
}

mxorders_t TsvRow::GetOrders ( const std::string& column ) const
{
    return ParseOrders(GetString(column));
}

#if defined(_DDR_)
mxobjtype_t TsvRow::GetObjectType ( const std::string& column ) const
{
    return ResolveObjectType(*m_symbols, GetString(column));
}

mxobjpower_t TsvRow::GetObjectPower ( const std::string& column ) const
{
    return ResolveObjectPower(*m_symbols, GetString(column));
}
#endif

bool TsvTable::ValidateHeader ( const std::vector<std::string>& requiredColumns ) const
{
    bool ok = true;
    for ( auto& column : requiredColumns ) {
        if ( std::find(m_header.begin(), m_header.end(), column) == m_header.end() ) {
            MXTRACE("TsvTable '%s': missing required column '%s'", m_filename.c_str(), column.c_str());
            ok = false;
        }
    }
    return ok;
}

bool TsvTable::ValidateVersion ( u32 expectedVersion ) const
{
    bool ok = true;
    for ( u32 ii=0; ii<m_rows.size(); ii++ ) {
        u32 version = m_rows[ii].GetU32(TsvField::Version);
        if ( version != expectedVersion ) {
            MXTRACE("TsvTable '%s': row %d has version %d, expected %d", m_filename.c_str(), (int)ii, (int)version, (int)expectedVersion);
            ok = false;
        }
    }
    return ok;
}

bool TsvTable::Load ( const std::string& filename, const TsvSymbolTable& symbols )
{
    m_filename = filename;

    u32 size = 0;
    void* data = chilli::os::filemanager::Load(filename, &size);
    if ( data == nullptr )
        return false;

    std::string content((const char*)data, size);
    chilli::os::filemanager::Unload((u8**)&data);

    auto lines = chilli::lib::StringExtensions::split_by_newline(content);
    if ( lines.empty() )
        return false;

    m_header.clear();
    chilli::collections::c_string headerCells;
    chilli::lib::StringExtensions::split(lines[0], '\t', headerCells);
    for ( auto& cell : headerCells )
        m_header.push_back(TrimQuote(TrimCR(cell)));

    m_rows.clear();
    for ( u32 li=1; li<lines.size(); li++ ) {
        if ( lines[li].empty() )
            continue;

        chilli::collections::c_string cells;
        chilli::lib::StringExtensions::split(lines[li], '\t', cells);

        TsvRow row(symbols);
        for ( u32 ci=0; ci<m_header.size() && ci<cells.size(); ci++ )
            row.Set(m_header[ci], TrimQuote(TrimCR(cells[ci])));

        m_rows.push_back(row);
    }

    return true;
}

}
// namespace tme
