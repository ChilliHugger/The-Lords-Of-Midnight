/*
 *  mxengine_map_binary.cpp
 *  tme
 *
 *  Created by Chris Wild on 02/02/2011.
 *  Copyright 2011 Chilli Hugger. All rights reserved.
 *
 */

#include "../baseinc/tme_internal.h"
#include "axmol.h"

USING_NS_TME ;

/*
 * Function name    : mxengine::LoadMapBinary
 *
 * Return type        : MXRESULT
 *
 * Arguments        : const std::string& filename
 */

MXRESULT mxengine::LoadMapBinary ( const std::string& filename )
{
std::string loadFilename = filename;

#if !defined(_OS_DESKTOP_)
{
    std::string scenarioIdStr = std::to_string((int)scenario->GetInfoBlock()->Id);
    auto database = loadFilename;
    loadFilename = ax::FileUtils::getInstance()->getWritablePath() + "/map_" + scenarioIdStr;
    MXTRACE( "Copying Map 'map' from '%s' to '%s'",
        database.c_str(),
        loadFilename.c_str());

    chilli::os::filemanager::Copy(database.c_str(), loadFilename.c_str());
}
#endif

    gamemap = new mxmap() ;

    if ( !gamemap->Load ( loadFilename ) ) {
        SAFEDELETE ( gamemap );
        return MX_FAILED;
    }

    return MX_OK ;
}
