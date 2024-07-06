#pragma once

// Limiting to three arguments for clarity and simplicity. 
// For more complex data, prefer structs/classes.

using FuncNoArg = void (*)();

template <typename T>
using Func1Arg = void (*)(T);

template <typename T1, typename T2>
using Func2Arg = void (*)(T1, T2);

template <typename T1, typename T2, typename T3>
using Func3Arg = void (*)(T1, T2, T3);