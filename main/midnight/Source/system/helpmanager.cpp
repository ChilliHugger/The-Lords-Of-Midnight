//
//  help.cpp
//  midnight
//
//  Created by Chris Wild on 11/12/2017.
//

#include "helpmanager.h"
#include "storymanager.h"
#include "moonring.h"

#include "../library/inc/mxtypes.h"

#include "help_ddr.cpp"
#include "help_lom.cpp"

static helpitem_t blank = { HELP_NONE, hf_none, nullptr };

helpmanager::helpmanager()
{
    displayed.clear();
    lookup.clear();
    for ( u32 ii=0; ii<NUMELE(help_messages); ii++ ) {
        auto item = &help_messages[ii];
        if (item->id != HELP_NONE) {
            lookup[item->id] = item;
        }
    }
}

helpmanager::~helpmanager()
{
}

const helpitem_t* helpmanager::GetItem( helpid_t id )
{
    auto item = lookup[id];
    if (item == nullptr) {
        return &blank;
    }
    
    return item;
}


std::string helpmanager::Get( helpid_t id )
{
    return GetItem( id )->text;
}

bool helpmanager::isShown ( helpid_t id )
{
    if ( displayed[ id ] )
        return true;
    
    if ( GetItem( id )->text == nullptr )
        return true;
    
    return false;
}

bool helpmanager::isImportant ( helpid_t id )
{
    return GetItem( id )->flags&hf_important;
}

bool helpmanager::isAlways ( helpid_t id )
{
    return GetItem( id )->flags&hf_always;
}

void helpmanager::Shown( helpid_t id )
{
    displayed[ id ] = true;
}

bool helpmanager::Save ( storyid_t id )
{
    auto filename = StringExtensions::Format("%s/help", mr->stories->getFolder(id).c_str() );

    chilli::os::file* pFile = new chilli::os::file ( filename.c_str(), chilli::os::file::modeReadWrite|chilli::os::file::modeCreate );
    if ( pFile == NULL || !pFile->IsOpen() ) {
        if ( pFile ) delete pFile;
        return false;
    }

    chilli::lib::archive ar (pFile, archive::store | archive::bNoFlushOnDelete );

    u32 version = HELP_VERSION;
    u32 count = (u32)displayed.size();

    ar << version ;
    ar << count ;
    
    for (const auto & [key, value] : displayed)
      ar << key;

    ar.Close();

    SAFEDELETE ( pFile );
    
    return true;
}

bool helpmanager::Load ( storyid_t id )
{
    displayed.clear();

    auto filename = StringExtensions::Format( "%s/help", mr->stories->getFolder(id).c_str() );

    chilli::os::file* pFile = new chilli::os::file ( filename.c_str(), chilli::os::file::modeRead );
    if ( !pFile->IsOpen() ) {
        if ( pFile ) delete pFile;
        return false;
    }

    archive ar (pFile, archive::load | archive::bNoFlushOnDelete);

    u32 version = HELP_VERSION;
    u32 count=0;

    ar >> version ;
    ar >> count ;
    
    if (version >= 1) {
        int key;
        for ( u32 ii=0; ii<count; ii++ ) {
            ar >> key;
            Shown((helpid_t)key);
        }
    } else{
        BOOL value;
        for ( u32 ii=0; ii<count; ii++ ) {
            ar >> value;
            if (value) {
                Shown((helpid_t)ii);
            }
        }
    }

    ar.Close();

    SAFEDELETE ( pFile );
    
    
    return true;
}
