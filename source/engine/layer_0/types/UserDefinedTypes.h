#pragma once

#include <functional>

// Limiting to three arguments for clarity and simplicity. 
// For more complex data, prefer structs/classes.

// Static function (no captures)
using Func0Arg = void (*)();
template <typename T>
using Func1Arg = void (*)(T);
template <typename T1, typename T2>
using Func2Arg = void (*)(T1, T2);
template <typename T1, typename T2, typename T3>
using Func3Arg = void (*)(T1, T2, T3);

// Dynamic function (capture-supported)
using FuncDyn0 = std::function<void()>;
template <typename T>
using FuncDyn1 = std::function<void(T)>;
template <typename T1, typename T2>
using FuncDyn2 = std::function<void(T1, T2)>;
template <typename T1, typename T2, typename T3>
using FuncDyn3 = std::function<void(T1, T2, T3)>;