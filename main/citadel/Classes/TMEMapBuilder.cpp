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


tme::mxmap* TMEMapBuilderCitadel::Build( const std::string& tmxFile )
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
        
        //if (!layer->_visible)
        //    continue;

        
        if ( layer->_name == "Area" ) {
            for ( int ii=0; ii<totalSize; ii++ )
                map->m_data[ii].area =  layer->_tiles[ii] - areaGID ;
        }
        
        if ( layer->_name == "mountains (40,41)" ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }

        if ( layer->_name == "Trees (39)" ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }

        if ( layer->_name == "forest (37)" ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }
        
        if ( layer->_name == "foothills (44)" ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }
        
        if ( layer->_name == "hills (43)" ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }

        if ( layer->_name == "downs (42)" ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }

        if ( layer->_name == "border.txt" ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].flags |=  lf_mist;
            }
        }
        
        if ( layer->_name == "river (48)" ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }

        if ( layer->_name == "sea (47)" ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }
        
        if ( layer->_name == "lake (50)" ) {
            for ( int ii=0; ii<totalSize; ii++ ) {
                if ( layer->_tiles[ii] != 0 )
                    map->m_data[ii].terrain =  layer->_tiles[ii];
            }
        }
        
    }
    
    return map;
}
