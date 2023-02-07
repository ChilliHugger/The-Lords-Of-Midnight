//
//  storymanager.h
//  midnight
//
//  Created by Chris Wild on 15/12/2017.
//
//

#pragma once
#include "../library/inc/mxtypes.h"
#include "../library/inc/collections.h"
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
    std::string         description ;
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
    
    virtual void checkStories(void);
    virtual storyinfo_t* getStoriesInfo();
    
    virtual int stories_used()  ;
    virtual storyid_t next_free_story()  ;
    virtual storyid_t first_used_story() ;
    virtual storyid_t alloc();
    virtual saveid_t lastMorning() const ;
    virtual saveid_t lastNight() const ;
    
    virtual bool save ( savemode_t mode = savemode_normal );
    virtual bool create ( storyid_t id, u64 flags, mxdifficulty_t difficulty );
    virtual bool save ( storyid_t id, savemode_t mode=savemode_normal );
    virtual bool load ( storyid_t id );
    virtual bool destroy ( storyid_t id );
    virtual bool cleanup ( void );
    virtual bool canUndo ( savemode_t mode );
    virtual bool undo ( savemode_t mode );

    std::string getFolder( storyid_t id );
    std::string getPath( storyid_t id );
    std::string getPath(storyid_t id, saveid_t save);

    storyid_t current_story() const { return currentStory; }
    
    bool isUsed ( storyid_t id ) { return used[id-1]; }
    std::string getDescription( storyid_t id );
        
    virtual void SetLoadSave( tme::PFNSERIALIZE function );
    bool Serialize( u32 version, chilli::lib::archive& ar );
    
    std::string DiscoveryMapFilename();
    
protected:
    bool                used[MAX_STORIES];
    storyid_t           currentStory;
    tme::PFNSERIALIZE   loadsave;
    saveid_t            last_save;
    c_u32               last_night;
    c_u32               last_morning;
    bool                undo_last_available;
    storyinfo_t         stories ;
};

