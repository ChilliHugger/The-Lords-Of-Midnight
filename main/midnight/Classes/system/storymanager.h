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

typedef u32 storyid_t;
typedef u32 saveid_t;

constexpr saveid_t SAVE_NONE = 0;
constexpr storyid_t STORY_NONE = 0;
constexpr u32 MAX_STORIES = 8;

typedef enum savemode_t {
    savemode_normal=0,
    savemode_night,
    savemode_dawn,
    savemode_last,
} savemode_t;

typedef struct storyheader_t {
    storyid_t           id;
    c_str               description ;
    u32                 slot;
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
    
    LPCSTR getPath(storyid_t id, saveid_t save);
    
    storyid_t current_story() const { return currentStory; }
    
    bool save ( savemode_t mode = savemode_normal );
    bool create ( storyid_t id );
    bool save ( storyid_t id, savemode_t mode=savemode_normal );
    bool load ( storyid_t id );
    bool destroy ( storyid_t id );
    bool isUsed ( storyid_t id ) { return used[id-1]; }
    bool getDescription( storyid_t id, c_str& description );
    
    bool cleanup ( void );
    
    bool canUndo ( savemode_t mode );
    bool undo ( savemode_t mode );
    
#if defined(_DEBUG_EMAIL_)
    bool debug_email ( storyid_t id );
    bool debug_email_all();
    bool isEmailEnabled();
    bool copyToSdCard();
#endif

    
    void SetLoadSave( tme::PFNSERIALIZE function );
    bool Serialize( u32 version, chilli::lib::archive& ar );
    
    LPCSTR DiscoveryMapFilename();
    
    saveid_t lastMorning() const ;
    saveid_t lastNight() const ;
    
    
private:
    bool                used[MAX_STORIES];
    storyid_t           currentStory;
    tme::PFNSERIALIZE   loadsave;
    saveid_t            last_save;
    c_u32               last_night;
    c_u32               last_morning;
    bool                undo_last_available;
    storyinfo_t         stories ;
    
};

