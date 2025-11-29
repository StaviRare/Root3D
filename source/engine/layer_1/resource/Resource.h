#pragma once

#include "Types.h"

struct Resource
{
    // Hash of the resource content for deduplication
    hash hash;   
    
    // Backend object identifier
    uniqueID handle;     

    // Number of active references
    unsigned int useCount = 0;  
};