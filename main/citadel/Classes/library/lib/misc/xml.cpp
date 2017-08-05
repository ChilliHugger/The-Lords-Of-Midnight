#include "../../libinc/library.h"

#if !defined _MSC_VER
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-undefined-compare"
#endif

namespace chilli {
	namespace lib {

token_t token_Bool[] = {
	{ "yes",	TRUE },
	{ "true",	TRUE },
	{ "no",		FALSE },
	{ "false",	FALSE },
	};


xml::xml()
{
}

xml::~xml()
{
}

BOOL xml::Load ( LPCSTR filename )
{
u8* data = NULL ;
u32 size=0;


    data = (u8*)os::filemanager::Load ( filename, &size );
	if ( data == NULL )
		return FALSE;

	//m_doc = new TiXmlDocument();
	Parse((const char*)data);

	SAFEDELETE ( data );

	return TRUE ;
}

xml::node* xml::Find ( LPCSTR section )
{
	if ( this == NULL )
		return NULL;
	return (xml::node*)FirstChild(section)->ToElement();
}

xml::node* xml::node::Find ( LPCSTR section )
{
	if ( this == NULL )
		return NULL;
	return (xml::node*)FirstChild(section)->ToElement();
}

BOOL xml::node::IsType(LPCSTR name) const
{
	return lib::stricmp(Value(),name) == 0;
}

int xml::node::Count ()
{
	if ( this == NULL )
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
			if ( lib::stricmp(e->ReadStr("id"), id) == 0 ) {
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
			if ( lib::stricmp(e->ReadStr("id"), id) == 0 ) {
				return e->ReadStr(tag);
			}
		}
	}
	return strNull ;
}

LPCSTR xml::node::ReadItemEx ( LPCSTR name )
{
	if ( this == NULL )
		return NULL;

	// read an attribute first
	LPCSTR value = this->Attribute(name);

	// try this
	if ( value == NULL ) {
		if ( IsType(name) ) {
			TiXmlText* m1 = FirstChild()->ToText();
			if ( m1 )
				value = m1->Value();
		}
	}

	// try element
	if ( value == NULL ) {
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
	if ( value == NULL )
		return defaultvalue;
	return value;
}

	LPCSTR  xml::node::ReadStr( LPCSTR name, LPCSTR defaultvalue )
	{
		return ReadItem ( name, defaultvalue );
	}

f32 xml::node::ReadItem ( LPCSTR name, f32 defaultvalue )
{
	LPCSTR value = ReadItemEx(name);
	if ( value == NULL )
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
	if ( value == NULL )
		return defaultvalue;
	return atoi ( value );
}

	int xml::node::ReadInt( LPCSTR name, int defaultvalue )
	{
		return ReadItem(name,defaultvalue) ;
	}


BOOL xml::node::Exists( LPCSTR name )
{
	if ( this == NULL )
		return FALSE;

	if ( this->Attribute(name) == NULL )
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

	LPCSTR 	val = ReadStr(token,NULL);
	if ( val == NULL )
		return defaultvalue;

	int temp =  GetToken ( val, array, max );
	if ( temp ) return temp ;
	return atoi(val);
}

BOOL xml::node::ReadBool( LPCSTR name, BOOL defaultvalue )
{
	return (BOOL)ReadToken( name, token_Bool, NUMELE(token_Bool), defaultvalue );
}

int ConvertArray ( LPSTR value, collections::c_s32& c, LPCSTR delim )
{
	if ( value == NULL ) return 0;
	LPCSTR token = strtok( value, delim );
	while( token != NULL )   {
		c.Add( atol(token) ) ;
		token = strtok( NULL, delim );
	}
	return c.Count();
}

int ConvertArray ( LPSTR value, collections::c_float& c, LPCSTR delim )
{
	if ( value == NULL ) return 0;
	LPCSTR token = strtok( value, delim );
	while( token != NULL )   {
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


	string value = ReadStr( name );
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
	return NULL ;

}

int xml::node::ReadValueArray ( LPCSTR  name, collections::c_s32& c )
{
	string s = ReadValue(name);
	return ConvertArray(s,c,",");
}

int xml::node::ReadValueArray ( LPCSTR  name, collections::c_float& c )
{
	string s = ReadValue(name);
	return ConvertArray(s,c,",");
}

int xml::node::ReadIntProperty ( LPCSTR  name, int defaultvalue )
{
	xml::node* e = NULL ;
	if ( (e = Find( "integer", name )) ) {
		return e->ReadInt("value",defaultvalue);
	}
	return defaultvalue ;
}

BOOL xml::node::ReadBoolProperty ( LPCSTR  name, BOOL defaultvalue )
{
	xml::node* e = NULL ;
	if ( (e = Find( "bool", name )) ) {
		return e->ReadBool("value",defaultvalue);
	}
	return defaultvalue ;
}

LPCSTR xml::node::ReadStrProperty ( LPCSTR  name, LPCSTR defaultvalue )
{
	xml::node* e = NULL;
	if ( (e = Find( "string", name )) ) {
		return e->ReadStr("value",defaultvalue);
	}
	return defaultvalue ;
}

f32 xml::node::ReadFloatProperty ( LPCSTR  name, f32 defaultvalue )
{
	xml::node* e = NULL;
	if ( (e = Find( "float", name )) ) {
		return e->ReadFloat("value",defaultvalue);
	}
	return defaultvalue ;
}



//
//CDate xml::ReadDate ( TiXmlElement* element, LPCSTR name )
//{
//CCollection collection;
//	ReadXml_Array(element,name,collection,"/");
//	return CDate( collection[2],collection[1], collection[0] );
//}

	} // namespace lib
} // namespace tme
