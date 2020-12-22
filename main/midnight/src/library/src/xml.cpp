#include "../inc/mxtypes.h"
#include "../inc/xml.h"

#include <string>

#if !defined _MSC_VER
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-undefined-compare"
#endif

namespace chilli {
    namespace lib {

token_t token_Bool[] = {
    { "yes",    true },
    { "true",    true },
    { "no",        false },
    { "false",    false },
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

bool xml::IsType(XmlNode* node, LPCSTR name)
{
    if(node==nullptr)
        return false;
    return c_stricmp(node->Value(),name) == 0;
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
            if ( c_stricmp(ReadStr(e,"id"), id) == 0 ) {
                return e;
            }
        }
    }
    return nullptr ;
}

LPCSTR xml::ReadElement( XmlNode* node, LPCSTR element, LPCSTR id, LPCSTR tag )
{
    FOREACHELEMENT(node,e){
        if ( IsType(e,element) ) {
            if (c_stricmp(ReadStr(e,"id"), id) == 0 ) {
                return ReadStr(e,tag);
            }
        }
    }
    return strNull ;
}

LPCSTR xml::ReadItemEx ( XmlNode* node, LPCSTR name )
{
    if ( node == nullptr)
        return nullptr;

    // read an attribute first
    LPCSTR value = ((TiXmlElement*)node)->Attribute(name);

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

    return value ;
}

LPCSTR xml::ReadItem ( XmlNode* node, LPCSTR name, LPCSTR defaultvalue )
{
    LPCSTR value = ReadItemEx(node,name);
    if ( value == nullptr)
        return defaultvalue;
    return value;
}

LPCSTR  xml::ReadStr( XmlNode* node, LPCSTR name, LPCSTR defaultvalue )
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
    LPCSTR value = ReadItemEx(node,name);
    if ( value == nullptr)
        return defaultvalue;
    return (f32) atof ( value );
}

f32 xml::ReadFloat ( XmlNode* node, LPCSTR name, f32 defaultvalue )
{
    return ReadItem( node,name,defaultvalue) ;
}

int xml::ReadItem ( XmlNode* node, LPCSTR name, int defaultvalue )
{
    LPCSTR value = ReadItemEx(node,name);
    if ( value == nullptr)
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

    LPCSTR val = ReadStr(node, name);
    
    if ( val == NULL )
        return defaultvalue;

    if ( val[0] == '#' ) {
        sscanf(&val[1],"%x",&value);
        return value;
    }

    // find a global colour variable
    //if ( val[0] == '@' ) {
    //    var_t* var = FindGlobalVariable ( &val[1] );
    //    if ( var ) {
    //        if ( var->type == V_RGB ) {
    //            CRgb* rgb = (CRgb*)var->ptr ;
    //            return rgb->rgba;
    //        }
    //        if ( var->type == V_INT ) {
    //            int* integer = (int*)var->ptr ;
    //            return *integer;
    //        }
    //    }
    //    return 0;
    //}

    return atoi(val) ;
}


int xml::ReadToken( XmlNode* node, LPCSTR token, token_t array[], int max, int defaultvalue )
{
    LPCSTR val = ReadStr(node,token, nullptr);
    if ( val == nullptr)
        return defaultvalue;

    int temp =  GetToken ( val, array, max );
    if ( temp ) return temp ;
    return atoi(val);
}

bool xml::ReadBool( XmlNode* node, LPCSTR name, bool defaultvalue )
{
    return (bool)ReadToken( node, name, token_Bool, NUMELE(token_Bool), defaultvalue );
}

int ConvertArray ( LPSTR value, collections::c_s32& c, LPCSTR delim )
{
    if ( value == NULL ) return 0;
    LPCSTR token = strtok( value, delim );
    while( token != NULL )   {
        c.Add( (s32)atol(token) ) ;
        token = strtok( NULL, delim );
    }
    return c.Count();
}

int ConvertArray ( LPSTR value, collections::c_float& c, LPCSTR delim )
{
    if ( value == nullptr) return 0;
    LPCSTR token = strtok( value, delim );
    while( token != nullptr)   {
        c.Add( (f32)atof(token) ) ;
        token = strtok( NULL, delim );
    }
    return c.Count();
}

int xml::ReadArray ( XmlNode* node, LPCSTR name, collections::c_s32& c, LPCSTR delim )
{
    c_str value = ReadStr( node, name );
    if ( value.IsNull() )
        return 0;
    return ConvertArray(value,c,delim);

}

int xml::ReadArray ( XmlNode* node, LPCSTR name, collections::c_s32& c )
{
    return ReadArray(node,name,c,",");
}

LPCSTR xml::ReadValue ( XmlNode* node, LPCSTR name )
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
    c_str s = ReadValue(node,name);
    return ConvertArray(s,c,",");
}

int xml::ReadValueArray ( XmlNode* node, LPCSTR name, collections::c_float& c )
{
    c_str s = ReadValue(node,name);
    return ConvertArray(s,c,",");
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

LPCSTR xml::ReadStrProperty ( XmlNode* node, LPCSTR name, LPCSTR defaultvalue )
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
