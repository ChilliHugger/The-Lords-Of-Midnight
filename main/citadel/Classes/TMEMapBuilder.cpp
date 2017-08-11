//
//  TMEMapBuilder.cpp
//  citadel
//
//  Created by Chris Wild on 05/08/2017.
//
//

#include "TMEMapBuilder.h"

#include "2d/CCTMXTiledMap.h"
#include "2d/CCTMXXMLParser.h"
#include "2d/CCTMXLayer.h"

USING_NS_CC;



tme::mxmap* TMEMapBuilder::Build( const std::string& tmxFile )
{
    TMXMapInfo *mapInfo = TMXMapInfo::create(tmxFile);
    if ( mapInfo == nullptr )
        return nullptr;
    
    int totalSize = mapInfo->getMapSize().width*mapInfo->getMapSize().height ;
    int areaGID = 0;
    
    // setup GID
    for (const auto &tileset : mapInfo->getTilesets() ) {
        if ( tileset->_name == "areas" )
            areaGID = tileset->_firstGid ;
    }
    
    mxmap* map = new tme::mxmap( mapInfo->getMapSize().width, mapInfo->getMapSize().height );
    
    for (const auto &layer : mapInfo->getLayers()) {
        
//        if (!layer->_visible)
//            continue;
//        
        
        if ( layer->_name == "Area" ) {
            for ( int ii=0; ii<totalSize; ii++ )
                map->m_data[ii].area =  layer->_tiles[ii] - areaGID ;
        }
        
        if ( layer->_name == "Terrain" ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }
    }
    
    return map;
}


