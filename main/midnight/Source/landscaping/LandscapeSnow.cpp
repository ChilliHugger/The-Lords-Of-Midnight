#include "LandscapeSnow.h"

USING_NS_AX;

// longer than the longest flake life, so the screen is full from the first frame
static const float PREWARM_SECONDS = 15.0f;

// Snow emitter that can move its live flakes, so a pan is seen from the first frame
class SnowLayer : public ParticleSnow
{
public:
    static SnowLayer* create( int count )
    {
        auto p = new (std::nothrow) SnowLayer();
        if ( p && p->initWithTotalParticles(count) ) {
            p->autorelease();
            return p;
        }
        AX_SAFE_DELETE(p);
        return nullptr;
    }

    void shift( float dx )
    {
        for ( unsigned int ii=0; ii<_particleCount; ii++ )
            _particleData.posx[ii] += dx;
    }
};

LandscapeSnow* LandscapeSnow::create( const Size& size )
{
    auto snow = new (std::nothrow) LandscapeSnow();
    if ( snow && snow->initWithSize(size) ) {
        snow->autorelease();
        return snow;
    }
    AX_SAFE_DELETE(snow);
    return nullptr;
}

bool LandscapeSnow::initWithSize( const Size& s )
{
    if ( !Node::init() )
        return false;

    size = s;
    setContentSize(size);
    setAnchorPoint(Vec2::ZERO);

    // far layer: many small, slow flakes
    layers[0] = createLayer( 300, size.height*0.008f, size.height*0.004f, size.height*0.05f, 14.0f, 6.0f );
    // near layer: fewer, larger, faster flakes
    layers[1] = createLayer( 80, size.height*0.022f, size.height*0.010f, size.height*0.12f, 8.0f, 12.0f );

    for ( int ii=0; ii<2; ii++ ) {
        baseRate[ii] = layers[ii]->getEmissionRate();
        baseWind[ii] = layers[ii]->getGravity().x;
        layers[ii]->setEmissionRate(0);
        addChild(layers[ii]);
    }
    scheduleUpdate();
    return true;
}

SnowLayer* LandscapeSnow::createLayer( int count, float flakeSize, float flakeSizeVar,
                                                float speed, float life, float wind )
{
    auto p = SnowLayer::create(count);
    p->setPosition( Vec2(size.width/2, size.height + flakeSize) );
    p->setPosVar( Vec2(size.width/2 + size.height*0.3f, 0) );   // overscan so wind doesn't leave gaps
    p->setStartSize(flakeSize);
    p->setStartSizeVar(flakeSizeVar);
    p->setEndSize(ParticleSystem::START_SIZE_EQUAL_TO_END_SIZE);
    p->setSpeed(speed);
    p->setSpeedVar(speed*0.3f);
    p->setGravity( Vec2(wind, -speed*0.15f) );   // wind is re-applied each frame in update()
    p->setTangentialAccel(0);
    p->setTangentialAccelVar(wind);                              // gentle sway
    p->setAngle(-90);
    p->setAngleVar(10);
    p->setLife(life);
    p->setLifeVar(life*0.25f);
    p->setEmissionRate(count/life);
    p->setStartColor( Color4F(1,1,1,0.9f) );
    p->setEndColor( Color4F(1,1,1,0.6f) );
    p->setPositionType( ParticleSystem::PositionType::RELATIVE );
    p->setBlendFunc( BlendFunc::ALPHA_NON_PREMULTIPLIED );
    return p;
}

void LandscapeSnow::setSnowing( bool value )
{
    if ( snowing == value )
        return;
    snowing = value;

    // Stopping the emitter lets existing flakes finish falling instead of vanishing
    for ( int ii=0; ii<2; ii++ ) {
        layers[ii]->setEmissionRate( snowing ? baseRate[ii] : 0 );
        if ( snowing )
            layers[ii]->resetSystem();
    }

    // First time snow starts the sky is empty, so run the simulation forward
    // until flakes have fallen across the whole screen
    if ( snowing && layers[0]->getParticleCount() == 0 && layers[1]->getParticleCount() == 0 ) {
        const float step = 1.0f/30.0f;
        for ( float t=0; t<PREWARM_SECONDS; t+=step ) {
            for ( int ii=0; ii<2; ii++ )
                layers[ii]->update(step);
        }
    }
    setVisible(true);
}

void LandscapeSnow::setLook( float lookAmount )
{
    if ( hasLook ) {
        // flakes in the near layer move further, like objects close to the viewer
        float dx = -(lookAmount - lastLook);
        layers[0]->shift(dx*0.5f);
        layers[1]->shift(dx);
    }
    lastLook = lookAmount;
    hasLook = true;
}

void LandscapeSnow::update( float delta )
{
    Node::update(delta);
    time += delta;

    // slow, irregular gusts so the flakes drift in and out of the wind
    float gust = 1.0f + 1.5f*sinf(time*0.35f) + 0.8f*sinf(time*1.1f + 1.7f);

    for ( int ii=0; ii<2; ii++ ) {
        layers[ii]->setGravity( Vec2(baseWind[ii]*gust, layers[ii]->getGravity().y) );
    }
}
