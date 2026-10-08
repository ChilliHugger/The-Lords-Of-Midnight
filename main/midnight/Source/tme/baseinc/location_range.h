#ifndef _LOCATION_RANGE_H_INCLUDED_
#define _LOCATION_RANGE_H_INCLUDED_

namespace tme {

    // Visits every location of a map in row order, giving the location's
    // coordinates and the square held there.
    //
    //   for ( auto [loc, sqr] : map->Locations() ) { ... }
    //
    // T is the type of a square (mxloc, flags32)
    template <typename T>
    class location_range
    {
    public:
        struct entry {
            mxgridref   loc;
            T&          sqr;
        };

        class iterator
        {
        public:
            iterator( T* data, int width, int index ) :
                m_data(data), m_width(width), m_index(index), m_x(index % width), m_y(index / width) {}

            entry operator*() const { return { mxgridref(m_x, m_y), m_data[m_index] }; }

            iterator& operator++() {
                m_index++;
                if ( ++m_x == m_width ) {
                    m_x = 0;
                    m_y++;
                }
                return *this;
            }

            bool operator!=( const iterator& other ) const { return m_index != other.m_index; }

        private:
            T*  m_data;
            int m_width;
            int m_index;
            int m_x;
            int m_y;
        };

        location_range( T* data, size dimensions ) : m_data(data), m_size(dimensions) {}

        iterator begin() const { return iterator( m_data, m_size.cx, 0 ); }
        iterator end() const   { return iterator( m_data, m_size.cx, m_size.cx * m_size.cy ); }

    private:
        T*      m_data;
        size    m_size;
    };

}

#endif //_LOCATION_RANGE_H_INCLUDED_
