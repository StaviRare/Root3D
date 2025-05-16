#include "TypeRegistry.h"

std::unordered_map<const char*, TypeInfo*>& TypeRegistry::GetTypes()
{
    static std::unordered_map<const char*, TypeInfo*> types;
    return types;
}

void TypeRegistry::RegisterType(TypeInfo* type)
{
    GetTypes()[type->name] = type;
}
