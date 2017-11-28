//
//  TMEMapBuilder.h
//  citadel
//
//  Created by Chris Wild on 05/08/2017.
//
//

#ifndef TMEMapBuilder_h
#define TMEMapBuilder_h

#include <string>

namespace tme {
	class mxmap;
}


class TMEMapBuilder
{
public:
    virtual tme::mxmap* Build( const std::string& tmxFile );
    
    
};


class TMEMapBuilderCitadel : public TMEMapBuilder
{
public:
    tme::mxmap* Build( const std::string& tmxFile );
    
    
};



#endif /* TMEMapBuilder_h */
