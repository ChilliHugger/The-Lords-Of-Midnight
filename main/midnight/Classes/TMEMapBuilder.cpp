//
//  TMEMapBuilder.cpp
//  citadel
//
//  Created by Chris Wild on 05/08/2017.
//
//

#include "2d/CCTMXTiledMap.h"
#include "2d/CCTMXXMLParser.h"
#include "2d/CCTMXLayer.h"

#include "TMEMapBuilder.h"
#include "tme/baseinc/tme_internal.h"

USING_NS_CC;
using namespace tme;

tme::mxmap* TMEMapBuilder::Build( const std::string& tmxFile )
{
    TMXMapInfo *mapInfo = TMXMapInfo::create(tmxFile);
    if ( mapInfo == nullptr )
        return nullptr;
    
    int totalSize = mapInfo->getMapSize().width*mapInfo->getMapSize().height ;
    int areaGID = 0;
    
    // setup GID
    for (const auto &tileset : mapInfo->getTilesets() ) {
        if ( tileset->_name.compare("areas") == 0 )
            areaGID = tileset->_firstGid ;
    }
    
    mxmap* map = new tme::mxmap( mapInfo->getMapSize().width, mapInfo->getMapSize().height );
    
    for (const auto &layer : mapInfo->getLayers()) {
        
        if ( layer->_name.compare("Area") == 0 ) {
            for ( int ii=0; ii<totalSize; ii++ )
                map->m_data[ii].area =  layer->_tiles[ii] - areaGID ;
        }
        
        if ( layer->_name.compare("Terrain") == 0 ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }
    }
    
    return map;
}


tme::mxmap* TMEMapBuilderCitadel::Build( const std::string& tmxFile )
{
    TMXMapInfo *mapInfo = TMXMapInfo::create(tmxFile);
    if ( mapInfo == nullptr )
        return nullptr;
    
    int totalSize = mapInfo->getMapSize().width*mapInfo->getMapSize().height ;
    int areaGID = 0;
    
    // setup GID
    for (const auto &tileset : mapInfo->getTilesets() ) {
        if ( tileset->_name.compare("areas") == 0 )
            areaGID = tileset->_firstGid ;
    }
    
    mxmap* map = new tme::mxmap( mapInfo->getMapSize().width, mapInfo->getMapSize().height );
    
    for (const auto &layer : mapInfo->getLayers()) {
        
        //if (!layer->_visible)
        //    continue;

        
        if ( layer->_name.compare("Area") == 0 ) {
            for ( int ii=0; ii<totalSize; ii++ )
                map->m_data[ii].area =  layer->_tiles[ii] - areaGID ;
        }
        
        if ( layer->_name.compare("mountains (40,41)") == 0 ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }

        if ( layer->_name.compare("Trees (39)") == 0 ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }

        if ( layer->_name.compare("forest (37)") == 0 ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }
        
        if ( layer->_name.compare("foothills (44)") == 0 ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }
        
        if ( layer->_name.compare("hills (43)") == 0 ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }

        if ( layer->_name.compare("downs (42)") == 0 ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }

        if ( layer->_name.compare("border.txt") == 0 ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].flags |=  lf_mist;
            }
        }
        
        if ( layer->_name.compare("river (48)") == 0 ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }

        if ( layer->_name.compare("sea (47)") == 0 ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }
        
        if ( layer->_name.compare("lake (50)") == 0 ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }
        
    }
    
    return map;
}
