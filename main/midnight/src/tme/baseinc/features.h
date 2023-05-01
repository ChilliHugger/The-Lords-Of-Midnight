//
//  features.h
//  midnight
//
//  Created by Chris Wild on 29/04/2023.
//

#ifndef features_h
#define features_h

//#define _DDR_
//#define _LOM_
#define _TUNNELS_

#if !defined(_TUNNELS_)
#if defined(_DDR_)
    #define _TUNNELS_
#endif
#endif

//#define _TEST_WINLOSE_CONDITIONS_


#endif /* features_h */
