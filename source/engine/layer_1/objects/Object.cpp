#include "Object.h"
#include "ObjectManager.h"

Object::Object() : m_name("Object")
{
    m_id = ObjectManager::GenerateId();
}

Object::Object(const string& name) : m_name(name)
{
    m_id = ObjectManager::GenerateId();
}

Object::~Object()
{

}

uniqueID Object::GetId() const
{
    uniqueID returnValue = 0;
    returnValue = m_id;
    return returnValue;
}

const string &Object::GetName() const
{
    return m_name;
}

void Object::SetName(const string &name)
{
    m_name = name;
}