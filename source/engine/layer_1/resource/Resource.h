#pragma once

#include "Types.h"

struct Resource
{
    // Unique ID for lookup/deduplication
    hash hash;   
    
    // Backend-specific object (graphics, physics, etc.)
    void* handle;     

    // Number of references
    unsigned int useCount = 0;  
};