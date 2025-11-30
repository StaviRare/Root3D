#pragma once
#include "Types.h"

class Object
{
    public:
    Object();
    explicit Object(const string& name);
    ~Object();

    uniqueID GetId() const;
    const string& GetName() const;
    void SetName(const string& name);

    protected:
    uniqueID m_id; // change to instance_id
    string m_name;
};
