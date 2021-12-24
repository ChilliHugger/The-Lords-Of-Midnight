#ifndef _XML_H_INCLUDED_
#define _XML_H_INCLUDED_

#include "../chilli.h"


#define TiXmlDocument tinyxml2::XMLDocument
#define TiXmlElement tinyxml2::XMLElement
#define TiXmlText tinyxml2::XMLText
#define XmlNode tinyxml2::XMLNode

namespace chilli  {

    namespace lib {

        class xml : public TiXmlDocument
        {
        public:

            xml();
            ~xml();

            bool Load ( LPCSTR filename );
            XmlNode* Find ( LPCSTR section );

            static XmlNode* Find ( XmlNode* node, LPCSTR section );
            static XmlNode* Find( XmlNode* node, LPCSTR element, LPCSTR id );

            static bool IsType( XmlNode* node, LPCSTR name) ;

            static LPCSTR ReadValue ( XmlNode* node, LPCSTR name );

            static int ReadValueArray ( XmlNode* node, LPCSTR name, collections::c_s32& c );
            static int ReadValueArray ( XmlNode* node, LPCSTR  name, collections::c_float& c );

            static int ReadIntProperty ( XmlNode* node, LPCSTR  name, int defaultvalue=0 );
            static bool ReadBoolProperty ( XmlNode* node, LPCSTR  name, bool defaultvalue=FALSE );
            static f32 ReadFloatProperty ( XmlNode* node, LPCSTR  name, f32 defaultvalue=0 );
            static LPCSTR ReadStrProperty ( XmlNode* node, LPCSTR  name, LPCSTR defaultvalue=NULL );

            static int ReadArray ( XmlNode* node, LPCSTR name, collections::c_s32& c );
            static int ReadArray ( XmlNode* node, LPCSTR name, collections::c_s32& c, char delim );

            static int ReadToken( XmlNode* node, LPCSTR token, token_t array[], int max, int defaultvalue=0 );
            static f32 ReadFloat ( XmlNode* node, LPCSTR name, f32 defaultvalue=0 );
            static bool ReadBool( XmlNode* node, LPCSTR name, bool defaultvalue=FALSE );
            static int ReadInt( XmlNode* node, LPCSTR name, int defaultvalue=0 );
            static LPCSTR ReadStr( XmlNode* node, LPCSTR name, LPCSTR defaultvalue = NULL );

            static LPCSTR ReadItemEx ( XmlNode* node, LPCSTR name );
            static LPCSTR ReadItem ( XmlNode* node, LPCSTR name, LPCSTR defaultvalue= NULL  );
            static int ReadItem ( XmlNode* node, LPCSTR name, int defaultvalue=0 );
            static f32 ReadItem (XmlNode* node,  LPCSTR name, f32 defaultvalue=0 );

            static bool Exists( XmlNode* node, LPCSTR name );
            static int Count (XmlNode* node);

            static LPCSTR ReadElement( XmlNode* node, LPCSTR element, LPCSTR id, LPCSTR tag );
            static point ReadPoint( XmlNode* node, LPCSTR name, point& po );
            static size ReadSize( XmlNode* node, LPCSTR name, size& s1 );

            static int ReadColour( XmlNode* node, LPCSTR name, int defaultvalue );
            
            static char ReadChar( XmlNode* node, LPCSTR name, char defaultvalue );

        };
    }
}

#define FOREACHELEMENT( parent,element ) \
    for( auto element = parent->FirstChildElement(); element; element = element->NextSiblingElement() )


#endif //_XML_H_INCLUDED_
