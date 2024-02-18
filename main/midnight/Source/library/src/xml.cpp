#include "../inc/mxtypes.h"
#include "../inc/xml.h"
#include "../inc/misc.h"

#include <string>

#if !defined _MSC_VER
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-undefined-compare"
#endif

using namespace chilli::lib::StringExtensions;

namespace chilli {
    namespace lib {

token_t token_Bool[] = {
    { "yes",    true },
    { "true",   true },
    { "no",     false },
    { "false",  false },
    };


xml::xml()
{
}

xml::~xml()
{
}

bool xml::Load ( LPCSTR filename )
{
u8* data = NULL ;
u32 size=0;

    data = (u8*)os::filemanager::Load ( filename, &size );
    if ( data == nullptr || size == 0 )
        return false;

    Parse((const char*)data, size);

    SAFEDELETE ( data );

    return true ;
}

XmlNode* xml::Find ( LPCSTR section )
{
    if ( this == nullptr)
        return nullptr;
    return FirstChildElement(section)->ToElement();
}

XmlNode* xml::Find ( XmlNode* node, LPCSTR section )
{
    if ( node == nullptr)
        return nullptr;
    return (XmlNode*)node->FirstChildElement(section)->ToElement();
}

bool xml::IsType(XmlNode* node, const std::string& name)
{
    if(node==nullptr)
        return false;
    return stringicompare(node->Value(),name);
}

int xml::Count (XmlNode* node)
{
    if ( node == nullptr)
        return 0;

    int count=0;
    FOREACHELEMENT(node,e) {
        count++;
    }
    return count;
}

XmlNode* xml::Find( XmlNode* node, LPCSTR element, LPCSTR id )
{
    FOREACHELEMENT(node,e){
        if ( IsType(e,element) ) {
            if ( stringicompare(ReadStr(e,"id"), id) ) {
                return e;
            }
        }
    }
    return nullptr ;
}

std::string xml::ReadElement( XmlNode* node, LPCSTR element, const std::string& id, LPCSTR tag )
{
    FOREACHELEMENT(node,e){
        if ( IsType(e,element) ) {
            if (stringicompare(ReadStr(e,"id"), id)) {
                return ReadStr(e,tag);
            }
        }
    }
    return "" ;
}

std::string xml::ReadItemEx ( XmlNode* node, LPCSTR name )
{
    if ( node == nullptr)
        return "";

    // read an attribute first
    auto value = ((TiXmlElement*)node)->Attribute(name);

    // try this
    if ( value == nullptr) {
        if ( IsType(node,name) ) {
            TiXmlText* m1 = node->FirstChild()->ToText();
            if ( m1 )
                value = m1->Value();
        }
    }

    // try element
    if ( value == nullptr) {
        FOREACHELEMENT(node,e){
            if ( IsType(e,name) ) {
                TiXmlText* m1 = e->FirstChild()->ToText();
                if ( m1 )
                    value = m1->Value();
                break;
            }
        }
    }

    if(value==nullptr) {
        return "";
    }

    return value ;
}

std::string xml::ReadItem ( XmlNode* node, LPCSTR name, LPCSTR defaultvalue )
{
    auto value = ReadItemEx(node,name);
    if ( value.empty())
        return defaultvalue;
    return value;
}

std::string xml::ReadStr( XmlNode* node, LPCSTR name, LPCSTR defaultvalue )
{
    return ReadItem ( node, name, defaultvalue );
}

char  xml::ReadChar( XmlNode* node, LPCSTR name, char defaultvalue )
{
    std::string item = ReadItem ( node, name, "" );
    if ( item.empty() ) {
        return defaultvalue;
    }
    return item[0];
}

f32 xml::ReadItem ( XmlNode* node, LPCSTR name, f32 defaultvalue )
{
    auto value = ReadItemEx(node,name);
    if ( value.empty())
        return defaultvalue;
    return (f32) atof ( value );
}

f32 xml::ReadFloat ( XmlNode* node, LPCSTR name, f32 defaultvalue )
{
    return ReadItem( node,name,defaultvalue) ;
}

int xml::ReadItem ( XmlNode* node, LPCSTR name, int defaultvalue )
{
    auto value = ReadItemEx(node,name);
    if ( value.empty() )
        return defaultvalue;
    return atoi ( value );
}

int xml::ReadInt( XmlNode* node, LPCSTR name, int defaultvalue )
{
    return ReadItem(node,name,defaultvalue) ;
}


bool xml::Exists( XmlNode* node, LPCSTR name )
{
    if ( node == NULL )
        return false;

    if ( ((TiXmlElement*)node)->Attribute(name) == nullptr)
        return false;

    return true;
}

size xml::ReadSize( XmlNode* node, LPCSTR name, size& s1 )
{
    size s;
    collections::c_s32 c;

    s.cx = s1.cx;
    s.cy = s1.cy;

    if ( ReadArray ( node,name, c ) ) {
        s.cx = c[0];
        s.cy = c[1];
    }
    return s;
}
    
point xml::ReadPoint( XmlNode* node, LPCSTR name, point& p1 )
{
    point p;
    collections::c_s32 c;

    p.x = p1.x;
    p.y = p1.y;

    if ( ReadArray ( node, name, c ) ) {
        p.x = c[0];
        p.y = c[1];
    }
    return p;
}

int xml::ReadColour( XmlNode* node, LPCSTR name, int defaultvalue )
{
int value=0;

    auto val = ReadStr(node, name);
    
    if ( val.empty() )
        return defaultvalue;

    if ( val[0] == '#' ) {
        sscanf(&val[1],"%x",&value);
        return value;
    }

    return atoi(val) ;
}


int xml::ReadToken( XmlNode* node, LPCSTR token, token_t array[], int max, int defaultvalue )
{
    auto val = ReadStr(node,token);
    if ( val.empty() )
        return defaultvalue;

    int temp =  GetToken ( val, array, max );
    if ( temp ) return temp ;
    return atoi(val);
}

bool xml::ReadBool( XmlNode* node, LPCSTR name, bool defaultvalue )
{
    return (bool)ReadToken( node, name, token_Bool, NUMELE(token_Bool), defaultvalue );
}

int ConvertArray ( const std::string value, collections::c_s32& c, char delim )
{
 c_string values;
 
    if (value.empty()) return 0;
 
    StringExtensions::split(value, delim, values);
 
    for( auto v : values) {
        c.Add( StringExtensions::atoi(v) ) ;
    }
 
    return c.Count();
}

int ConvertArray ( const std::string value, collections::c_float& c, char delim )
{
 c_string values;
 
    if (value.empty()) return 0;
 
    StringExtensions::split(value, delim, values);
 
    for( auto v : values) {
        c.Add( StringExtensions::atof(v) ) ;
    }
 
    return c.Count();
}

int xml::ReadArray ( XmlNode* node, LPCSTR name, collections::c_s32& c, char delim )
{
    std::string value = ReadStr( node, name );
    if ( value.empty() )
        return 0;
    return ConvertArray(value,c,delim);

}

int xml::ReadArray ( XmlNode* node, LPCSTR name, collections::c_s32& c )
{
    return ReadArray(node,name,c,',');
}

std::string xml::ReadValue ( XmlNode* node, LPCSTR name )
{
    XmlNode* m = Find(node,name);
    if ( m ) {
        TiXmlText* m1 = m->FirstChild()->ToText();
        if ( m1 )
            return m1->Value();
    }
    return nullptr;

}

int xml::ReadValueArray ( XmlNode* node, LPCSTR name, collections::c_s32& c )
{
    std::string s = ReadValue(node,name);
    return ConvertArray(s,c,',');
}

int xml::ReadValueArray ( XmlNode* node, LPCSTR name, collections::c_float& c )
{
    std::string s = ReadValue(node,name);
    return ConvertArray(s,c,',');
}

int xml::ReadIntProperty ( XmlNode* node, LPCSTR name, int defaultvalue )
{
    XmlNode* e = nullptr;
    if ( (e = Find( node, "integer", name )) ) {
        return ReadInt(e, "value",defaultvalue);
    }
    return defaultvalue ;
}

bool xml::ReadBoolProperty ( XmlNode* node, LPCSTR name, bool defaultvalue )
{
    XmlNode* e = nullptr;
    if ( (e = Find( node, "bool", name )) ) {
        return ReadBool(e,"value",defaultvalue);
    }
    return defaultvalue ;
}

std::string xml::ReadStrProperty ( XmlNode* node, LPCSTR name, LPCSTR defaultvalue )
{
    XmlNode* e = nullptr;
    if ( (e = Find( node, "string", name )) ) {
        return ReadStr(e,"value",defaultvalue);
    }
    return defaultvalue ;
}

f32 xml::ReadFloatProperty ( XmlNode* node, LPCSTR  name, f32 defaultvalue )
{
    XmlNode* e = nullptr;
    if ( (e = Find( node, "float", name )) ) {
        return ReadFloat(e,"value",defaultvalue);
    }
    return defaultvalue ;
}


#if !defined _MSC_VER
#pragma clang diagnostic pop
#endif
        
    } // namespace lib
} // namespace tme
