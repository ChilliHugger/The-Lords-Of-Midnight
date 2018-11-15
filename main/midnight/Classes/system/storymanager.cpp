//
//  storymanager.cpp
//  midnight
//
//  Created by Chris Wild on 15/12/2017.
//
//

#include "storymanager.h"
#include "configmanager.h"
#include "../tme_interface.h"
#include "moonring.h"
#include "../ui/uihelper.h"

using namespace chilli::os;
using namespace chilli::lib;

static rgb_t bookmark_colours[] = {
    rgb_t(0xde, 0x00, 0x00),
    rgb_t(0xde, 0x7e, 0x00),
    rgb_t(0xcc, 0xcc, 0x00),
    rgb_t(0x69, 0xa9, 0x00),
    rgb_t(0x00, 0xb2, 0xa4),
    rgb_t(0x00, 0x7e, 0xcc),
    rgb_t(0x84, 0x00, 0xdb),
    rgb_t(0x96, 0x96, 0x96)
};



storymanager::storymanager()
{
    CLEARARRAY(used);
    current=0;
    last_save=0;
    
    last_night.Clear();
    last_morning.Clear();
    
    loadsave = nullptr;
}


storymanager::~storymanager()
{
}

storyinfo_t* storymanager::getStoriesInfo()
{
    static storyinfo_t stories ;
    
    int used = 0;
    
    for ( u32 ii=0; ii<MAX_STORIES; ii++ ) {
    
        if ( filemanager::Exists( getPath(ii+1) ) ) {
            getDescription(ii+1, stories.chapter[used].description);
            stories.chapter[used].id = ii + 1;
            stories.chapter[used].color = bookmark_colours[used];
            used++;
        }
    
    
    }
    
    stories.count = used;
    
    return &stories;
}


void storymanager::checkStories( void )
{
    for ( u32 ii=0; ii<MAX_STORIES; ii++ ) {
        used[ii] = filemanager::Exists( getPath(ii+1) ) ;
    }
}

int storymanager::stories_used()
{
    u32 count=0;
    checkStories();
    for ( u32 ii=0; ii<MAX_STORIES; ii++ ) {
        if ( used[ii])
            count++;
    }
    return count;
    
}

storyid_t storymanager::next_free_story()
{
    checkStories();
    for ( u32 ii=0; ii<MAX_STORIES; ii++ ) {
        if ( !used[ii])
            return ii+1;
    }
    return STORY_NONE;
}

storyid_t storymanager::first_used_story()
{
    checkStories();
    for ( u32 ii=0; ii<MAX_STORIES; ii++ ) {
        if ( used[ii])
            return ii+1;
    }
    return STORY_NONE;
}

storyid_t storymanager::alloc( void )
{
    storyid_t id = next_free_story();
    used[id-1]=TRUE;
    return id;
}


void storymanager::SetLoadSave( tme::PFNSERIALIZE function )
{
    loadsave = function;
}



LPCSTR storymanager::DiscoveryMapFilename()
{
    static char filename[MAX_PATH]={0};
    

    sprintf(filename, "%s/story/discovery.map", mr->getWritablePath());
    
    return filename;
    
}

BOOL storymanager::create ( storyid_t id )
{
    TME_DeInit();
    TME_Init();
    current=id;
    last_save=0;
    last_night.Clear();
    last_morning.Clear();
    
    cocos2d::FileUtils::getInstance()->createDirectory(getFolder(id));
    
    TME_LoadDiscoveryMap( (char*)DiscoveryMapFilename() );
    
    return save(id,savemode_dawn);
}

LPCSTR storymanager::getFolder ( storyid_t id )
{
    static char folder[MAX_PATH]={0};
    
    sprintf(folder, "%s/story/%d", mr->getWritablePath(), (int)id);

    return folder;
    
}


LPCSTR storymanager::getPath( storyid_t id )
{
    static char filename[MAX_PATH]={0};
    
    sprintf(filename, "%s/current", getFolder(id) );
    
    return filename;
    
}

LPCSTR storymanager::getPath(storyid_t id, u32 save)
{
    static char filename[MAX_PATH]={0};
    
    sprintf(filename, "%s/%03d", getFolder(id), (int)save );
    
    return filename;
    
}

#if defined(_DEBUG_EMAIL_)
BOOL storymanager::debug_email ( storyid_t id )
{
    
    static char Subject[64];
    static char Message[64];
    static char ToAddress[64];
    static char path[MAX_PATH];
    
    
    if ( s3eEMailAvailable() && s3eEMailCanSendMail() )
    {
        lib::strcpy(Message,"") ;
        lib::strcpy(Subject,"Tester Save Game") ;
        lib::strcpy(ToAddress,"test@thelordsofmidnight.com") ;
        
        
        s3eEMail *mailz = new s3eEMail();
        s3eEMailAttachment attachment = {0};
        
        const char* _toAddress = ToAddress;
        mailz->m_toRecipients = &_toAddress ;
        
        mailz->m_numToRecipients = 1;
        mailz->m_subject = Subject;
        mailz->m_messageBody = Message;
        
        strcpy(path, (char*)getPath(id));
        
        attachment.m_fileName = strdup(&path[6]) ; // skip RAM:
        attachment.m_mimeType="application/octet-stream";
        
        os::file* file = new os::file( path, os::file::modeRead);
        attachment.m_dataSize = file->GetLength();
        void* data = malloc(attachment.m_dataSize);
        file->Read(data, attachment.m_dataSize);
        file->Close();
        
        attachment.m_data = data ;
        
        mailz->m_attachments = &attachment;
        mailz->m_numAttachments = 1;
        
        if (s3eEMailSendMail(mailz) == S3E_RESULT_SUCCESS)
        {
        }
        
        data = (void*)attachment.m_data;
        SAFEFREE( data );
        
        data = (void*)attachment.m_fileName;
        SAFEFREE( data );
        
        SAFEDELETE(mailz);
        
        return TRUE;
    }
    
    return FALSE;
    
}

BOOL storymanager::isEmailEnabled()
{
    return s3eEMailAvailable() && s3eEMailCanSendMail();
}

BOOL storymanager::copyToSdCard()
{
    UIDEBUG("storymanager::copyToSdCard\n");
    
    
    char path[MAX_PATH];
    
    for ( int s=0; s<MAX_STORIES; s++ )
    {
        char filename1[MAX_PATH]={0};
        char filename2[MAX_PATH]={0};
        char filename3[MAX_PATH]={0};
        
        if ( !os::filemanager::Exists(getPath(s+1)) )
            continue;
        
        strcpy(path,getFolder(s+1));
        
        UIDEBUG(path);
        
        s3eFileList* file = s3eFileListDirectory(path);
        if ( file != NULL ) {
            
            sprintf(filename3,"raw://sdcard/%s/story/%d", TME_ScenarioDirectory() , s+1);
            if ( IwFileMakeDirs( filename3 ) == S3E_RESULT_SUCCESS ) {
                
                while ( s3eFileListNext( file, filename1, MAX_PATH ) == S3E_RESULT_SUCCESS ) {
                    
                    sprintf(filename2, "%s/%s", path, filename1); // src
                    sprintf(filename3,"raw://sdcard/%s/story/%d/%s",TME_ScenarioDirectory(),s+1,filename1); //dst
                    
                    UIDEBUG("COPY %s TO %s\n", filename2, filename3 );
                    
                    IwFileCopy(filename3, filename2);
                    
                }
            } else {
                UIDEBUG("Can't create folder %s\n", filename3 );
            }
            s3eFileListClose(file);
            
        }
    }
    
    return true;
}





BOOL storymanager::debug_email_all()
{
    
    static char Subject[64];
    static char Message[64];
    static char ToAddress[64];
    static char path[MAX_PATH];
    
    if ( s3eEMailAvailable() && s3eEMailCanSendMail() )
    {
        lib::strcpy(Message,"") ;
        lib::strcpy(Subject,"Tester Save Game") ;
        lib::strcpy(ToAddress,"test@thelordsofmidnight.com") ;
        
        
        s3eEMail *mailz = new s3eEMail();
        s3eEMailAttachment attachment[MAX_STORIES] ;
        
        CLEARARRAY(attachment);
        
        const char* _toAddress = ToAddress;
        mailz->m_toRecipients = &_toAddress ;
        
        mailz->m_numToRecipients = 1;
        mailz->m_subject = Subject;
        mailz->m_messageBody = Message;
        mailz->m_numAttachments = 0;
        
        int count=0;
        for ( int s=0; s<MAX_STORIES; s++ )
        {
            attachment[count].m_fileName = (char*)getPath(s+1) ;
            if ( os::filemanager::Exists(attachment[count].m_fileName) )
            {
                attachment[count].m_mimeType="application/octet-stream";
                
                strcpy(path, (char*)getPath(s+1));
                
                os::file* file = new os::file( path, os::file::modeRead);
                attachment[count].m_dataSize = file->GetLength();
                
                void* data = malloc(attachment[count].m_dataSize);
                file->Read(data, attachment[count].m_dataSize);
                file->Close();
                
                attachment[count].m_data = data ;
                attachment[count].m_fileName = strdup(&path[6]) ; // skip RAM:
                
                count++;
            }
        }
        
        mailz->m_attachments = attachment;
        mailz->m_numAttachments = count;
        
        if (s3eEMailSendMail(mailz) == S3E_RESULT_SUCCESS)
        {
        }
        
        for ( int s=0; s<MAX_STORIES; s++ ) {
            void* data = (void*)attachment[s].m_data ;
            SAFEFREE( data );
            data = (void*)attachment[s].m_fileName ;
            SAFEFREE( data );
            
            
        }
        
        SAFEDELETE(mailz);
        
        return TRUE;
    }
    
    return FALSE;
}
#endif


BOOL storymanager::save( savemode_t mode )
{
    if ( current == 0 )
        return false;
    return save(current, mode );
}


BOOL storymanager::save ( storyid_t id, savemode_t mode )
{
    undo_last_available=TRUE;
    
    if ( mode == savemode_night ) {
        last_night.Add( last_save );
        undo_last_available=FALSE;
        return TRUE;
    }
    else if ( mode == savemode_dawn ) {
        last_morning.Add( last_save+1 );
        undo_last_available=FALSE;
    }
    
    char* file = (char*)getPath(id) ;
    
    // copy save file to last
    if ( last_save  ) {
        char* cpy = (char*)getPath(id, last_save);
        if ( !filemanager::Copy( file, cpy ) ) {
            UIDEBUG("Unable to create undo file");
        }
    }
    
    last_save++;
    
    if ( filemanager::Exists(file) )
        filemanager::Remove(file);
    
    if ( !TME_Save( file,loadsave ) ) {
        
        last_save--;
        
        return FALSE ;
    }
    
    TME_SaveDiscoveryMap( (char*)DiscoveryMapFilename() );
    
    cleanup();
    
    return TRUE;
}

BOOL storymanager::load ( storyid_t id )
{
    current = id;
    undo_last_available=FALSE;
    
    if (!TME_Load( (char*)getPath(id), loadsave ) ){
        return FALSE ;
    }
    
    TME_LoadDiscoveryMap( (char*)DiscoveryMapFilename() );
    
    return TRUE ;
}

BOOL storymanager::canUndo ( savemode_t mode )
{
    u32 save=last_save-1;
    if ( mode == savemode_dawn ) {
        save=lastMorning()>0 ? lastMorning() : 1 ;
    } else if ( mode == savemode_night )
        save=lastNight();
    else if ( mode == savemode_last && _DEBUG_UNDO_HISTORY_ == 1 )
        return undo_last_available ;
    
    return filemanager::Exists(getPath(current,save));
    
}


u32 storymanager::lastMorning() const
{
    u32 dawn = 0;
    
    if ( last_morning.Count()>0 ) {
        
        dawn = last_morning.Get(last_morning.Count()-1);
        
        // are we at dawn?
        // then we want the previous dawn
        if ( dawn == last_save && last_morning.Count() > 1 ) {
            
            dawn = last_morning.Get(last_morning.Count()-2);
            
        }
        
        return dawn ;
    }
    
    
    
    
    return 0;
}

u32 storymanager::lastNight() const
{
    if ( last_night.Count()>0 )
        return last_night.Get(last_night.Count()-1);
    return 0;
}

BOOL storymanager::getDescription( storyid_t id, c_str& description )
{
    return TME_SaveDescription((char*)getPath(id), description);
}

BOOL storymanager::undo ( savemode_t mode )
{
    u32 save=last_save-1;
    if ( mode == savemode_dawn )
        save=lastMorning();
    else if ( mode == savemode_night )
        save=lastNight();
    
    u32 temp_save = last_save ;
    
    char* file = (char*)getPath(current,save);
    
    if ( TME_Load( file , loadsave ) ) {
        
        char* cpy = (char*)getPath(current) ;
        filemanager::Copy( file, cpy );
        
        for ( u32 ii=save; ii<temp_save; ii++ )  {
            LPCSTR file = getPath(current,ii);
            if ( filemanager::Exists(file) )
                filemanager::Remove(file);
        }
        
        undo_last_available=FALSE;
        
        return TRUE;
    }
    
    return FALSE;
}

BOOL storymanager::cleanup ( void )
{
    s32 final_save = MAX(0,last_save-_DEBUG_UNDO_HISTORY_);
    for ( s32 ii=0; ii<final_save; ii++ ) {
        if ( !last_morning.IsInList(ii) ) {
            LPCSTR file = getPath(current,ii) ;
            if ( filemanager::Exists(file) ) {
                filemanager::Remove(file);
            }
        }
    }
    
    return true;
}


BOOL storymanager::destroy( storyid_t id )
{
    current = 0 ;

    LPCSTR file = getFolder(id) ;

    return filemanager::DestroyDirectory(file) ;
}


BOOL storymanager::Serialize( u32 version, archive& ar )
{
    u32 count = 0;
    u32 value = 0;
    
    if ( ar.IsStoring() ) {
        
        ar << last_save ;
        
        
        ar << last_night.Count();
        for ( int i =0; i<last_night.Count(); i++ )
            ar << last_night.Get(i) ;
        
        ar << last_morning.Count();
        for ( int i =0; i<last_morning.Count(); i++ )
            ar << last_morning.Get(i) ;
        
    }else{
        
        ar >> last_save ;
        
        last_night.Clear();
        last_morning.Clear();
        
        
        if ( version < 13 ) {
            
            ar >> value ;
            last_night.Add( value );
            
            ar >> value ;
            last_morning.Add( value );
            
        }else{
            
            ar >> count ;
            for ( int i =0; i<count; i++ ) {
                ar >> value ;
                last_night.Add(value);
            }
            
            ar >> count ;
            for ( int i =0; i<count; i++ ) {
                ar >> value ;
                last_morning.Add(value);
            }
            
        }
        
        
        
    }
    
    return TRUE;
}


