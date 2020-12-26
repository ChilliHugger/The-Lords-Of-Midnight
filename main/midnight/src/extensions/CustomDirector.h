//
//  CustomDirector.h
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef CustomDirector_h
#define CustomDirector_h

#include "cocos2d.h"

class CustomDirector : public cocos2d::Director
{
    using Scene = cocos2d::Scene;
public:
    template<class T>  void popSceneWithTransition(float duration)
    {
        CCASSERT(_runningScene != NULL, "running scene should not null");
        
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
    
    void resetOpenGLView()
    {
        // set size
        _winSizeInPoints = _openGLView->getDesignResolutionSize();

        //_isStatusLabelUpdated = true;

        if (_openGLView)
        {
            setGLDefaultValues();
        }

       //_renderer->init();

        //if (_eventDispatcher)
       //{
       //     _eventDispatcher->setEnabled(true);
       // }
    }
};


#endif /* CustomDirector_h */
