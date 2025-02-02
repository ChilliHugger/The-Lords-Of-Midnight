
#include "../inc/mxtypes.h"

#include <time.h>
#include <stdlib.h>

namespace chilli {

    namespace types {

        //randomno g_random;
/* linear congruential generator.  Generator x[n+1] = a * x[n] mod m */
//#define RAND_INT(l,h) (((int)(random() * ((double)(h)-(l)+1))) + (l))
//double random (void);          /* return the next random number x: 0 <= x < 1*/
//void  rand_seed (unsigned int);         /* seed the generator */
/* The following parameters are recommended settings based on research
   uncomment the one you want. */
//   static unsigned int a = 1588635695, m = 4294967291U, q = 2, r = 1117695901;
/* static unsigned int a = 1223106847, m = 4294967291U, q = 3, r = 625646750;*/
/* static unsigned int a = 279470273, m = 4294967291U, q = 15, r = 102913196;*/
/* static unsigned int a = 1583458089, m = 2147483647, q = 1, r = 564025558; */
/* static unsigned int a = 784588716, m = 2147483647, q = 2, r = 578306215;  */
/* static unsigned int a = 16807, m = 2147483647, q = 127773, r = 2836;      */
/* static unsigned int a = 950706376, m = 2147483647, q = 2, r = 246070895;  */
//   SEED = a*(SEED % q) - r*(SEED / q);
//   return ((double)SEED / (double)m);
//void rand_seed (unsigned int init)   {if (init != 0) SEED = init;}

        
        randomno* randomno::instance = new randomno();
        
        randomno::randomno()
        {
            m_seed = 93186752L;
        }

        randomno::~randomno()
        {
        }

        void randomno::srand ( u32 seed )
        {
            m_seed = (u32)seed;
        }

        int randomno::get ( void )
        {
            return get(SHRT_MAX);
        }

        void randomno::seed ( u32 seed )
        {
            srand ( seed ) ;
        }

        u32 randomno::seed()
        {
            return m_seed ;
        }

        f32 randomno::getfloat()
        {
            m_seed = ((((m_seed + 1)*1951) % 65537) - 1);
            return (f32) (((double)m_seed / (double)65535)); 
        }

        int randomno::get ( int number )
        {
            f32 f = getfloat();
            
            int n = (int) ROUNDFLOAT( (f*number) );
            return n;
        }

        int randomno::get ( int lower, int upper )
        {
            int range=upper-lower;
            return (int) ( get( range ) + lower);
        }

        bool randomno::chance ( f32 chance )
        {
            return getfloat() < chance ;
        }

        void randomno::randomize ( void )
        {
            seed( (unsigned)time( NULL ) );
        }

    }
    // namespace types
}
// namespace tme
