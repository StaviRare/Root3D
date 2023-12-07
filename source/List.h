#pragma once

// Header-only implementation to allow template instantiation with different types
// at compile time, avoiding the need for explicit template instantiation.

#include "Iterator.h"
#include "Debug.h"

template <typename T>
class List {
public:
    List() : data(nullptr), size(0), capacity(0) {}

    // Copy constructor for deep copy
    List(const List& other) : data(nullptr), size(0), capacity(0) 
    {
        if (other.size > 0) 
        {
            data = new T[other.capacity];

            for (size_t i = 0; i < other.size; ++i) 
            {
                data[i] = other.data[i];
            }

            size = other.size;
            capacity = other.capacity;
        }
    }

    // Copy assignment operator
    List& operator=(const List& other) 
    {
        if (this != &other) 
        {
            T* newData = new T[other.capacity];

            for (size_t i = 0; i < other.size; ++i) 
            {
                newData[i] = other.data[i];
            }

            delete[] data;
            data = newData;
            size = other.size;
            capacity = other.capacity;
        }

        return *this;
    }

    ~List() 
    {
        delete[] data;
    }

    void Add(const T& item) 
    {
        if (size >= capacity) 
        {
            Reserve(capacity == 0 ? 1 : capacity * 2);
        }

        data[size++] = item;
    }

    // Add elements from an array
    template <size_t N>
    void AddRange(const T(&arr)[N]) 
    {
        for (size_t i = 0; i < N; ++i) 
        {
            Add(arr[i]);
        }
    }

    T& operator[](size_t index) 
    {
        T& returnValue = dummy;

        if (index < size) 
        {
            returnValue = data[index];
        }
        else 
        {
            LogOutOfBoundsError(index);
        }

        return returnValue;
    }

    const T& operator[](size_t index) const {
        const T& returnValue = dummy;

        if (index < size) 
        {
            returnValue = data[index];
        }
        else 
        {
            LogOutOfBoundsError(index);
        }

        return returnValue;
    }

    size_t Count() const 
    {
        return size;
    }

    void Clear() 
    {
        delete[] data;
        data = nullptr;
        size = 0;
        capacity = 0;
    }

    Iterator<T> begin() const 
    {
        return Iterator<T>(data);
    }

    Iterator<T> end() const 
    {
        return Iterator<T>(data + size);
    }

private:
    T dummy;
    T* data;
    size_t size;
    size_t capacity;

    void Reserve(size_t newCapacity) 
    {
        T* newData = new T[newCapacity];

        for (size_t i = 0; i < size; ++i) 
        {
            newData[i] = data[i];
        }

        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

    void LogOutOfBoundsError(int index)
    {
        std::string errorMessage = "Index " + std::to_string(index) + " out of bounds";
        errorMessage += " of an object with max length " + std::to_string(size) + ".";
        Debug::error(errorMessage);
    }
};