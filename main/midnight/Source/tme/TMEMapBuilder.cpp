//
//  TMEMapBuilder.cpp
//  citadel
//
//  Created by Chris Wild on 05/08/2017.
//
//
#include "TMEMapBuilder.h"
#include "baseinc/tme_internal.h"
#include "../tinyxml2/tinyxml2.h"

USING_NS_AX;
USING_NS_TME;

// axmol's TMX parser doesn't expose the Tiled "class" attribute on <layer>
// elements, so read it ourselves, in document order, to line up with
// TMXMapInfo::getLayers() (which preserves document order).
std::vector<std::string> TMEMapBuilder::GetLayerClasses( const std::string& tmxFile )
{
    std::vector<std::string> classes;

    tinyxml2::XMLDocument doc;
    if ( doc.LoadFile( tmxFile.c_str() ) != tinyxml2::XML_SUCCESS )
        return classes;

    auto* map = doc.RootElement();
    if ( map == nullptr )
        return classes;

    for ( auto* layer = map->FirstChildElement("layer"); layer != nullptr; layer = layer->NextSiblingElement("layer") ) {
        const char* layerClass = layer->Attribute("class");
        const char* layerName  = nullptr; //layer->Attribute("name");
        classes.push_back( layerClass != nullptr ? layerClass : ( layerName != nullptr ? layerName : "" ) );
    }

    return classes;
}


// decode each non-zero tile against the shared "flags" tileset and set the
// corresponding mxloc/mxmap flag - mirrors TMXMapExporter's CheckFlags()
void TMEMapBuilder::CheckFlags( tme::mxmap* map, const TMXLayerInfo* layer, int flagsGID, int totalSize )
{
    for ( int loc=0; loc<totalSize; loc++ ) {

        uint32_t tile = layer->_tiles[loc];
        if ( tile == 0 )
            continue;

        switch ( (TmxLocationFlag)(tile - flagsGID) ) {
            case tlf_domain:
                map->m_data[loc].flags |= lf_domain;
                break;
            case tlf_tunnel:
                map->m_data[loc].flags |= lf_tunnel;
                break;
            case tlf_mist:
                map->m_data[loc].flags |= lf_mist;
                break;
            case tlf_creature:
                map->m_data[loc].flags |= lf_creature;
                break;
            case tlf_impassable:
                map->m_data[loc].flags |= lf_impassable;
                map->m_flags.Set(mf_impassablelocations);
                break;
            case tlf_respawn:
                map->m_data[loc].flags |= lf_respawn;
                break;
            case tlf_tunnelentrance:
                map->m_data[loc].flags |= lf_tunnel_entrance | lf_tunnel;
                map->m_flags.Set(mf_tunnnelendpoints);
                break;
            case tlf_tunnelexit:
                map->m_data[loc].flags |= lf_tunnel_exit | lf_tunnel;
                map->m_flags.Set(mf_tunnnelendpoints);
                break;
            case tlf_tunnelentranceexit:
                map->m_data[loc].flags |= lf_tunnel_entrance | lf_tunnel_exit | lf_tunnel;
                map->m_flags.Set(mf_tunnnelendpoints);
                break;
            case tlf_tunnelsmall:
                map->m_data[loc].flags |= lf_tunnel_small | lf_tunnel;
                break;
            case tlf_path:
            case tlf_unused1:
            case tlf_unused2:
            case tlf_unused3:
                break;
        }
    }
}

tme::mxmap* TMEMapBuilder::Build( const std::string& tmxFile )
{
    // Bypass TMXMapInfo::create() deliberately: create() autoreleases the
    // returned object, which messes with this function being called on a background thread.
    std::unique_ptr<TMXMapInfo> mapInfo(new TMXMapInfo());
    
    if ( !mapInfo->initWithTMXFile(tmxFile) ) {
        return nullptr;
    }

    if ( mapInfo->getLayers().empty() ) {
        MXTRACE("TMX Map '%s' parsed with no layers - axmol TMX parser likely failed silently, aborting build", tmxFile.c_str());
        return nullptr;
    }

    int totalSize = mapInfo->getMapSize().width*mapInfo->getMapSize().height ;
    int areaGID = 0;
    int thingsGID = 0;
    int flagsGID = 0;

    // setup GID
    for (const auto &tileset : mapInfo->getTilesets() ) {
        if ( tileset->_name.compare("areas") == 0 )
            areaGID = tileset->_firstGid ;
        if ( tileset->_name.compare("things") == 0 )
            thingsGID = tileset->_firstGid ;
        if ( tileset->_name.compare("flags") == 0 )
            flagsGID = tileset->_firstGid ;
    }

    auto map = new tme::mxmap( mapInfo->getMapSize().width, mapInfo->getMapSize().height );

    auto classes = GetLayerClasses(tmxFile);
    auto& layers = mapInfo->getLayers();

    MXTRACE("TMX Map: width=%d, height=%d size=%d layers=%d",
        map->m_size.cx,
        map->m_size.cy,
        totalSize,
        layers.size()
    );
    
    MXTRACE("areaGID = %d", areaGID);
    MXTRACE("thingsGID = %d", thingsGID);
    MXTRACE("flagsGID = %d", flagsGID);

    for ( size_t ii=0; ii<layers.size(); ii++ ) {

        const auto& layer = layers[ii];
        if ( layer->_tiles == nullptr ) {
            MXTRACE("Layer %d:'%s' has no tile data (_tiles is null) - skipping corrupted layer", ii, layer->_name.c_str());
            continue;
        }

        const std::string layerClass = ii < classes.size() ? classes[ii] : "";

        MXTRACE("Checking layer %d:'%s' : type = '%s'", ii, layer->_name.c_str(), layerClass.c_str() );

        int layerTileCount = (int)(layer->_layerSize.width * layer->_layerSize.height);
        if ( layerTileCount < totalSize ) {
            MXTRACE("Layer %d:'%s' tile buffer (%d) is smaller than map size (%d) - skipping corrupted layer",
                ii, layer->_name.c_str(), layerTileCount, totalSize);
            continue;
        }

        if ( layerClass.compare("Terrain") == 0 ) {
            MXTRACE("Parsing layer '%s' : type = '%s'", layer->_name.c_str(), layerClass.c_str() );
            for ( int loc=0; loc<totalSize; loc++ ) {
                if (layer->_tiles[loc] != 0 ) {
                    auto t = layer->_tiles[loc];
                    auto tn = (mxterrain_t)layer->_tiles[loc];
                    map->m_data[loc].terrain =  (mxterrain_t)layer->_tiles[loc];
                }
            }
        }
        else if ( layerClass.compare("Area") == 0 ) {
            MXTRACE("Parsing layer '%s' : type = '%s'", layer->_name.c_str(), layerClass.c_str() );
            for ( int loc=0; loc<totalSize; loc++ )
                if (layer->_tiles[loc] != 0 ) {
                    auto t = layer->_tiles[loc];
                    auto ar = (t - areaGID + 1);
                    map->m_data[loc].area = ar;
                }
        }
        else if ( layerClass.compare("Things") == 0 ) {
            MXTRACE("Parsing layer '%s' : type = '%s'", layer->_name.c_str(), layerClass.c_str() );
            for ( int loc=0; loc<totalSize; loc++ ) {
                if (layer->_tiles[loc] != 0 ) {
                    auto t = layer->_tiles[loc];
                    auto tg = (mxthing_t)(t - thingsGID + 1);
                    map->m_data[loc].object = tg;
                }
            }
        }
        else if ( layerClass.compare("Flags") == 0 ) {
            MXTRACE("Parsing layer '%s' : type = '%s'", layer->_name.c_str(), layerClass.c_str() );
            CheckFlags( map, layer, flagsGID, totalSize );
        } else {
            MXTRACE("Unknown layer '%s' : type = '%s'", layer->_name.c_str(), layerClass.c_str() );
        }
    }

    return map;
}
