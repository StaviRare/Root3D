#pragma once

#include <vector>
#include <unordered_map>
#include "TypeInfo.h"
#include "FieldInfo.h"

class TypeRegistry
{
    public:
    static std::unordered_map<const char*, TypeInfo*>& GetTypes();
    static void RegisterType(TypeInfo* type);
};

#define OFFSET_OF(type, field) ((uint64) &(((type*)0)->field))

#define REGISTER_TYPE_BEGIN(typeName) \
    namespace { \
    struct typeName##_TypeRegister { \
        typeName##_TypeRegister() { \
            static TypeInfo info; \
            info.name = #typeName; \
            info.size = sizeof(typeName); \
            info.baseType = nullptr;

#define REGISTER_FIELD_TYPED(typeName, field, typeStr, typeCpp) \
            info.fields.push_back({ \
                #field, \
                typeStr, \
                OFFSET_OF(typeName, field), \
                [](void* obj, ISerializer& s) { s.WriteValue((typeCpp)((typeName*)obj)->field); }, \
                [](void* obj, IDeserializer& d) { d.ReadValue((typeCpp&)((typeName*)obj)->field); } \
            });

#define REGISTER_TYPE_END(typeName) \
            TypeRegistry::RegisterType(&info); \
        } \
    }; \
    static typeName##_TypeRegister _##typeName##_TypeRegister_Instance; \
    }
