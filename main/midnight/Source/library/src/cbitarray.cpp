/*
 * FILE:    bitarray.cpp
 * 
 * PROJECT: 
 * 
 * PURPOSE: 
 * 
 * CREATED: 
 * 
 * AUTHOR:  Chris Wild
 * 
 * (c) Copyright 2009 Chilli Hugger Software.
 * 
 */

#include "../inc/mxtypes.h"
#include "../inc/carchive.h"

namespace chilli {

    namespace types {

        using namespace chilli::lib ;
        
        bool bitarray::Create ( int nElements )
        {
        u8    *temp;

            m_nElements = nElements ;

            temp = new u8[Size()];
                
            if ( temp == NULL )
                return(FALSE);

            Attatch( temp, nElements );

            ClrAll();

            return ( TRUE );
            
        }

        void bitarray::Set ( int bit )
        {
        int offset, bitflag;

            offset = bit / 8;
            bit %= 8;
            bitflag = 1 << bit;
            m_pData[offset] |= bitflag;
        }

        void bitarray::Clr ( int bit )
        {
        int offset, bitflag;

            offset = bit / 8;
            bit %= 8;
            bitflag = 1 << bit;
            m_pData[offset] &= ~bitflag;
        }

        void bitarray::Xor ( int bit )
        {
        int offset, bitflag;

            offset = bit / 8;
            bit %= 8;
            bitflag = 1 << bit;
            m_pData[offset] ^= bitflag;
        }

        bool bitarray::Get ( int bit ) const
        {
        int offset, bitflag;

            offset = bit / 8;
            bit %= 8;
            bitflag = 1 << bit;
            return ( (m_pData[offset] & bitflag) >> bit );
        }

        archive& bitarray::Serialize ( archive& ar )
        {
            if ( ar.IsStoring() ){
                ar << m_nElements;
                ar.Write ( m_pData, Size() );
            }else{
            int temp;
                ar >> temp;
                _MXASSERTE( temp == m_nElements  );
        //        if ( temp != m_nElements  ) {
        //            DEBUG("bitarray: Different number of elements\n");
        //            return(FALSE);
        //        }
                ar.Read ( m_pData, Size() );
            }
            return(ar);
        }

        int bitarray::Size() const
        {
        int    cbSize;
            cbSize = m_nElements / 8;
            if ( m_nElements % 8 ) cbSize++;
            return ( cbSize );
        }

    }
    // namespace types

}
// namespace types
