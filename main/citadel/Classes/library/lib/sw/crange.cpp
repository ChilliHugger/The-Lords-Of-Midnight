// range.cpp: implementation of the range class.
//
//////////////////////////////////////////////////////////////////////

#include "../../libinc/library.h"


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

		BOOL range::IsInRange( int value )
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
