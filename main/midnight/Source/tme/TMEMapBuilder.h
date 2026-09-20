//
//  TMEMapBuilder.h
//  citadel
//
//  Created by Chris Wild on 05/08/2017.
//
//

#ifndef TMEMapBuilder_h
#define TMEMapBuilder_h

#include "2d/TileMapAtlas.h"
#include "2d/TMXXMLParser.h"
#include <string>

namespace tme {
    class mxmap;
}

// tile values found in a flag layer, relative to the "flags" tileset's
// firstgid - matches the shared tile ordering produced by TMXMapExporter
enum TmxLocationFlag {
    tlf_domain = 0,
    tlf_tunnel,
    tlf_mist,
    tlf_creature,
    tlf_unused1,
    tlf_unused2,
    tlf_unused3,
    tlf_path,
    tlf_impassable,
    tlf_respawn,
    tlf_tunnelentrance,
    tlf_tunnelexit,
    tlf_tunnelentranceexit,
    tlf_tunnelsmall,
};

class TMEMapBuilder
{
public:
    virtual tme::mxmap* Build( const std::string& tmxFile );

    std::vector<std::string> GetLayerClasses( const std::string& tmxFile );
    void CheckFlags( tme::mxmap* map, const ax::TMXLayerInfo* layer, int flagsGID, int totalSize );

};



#endif /* TMEMapBuilder_h */
