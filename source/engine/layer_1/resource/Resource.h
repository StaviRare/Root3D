#pragma once

#include "Types.h"

struct Resource
{
    // hash, acts as id for resource
    hash id;
    
    // Backend object identifier
    uniqueID handle;     

    // Number of active references
    unsigned int useCount = 0;  
};