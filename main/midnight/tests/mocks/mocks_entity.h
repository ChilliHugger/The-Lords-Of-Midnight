//
//  mocks.hpp
//  midnight
//
//  Created by Chris Wild on 26/10/2023
//
#pragma once

#if defined(_LOM_)
    #include "../../../../src/tme/scenarios/lom/scenario_lom_internal.h"
    
    #define BASE_CHARACTER mxcharacter
    #define BASE_FACTORY lom_entityfactory
    
#endif

using namespace std;
using namespace tme;

class MockData {
public:
    void Clear() {
        events.clear();
        properties.clear();
    }
    
public:
    vector<string>      events;
    map<string, string> properties;
};


#define TRIGGERED(x) \
    SetEvent(__FUNCTION__); \
    BASE_CHARACTER::x();

#define TRACK_VOID(x) \
    virtual void x() override { SetEvent(__FUNCTION__); BASE_CHARACTER::x(); }

#define HANDLE_BOOL(x) \
    virtual bool x() const override { BOOL_OVERRIDE_CHECK(x); }

#define BOOL_OVERRIDE_CHECK(x) \
    if ( GetProperty(__FUNCTION__) == "false" ) return false; \
    if ( GetProperty(__FUNCTION__) == "true" ) return true; \
    BASE_CHARACTER::x()

class mockcharacter : public BASE_CHARACTER
{
public:
    void SetEvent( string name ) {
        mockData.events.push_back(name);
    }

    string GetProperty( string name ) const {
        return mockData.properties.find(name) == mockData.properties.end()
            ? ""
            : mockData.properties.at(name);
    }

    bool HasEvent(string event)
    {
        auto vec = mockData.events;
        return ( std::find(vec.begin(), vec.end(), event) != vec.end() );
    }

    //
    //
    //

    TRACK_VOID ( Cmd_Dead );
    TRACK_VOID ( LostFight );
    TRACK_VOID ( Dismount );

    HANDLE_BOOL ( ShouldLoseHorse );
    HANDLE_BOOL ( ShouldDieInFight );
    
    public:
        MockData    mockData;
};

class mockentityfactory : public BASE_FACTORY
{
public:
    mxentity* Create(id_type_t type) override
    {
        if ( type == IDT_CHARACTER )
            return static_cast<mxentity*>(new mockcharacter);
            
        return BASE_FACTORY::Create(type);
    }
};
