#include "../inc/mxtypes.h"
#include "../inc/carchive.h"

namespace chilli {

    namespace types {

        range::range()
        {
            min=0;
            max=0;
        }

        range::~range()
        {

        }

        range::range( int min, int max )
        { 
            this->min = min; 
            this->max = max;
        }

        bool range::IsInRange( int value )
        { 
            return value>=min && value<=max ;
        }

        int range::Range() const 
        {
            return ABS( max-min );
        }


        range& range::operator= ( range_t r )
        {
            min = r.min;
            max = r.max;
            return *this;
        }

    }
    // namespace types
}
// namespace tme
