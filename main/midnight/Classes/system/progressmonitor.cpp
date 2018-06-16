//
//  progressmonitor.cpp
//  midnight
//
//  Created by Chris Wild on 14/06/2018.
//

#include "progressmonitor.h"
#include "moonring.h"

progressmonitor::~progressmonitor()
{
    callback = nullptr;
}

progressmonitor::progressmonitor(MXProgressCallback callback)
{
    this->callback = callback;
}

void progressmonitor::Start()
{
    current = 0;
    NotifyProgress();
}

void progressmonitor::Stop()
{
    NotifyProgress();
}

void progressmonitor::Update( const std::string& text,int amount)
{
    current+=amount;
    UIDEBUG("[%d] %s", current, text.c_str());
    NotifyProgress();
}

void progressmonitor::NotifyProgress()
{
    if ( callback != nullptr )
        callback(current);
}

