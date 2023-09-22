//
//  thing_mapping.cpp
//  midnight
//
//  Created by Chris Wild on 07/09/2023.
//

#include "thing_mapping.h"


namespace tme {
    namespace utils {

        void UpdateObjectIds()
        {
            for(int ii=0; ii<sv_objects; ii++) {
                auto object = mx->ObjectById(ii+1);
                if(object->IsThing()) {
                    mx->thing_remap_table[object->thing-1] = object;
                }
            }
        }
    }
}
