//
//  help.cpp
//  midnight
//
//  Created by Chris Wild on 11/12/2017.
//

#include "helpmanager.h"
#include "storymanager.h"
#include "moonring.h"

#include "../library/libinc/mxtypes.h"

#include "help_ddr.cpp"
#include "help_lom.cpp"

helpmanager::helpmanager( moonring* mr )
{
    CLEARARRAY(displayed);
    this->mr = mr;
}

LPCSTR helpmanager::Get( helpid_t id )
{
    return help_messages[ id ].text;
}

BOOL helpmanager::isShown ( helpid_t id )
{
    if ( displayed[ id ] )
        return TRUE;
    
    if ( help_messages[ id ].text == NULL )
        return TRUE;
    
    return FALSE;
}

BOOL helpmanager::isImportant ( helpid_t id )
{
    return help_messages[ id ].flags&hf_important;
}

BOOL helpmanager::isAlways ( helpid_t id )
{
    return help_messages[ id ].flags&hf_always;
}

void helpmanager::Shown( helpid_t id )
{
    displayed[ id ]=TRUE;
}

BOOL helpmanager::Save ( storyid_t id )
{
    char filename[MAX_PATH]={0};
    sprintf(filename, "%s/help", mr->stories->getFolder(id) );
    
    chilli::os::file* pFile = new chilli::os::file ( filename, chilli::os::file::modeReadWrite|chilli::os::file::modeCreate );
    if ( pFile == NULL || !pFile->IsOpen() ) {
        if ( pFile ) delete pFile;
        return FALSE;
    }
    
    chilli::lib::archive ar (pFile, archive::store | archive::bNoFlushOnDelete );
    
    u32 version=1;
    u32 count=HELP_MAX;
    
    ar << version ;
    ar << count ;
    for ( u32 ii=0; ii<HELP_MAX; ii++ )
        ar << displayed[ ii ];
    
    ar.Close();
    
    SAFEDELETE ( pFile );
    
    return TRUE;
}

BOOL helpmanager::Load ( storyid_t id )
{
    
    CLEARARRAY(displayed);
    
    char filename[MAX_PATH]={0};
    sprintf(filename, "%s/help", mr->stories->getFolder(id) );
    
    
    chilli::os::file* pFile = new chilli::os::file ( filename, chilli::os::file::modeRead );
    if ( !pFile->IsOpen() ) {
        if ( pFile ) delete pFile;
        return FALSE;
    }
    
    
    archive ar (pFile, archive::load | archive::bNoFlushOnDelete);
    
    u32 version=1;
    u32 count=0;
    
    ar >> version ;
    ar >> count ;
    for ( u32 ii=0; ii<count; ii++ )
        ar >> displayed[ ii ];
    
    ar.Close();
    
    SAFEDELETE ( pFile );
    
    
    return TRUE;
}
