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

#if defined(_DDR_)
    #include "../../../../src/tme/scenarios/ddr/scenario_ddr_internal.h"
    
    #define BASE_CHARACTER ddr_character
    #define BASE_FACTORY ddr_entityfactory
    #define BASE_OBJECT ddr_object
    
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

template<typename T>
T ConvertStringTo(const string& value)
{
    return (T)StringExtensions::atol(value);
}

#define INIT_MOCK_CLASS(x) \
void SetEvent( string name ) const { \
    const_cast<x*>(this)->mockData.events.push_back(name); \
} \
\
string GetProperty( string name ) const { \
    return mockData.properties.find(name) == mockData.properties.end() ? "" : mockData.properties.at(name); \
} \
\
bool HasEvent(string event) { \
    return ( std::find(mockData.events.begin(), mockData.events.end(), event) != mockData.events.end() );  \
} \
\
bool Override( const string& name, function<bool(void)> func) const { \
    SetEvent(name); \
    auto value = GetProperty(name);  \
    if ( value == "false" ) return false;  \
    if ( value == "true" ) return true;  \
    return func();  \
}  \
\
u32 OverrideU32( const string& name, function<u32(void)> func) const { \
    SetEvent(name); \
    auto value = GetProperty(name);  \
    if ( !value.empty() ) return (u32)StringExtensions::atol(value);  \
    return func();  \
}  \
\
MockData    mockData \


#define CAPTURE_VOID(x) \
    virtual void x() override { SetEvent(__FUNCTION__); BASE_CHARACTER::x(); }

class mockcharacter : public BASE_CHARACTER
{
public:
    INIT_MOCK_CLASS(mockcharacter);

    CAPTURE_VOID ( Cmd_Dead );
    CAPTURE_VOID ( Dismount );
    
    virtual void LostFight(s32 hint) override
    {
        SetEvent(__FUNCTION__);
        BASE_CHARACTER::LostFight(hint);
    }

    virtual bool ShouldLoseHorse(s32 hint) const override
    {
        return Override(__FUNCTION__, [&] {
            return BASE_CHARACTER::ShouldLoseHorse(hint);
        });
    }

    virtual bool ShouldDieInFight() const override
    {
        return Override(__FUNCTION__, [&] {
            return BASE_CHARACTER::ShouldDieInFight();
        });
    }

    virtual bool ShouldWeStopTurnForEnemy() const override
    {
        return Override(__FUNCTION__, [&] {
            return BASE_CHARACTER::ShouldWeStopTurnForEnemy();
        });
    }

    
#if defined(_DDR_)
    virtual bool ShouldWeStayAndFight(const mxarmytotal* friends, const mxarmytotal* foe) const override
    {
        SetEvent(__FUNCTION__);
        return BASE_CHARACTER::ShouldWeStayAndFight(friends,foe);
    }
    
    virtual mxorders_t pickNewOrders () const override
    {
        auto value = GetProperty(__FUNCTION__);
        if ( !value.empty() )
            return ConvertStringTo<mxorders_t>(value);
        return BASE_CHARACTER::pickNewOrders();
    }
    
    
#endif

};

#if defined(_DDR_)

#undef CAPTURE_VOID
#define CAPTURE_VOID(x) \
    virtual void x() override { SetEvent(__FUNCTION__); BASE_OBJECT::x(); }


class mockobject : public BASE_OBJECT
{
public:
    INIT_MOCK_CLASS(mockobject);
    
    virtual u32 FightHP() const override
    {
        return OverrideU32(__FUNCTION__, [&] {
            return BASE_OBJECT::FightHP();
        });
    }
    virtual u32 KillRate(u32 hp) const override
    {
        return OverrideU32(__FUNCTION__, [&] {
            return BASE_OBJECT::KillRate(hp);
        });
    }
    virtual u32 FightSuccess() const override
    {
        return OverrideU32(__FUNCTION__, [&] {
            return BASE_OBJECT::FightSuccess();
        });
    }
};

#endif

class mockentityfactory : public BASE_FACTORY
{
public:
    mxentity* Create(id_type_t type) override
    {
        if ( type == IDT_CHARACTER )
            return static_cast<mxentity*>(new mockcharacter);
#if defined(_DDR_)
        if ( type == IDT_OBJECT )
            return static_cast<mxentity*>(new mockobject);
#endif
        return BASE_FACTORY::Create(type);
    }
};
