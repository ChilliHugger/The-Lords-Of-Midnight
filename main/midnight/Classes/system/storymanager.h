//
//  storymanager.h
//  midnight
//
//  Created by Chris Wild on 15/12/2017.
//
//

#pragma once
#include "../library/libinc/mxtypes.h"
#include "../library/libinc/collections.h"
#include "../tme/baseinc/lomxtypes.h"
#include "ringcontroller.h"

using chilli::collections::c_u32;

#define MAX_STORIES 8
#define STORY_NONE 0
typedef u32 storyid_t;

typedef enum savemode_t {
    savemode_normal=0,
    savemode_night,
    savemode_dawn,
    savemode_last,
} savemode_t;

typedef struct storyheader_t {
    storyid_t           id;
    c_str               description ;
    //cocos2d::Color3B    color;
    savemode_t          mode;
} storyheader_t;

typedef struct storyinfo_t
{
    u32                 count;
    storyheader_t       chapter[MAX_STORIES];
} storyinfo_t ;

class storymanager : public ringcontroller
{
public:
    storymanager();
    virtual ~storymanager();
    
    void checkStories(void);
    storyinfo_t* getStoriesInfo();
    
    int stories_used()  ;
    storyid_t next_free_story()  ;
    storyid_t first_used_story() ;
    storyid_t alloc();
    
    LPCSTR getFolder( storyid_t id );
    LPCSTR getPath( storyid_t id );
    
    LPCSTR getPath(storyid_t id, u32 save);
    
    storyid_t current_story() const { return current; }
    
    BOOL save ( savemode_t mode = savemode_normal );
    BOOL create ( storyid_t id );
    BOOL save ( storyid_t id, savemode_t mode=savemode_normal );
    BOOL load ( storyid_t id );
    BOOL destroy ( storyid_t id );
    BOOL isUsed ( storyid_t id ) { return used[id-1]; }
    BOOL getDescription( storyid_t id, c_str& description );
    
    BOOL cleanup ( void );
    
    BOOL canUndo ( savemode_t mode );
    BOOL undo ( savemode_t mode );
    
#if defined(_DEBUG_EMAIL_)
    BOOL debug_email ( storyid_t id );
    BOOL debug_email_all();
    BOOL isEmailEnabled();
    BOOL copyToSdCard();
#endif

    
    void SetLoadSave( tme::PFNSERIALIZE function );
    BOOL Serialize( u32 version, chilli::lib::archive& ar );
    
    LPCSTR DiscoveryMapFilename();
    
    u32 lastMorning() const ;
    u32 lastNight() const ;
    
    
private:
    BOOL            used[MAX_STORIES];
    storyid_t       current;
    tme::PFNSERIALIZE    loadsave;
    
    u32             last_save;
    c_u32           last_night;
    c_u32           last_morning;
    BOOL            undo_last_available;
    
    
};

