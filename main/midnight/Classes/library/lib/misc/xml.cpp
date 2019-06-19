#include "../../libinc/mxtypes.h"
#include "../../libinc/misc/xml.h"

#include <string>

#if !defined _MSC_VER
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-undefined-compare"
#endif

namespace chilli {
	namespace lib {

token_t token_Bool[] = {
	{ "yes",	true },
	{ "true",	true },
	{ "no",		false },
	{ "false",	false },
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

	//m_doc = new TiXmlDocument();
	Parse((const char*)data);

	SAFEDELETE ( data );

	return true ;
}

xml::node* xml::Find ( LPCSTR section )
{
	if ( this == nullptr)
		return nullptr;
	return (xml::node*)FirstChild(section)->ToElement();
}

xml::node* xml::node::Find ( LPCSTR section )
{
	if ( this == nullptr)
		return nullptr;
	return (xml::node*)FirstChild(section)->ToElement();
}

bool xml::node::IsType(LPCSTR name) const
{
	return c_stricmp(Value(),name) == 0;
}

int xml::node::Count ()
{
	if ( this == nullptr)
		return 0;

	int count=0;
	FOREACHELEMENT(this,e) {
		count++;
	}
	return count;
}

xml::node* xml::node::Find( LPCSTR element, LPCSTR id )
{
	FOREACHELEMENT(this,e){
		if ( e->IsType(element) ) {
			if ( c_stricmp(e->ReadStr("id"), id) == 0 ) {
				return e;
			}
		}
	}
	return NULL ;
}

LPCSTR xml::node::ReadElement( LPCSTR element, LPCSTR id, LPCSTR tag )
{
	FOREACHELEMENT(this,e){
		if ( e->IsType(element) ) {
			if (c_stricmp(e->ReadStr("id"), id) == 0 ) {
				return e->ReadStr(tag);
			}
		}
	}
	return strNull ;
}

LPCSTR xml::node::ReadItemEx ( LPCSTR name )
{
	if ( this == nullptr)
		return nullptr;

	// read an attribute first
	LPCSTR value = this->Attribute(name);

	// try this
	if ( value == nullptr) {
		if ( IsType(name) ) {
			TiXmlText* m1 = FirstChild()->ToText();
			if ( m1 )
				value = m1->Value();
		}
	}

	// try element
	if ( value == nullptr) {
		FOREACHELEMENT(this,e){
			if ( e->IsType(name) ) {
				TiXmlText* m1 = e->FirstChild()->ToText();
				if ( m1 )
					value = m1->Value();
				break;
			}
		}
	}

	return value ;
}

LPCSTR xml::node::ReadItem ( LPCSTR name, LPCSTR defaultvalue )
{
	LPCSTR value = ReadItemEx(name);
	if ( value == nullptr)
		return defaultvalue;
	return value;
}

LPCSTR  xml::node::ReadStr( LPCSTR name, LPCSTR defaultvalue )
{
    return ReadItem ( name, defaultvalue );
}

char  xml::node::ReadChar( LPCSTR name, char defaultvalue )
{
    std::string item = ReadItem ( name, "" );
    if ( item.empty() ) {
        return defaultvalue;
    }
    return item[0];
}

f32 xml::node::ReadItem ( LPCSTR name, f32 defaultvalue )
{
	LPCSTR value = ReadItemEx(name);
	if ( value == nullptr)
		return defaultvalue;
	return (f32) atof ( value );
}

	f32 xml::node::ReadFloat ( LPCSTR name, f32 defaultvalue )
	{
		return ReadItem(name,defaultvalue) ;
	}

int xml::node::ReadItem ( LPCSTR name, int defaultvalue )
{
	LPCSTR value = ReadItemEx(name);
	if ( value == nullptr)
		return defaultvalue;
	return atoi ( value );
}

	int xml::node::ReadInt( LPCSTR name, int defaultvalue )
	{
		return ReadItem(name,defaultvalue) ;
	}


bool xml::node::Exists( LPCSTR name )
{
	if ( this == NULL )
		return FALSE;

	if ( this->Attribute(name) == nullptr)
		return FALSE;

	return TRUE;
}

//int xml::node::ReadValue( LPCSTR name, int defaultvalue )
//{
//	xml::node* newelement = (xml::node*)this->FirstChildElement(name);
//	return newelement->ReadItem("value",defaultvalue);
//}



size xml::node::ReadSize( LPCSTR name, size& s1 )
{
	size s;
	collections::c_s32 c;

	s.cx = s1.cx;
	s.cy = s1.cy;

	if ( ReadArray ( name, c ) ) {
		s.cx = c[0];
		s.cy = c[1];
	}
	return s;
}
point xml::node::ReadPoint( LPCSTR name, point& p1 )
{
	point p;
	collections::c_s32 c;

	p.x = p1.x;
	p.y = p1.y;

	if ( ReadArray ( name, c ) ) {
		p.x = c[0];
		p.y = c[1];
	}
	return p;
}

int xml::node::ReadColour( LPCSTR name, int defaultvalue )
{
int value=0;

LPCSTR	val = ReadStr(name);
	if ( val == NULL )
		return defaultvalue;

	if ( val[0] == '#' ) {
		sscanf(&val[1],"%x",&value);
		return value;
	}

	// find a global colour variable
	//if ( val[0] == '@' ) {
	//	var_t* var = FindGlobalVariable ( &val[1] );
	//	if ( var ) {
	//		if ( var->type == V_RGB ) {
	//			CRgb* rgb = (CRgb*)var->ptr ;
	//			return rgb->rgba;
	//		}
	//		if ( var->type == V_INT ) {
	//			int* integer = (int*)var->ptr ;
	//			return *integer;
	//		}
	//	}
	//	return 0;
	//}

	return atoi(val) ;
}


int xml::node::ReadToken( LPCSTR token, token_t array[], int max, int defaultvalue )
{
	//if ( !Exists(token) )
	//	return defaultvalue;

	LPCSTR 	val = ReadStr(token, nullptr);
	if ( val == nullptr)
		return defaultvalue;

	int temp =  GetToken ( val, array, max );
	if ( temp ) return temp ;
	return atoi(val);
}

bool xml::node::ReadBool( LPCSTR name, bool defaultvalue )
{
	return (bool)ReadToken( name, token_Bool, NUMELE(token_Bool), defaultvalue );
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

int xml::node::ReadArray ( LPCSTR name, collections::c_s32& c, LPCSTR delim )
{
	//if ( IsType(name) ) {
	//	TiXmlText* m1 = FirstChild()->ToText();
	//	string value = m1->Value();
	//	if ( value.IsNull() )
	//		return 0;
	//	return ConvertArray(value,c,delim);
	//}


	c_str value = ReadStr( name );
	if ( value.IsNull() )
		return 0;
	return ConvertArray(value,c,delim);

}

int xml::node::ReadArray ( LPCSTR  name, collections::c_s32& c )
{
	return ReadArray(name,c,",");
}

LPCSTR xml::node::ReadValue ( LPCSTR name )
{
	xml::node* m = Find(name);
	if ( m ) {
		TiXmlText* m1 = m->FirstChild()->ToText();
		if ( m1 )
			return m1->Value();
	}
	return nullptr;

}

int xml::node::ReadValueArray ( LPCSTR  name, collections::c_s32& c )
{
	c_str s = ReadValue(name);
	return ConvertArray(s,c,",");
}

int xml::node::ReadValueArray ( LPCSTR  name, collections::c_float& c )
{
	c_str s = ReadValue(name);
	return ConvertArray(s,c,",");
}

int xml::node::ReadIntProperty ( LPCSTR  name, int defaultvalue )
{
	xml::node* e = nullptr;
	if ( (e = Find( "integer", name )) ) {
		return e->ReadInt("value",defaultvalue);
	}
	return defaultvalue ;
}

bool xml::node::ReadBoolProperty ( LPCSTR  name, bool defaultvalue )
{
	xml::node* e = nullptr;
	if ( (e = Find( "bool", name )) ) {
		return e->ReadBool("value",defaultvalue);
	}
	return defaultvalue ;
}

LPCSTR xml::node::ReadStrProperty ( LPCSTR  name, LPCSTR defaultvalue )
{
	xml::node* e = nullptr;
	if ( (e = Find( "string", name )) ) {
		return e->ReadStr("value",defaultvalue);
	}
	return defaultvalue ;
}

f32 xml::node::ReadFloatProperty ( LPCSTR  name, f32 defaultvalue )
{
	xml::node* e = nullptr;
	if ( (e = Find( "float", name )) ) {
		return e->ReadFloat("value",defaultvalue);
	}
	return defaultvalue ;
}


#if !defined _MSC_VER
#pragma clang diagnostic pop
#endif
        
//
//CDate xml::ReadDate ( TiXmlElement* element, LPCSTR name )
//{
//CCollection collection;
//	ReadXml_Array(element,name,collection,"/");
//	return CDate( collection[2],collection[1], collection[0] );
//}

	} // namespace lib
} // namespace tme
