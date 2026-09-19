//
//  tsvdatabaseloader.h
//  midnight
//
//  Loads the entity database directly from the bundled .tsv source files,
//  populating the same in-memory collections that mxengine::LoadDatabase
//  otherwise fills from the pre-generated binary "database" file.
//

#ifndef _TSVDATABASELOADER_H_INCLUDED_
#define _TSVDATABASELOADER_H_INCLUDED_

#include <string>

namespace tme {

    class mxscenario;

    class TsvDatabaseLoader
    {
    public:
        // tsvDirectory must contain the scenario's *.tsv files (areainfo.tsv,
        // characters.tsv, ... variables.tsv). Populates mx's entity
        // collections, text and variables in place. Returns false if any
        // required file is missing/unreadable.
        bool Load ( mxscenario* scenario, const std::string& tsvDirectory );
    };

}
// namespace tme

#endif //_TSVDATABASELOADER_H_INCLUDED_
