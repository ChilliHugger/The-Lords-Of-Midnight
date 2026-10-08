#ifndef _MAP_REGENERATOR_H_INCLUDED_
#define _MAP_REGENERATOR_H_INCLUDED_

namespace tme {

    // Puts things back on the map as the game progresses. Each scenario
    // provides its own rules for what comes back and when.
    class map_regenerator
    {
    public:
        virtual ~map_regenerator() {}

        // called once when a new game is created
        virtual void initialise() = 0;

        // called each night
        virtual void process() = 0;

    protected:
        // a stable pseudo random number for a location, used to pick
        // which of a terrain's things lives there
        virtual int LocationKey( mxgridref loc ) const;
    };

}

#endif //_MAP_REGENERATOR_H_INCLUDED_
