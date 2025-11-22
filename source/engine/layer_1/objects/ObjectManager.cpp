#include "ObjectManager.h"
#include "Random.h"

uniqueID ObjectManager::GenerateId()
{
    uniqueID returnValue = 0;
    uniqueID a = static_cast<uniqueID>(Random::Range(0, 0xFFFF));
    uniqueID b = static_cast<uniqueID>(Random::Range(0, 0xFFFF));
    uniqueID c = static_cast<uniqueID>(Random::Range(0, 0xFFFF));
    uniqueID d = static_cast<uniqueID>(Random::Range(0, 0xFFFF));
    returnValue = (a << 48) | (b << 32) | (c << 16) | d;
    return returnValue;
}
