//
//  TMEMapBuilder.h
//  citadel
//
//  Created by Chris Wild on 05/08/2017.
//
//

#ifndef TMEMapBuilder_h
#define TMEMapBuilder_h

#include <stdio.h>
#include "tme/baseinc/tme_internal.h"



class TMEMapBuilder
{
public:
    virtual mxmap* Build( const std::string& tmxFile );
    
    
};


class TMEMapBuilderCitadel : public TMEMapBuilder
{
public:
    mxmap* Build( const std::string& tmxFile );
    
    
};



#endif /* TMEMapBuilder_h */
