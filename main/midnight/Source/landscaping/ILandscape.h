/*
 *  ILandscape.h
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 ___ORGANIZATIONNAME___. All rights reserved.
 *
 */

#pragma once

#include "LandscapeNode.h"

FORWARD_REFERENCE(LandscapeGenerator);
FORWARD_REFERENCE(LandscapeColour);
FORWARD_REFERENCE(SimpleShader);

class LandscapeOptions
{
    using ProgramState = ax::backend::ProgramState;
public:
    LandscapeOptions();
    virtual ~LandscapeOptions();
    
public:
    moonring*               mr;
    LandscapeGenerator*     generator;
    LandscapeColour*        colour;
    SimpleShader*           terrainTimeShader;
    SimpleShader*           characterTimeShader;
    bool                    showWater;
    bool                    showLand;
    bool                    showTerrain;
    bool                    showPeople;
    int                     debugMode;
    bool                    debugLand;
    f32                     landScaleX;
    f32                     landScaleY;
    tme::loc_t              here ;
    tme::loc_t              currentLocation;
    tme::loc_t              aheadLocation;
    mxdir_t                 currentDirection;
    bool                    isMoving;
    tme::loc_t              moveFrom;
    tme::loc_t              moveTo;
    f32                     movementAmount;
    bool                    moveLocationHasArmy;
    bool                    isLooking;
    f32                     lookAmount;
    f32                     lookOffsetAdjustment;
    bool                    isInTunnel;
    bool                    isLookingDownTunnel;
    bool                    isLookingOutTunnel;
    mxtime_t                timeofday;
    f32                     resScale;
    
};

class ILandscape : public LandscapeNode
{
public:
	ILandscape();
    virtual ~ILandscape() {};
        
protected:
    bool initWithOptions( LandscapeOptions* options );
};
