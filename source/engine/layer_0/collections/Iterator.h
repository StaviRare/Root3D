#pragma once

template <typename T>
class Iterator 
{
public:
    Iterator(T* ptr) : ptr(ptr) {}

    Iterator& operator++() 
    {
        ++ptr; return *this;
    }

    bool operator!=(const Iterator& other) const 
    {
        return ptr != other.ptr;
    }

    T& operator*() 
    {
        return *ptr;
    }

private:
    T* ptr;
};