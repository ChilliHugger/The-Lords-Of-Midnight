//
//  progressmonitor.hpp
//  midnight
//
//  Created by Chris Wild on 14/06/2018.
//

#ifndef progressmonitor_hpp
#define progressmonitor_hpp

#include <stdio.h>

typedef std::function<void(int)> MXProgressCallback;


class progressmonitor
{
public:
    
    progressmonitor(MXProgressCallback callback);
    virtual ~progressmonitor();
    
    void Start();
    void Update( const std::string& text,int value);
    void Stop();
    
protected:
    void NotifyProgress();
    
public:
    MXProgressCallback  callback;
    int current;
};


#endif /* progressmonitor_hpp */
