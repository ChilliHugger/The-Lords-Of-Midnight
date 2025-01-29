//
//  CustomDirector.h
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef CustomDirector_h
#define CustomDirector_h

#include "axmol.h"

class CustomDirector : public ax::Director
{
    using Scene = ax::Scene;
public:
    template<class T>  void popSceneWithTransition(float duration)
    {
        AXASSERT(_runningScene != NULL, "running scene should not null");
        
        _scenesStack.popBack();
        ssize_t c = _scenesStack.size();
        
        if (c == 0)
        {
            end();
        }
        else
        {
            Scene *scene = (Scene *)T::create(duration, (Scene *)_scenesStack.at(c-1));
            _scenesStack.replace(c-1, scene);
            _sendCleanupToScene = true;
            _nextScene = (Scene*)_scenesStack.at(c - 1);
        }
    }

};


#endif /* CustomDirector_h */
