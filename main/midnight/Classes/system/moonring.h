//
//  moonring.h
//  midnight
//
//  "It will give you the Power of Command and the Power of Vision over those lords and subjects
//  loyal to you, even at great distances. With the Power of Vision you will be able to see through
//  their eyes what they see. With the Power of Command you will be able to urge them to undertake
//  any task they would willingly perform for you. And more than this, it will echo the warmth and
//  strength of your mind and send forth a tide of hope across the cold lands of Midnight.
//  It is yours. Take it, and use it with care."
//
//  Created by Chris Wild on 07/12/2017.
//

#pragma once

#include "config.h"

class moonring {
private:
    moonring() {};
public:
    
    static moonring* instance(){
        static moonring test;
        return &test;
    }
    
public:
    
    config      settings;

    
};


