
/*
 *  ILandscape.cpp
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#include "ILandscape.h"


ILandscape::ILandscape()
{
}

bool ILandscape::initWithOptions( LandscapeOptions* options )
{
    return LandscapeNode::initWithOptions(options) ;
}

LandscapeOptions::LandscapeOptions() :
    mr(nullptr),
    generator(nullptr),
    colour(nullptr),
    terrainTimeShader(nullptr),
    characterTimeShader(nullptr)
{
}

LandscapeOptions::~LandscapeOptions()
{
}
