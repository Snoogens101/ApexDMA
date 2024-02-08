#pragma once
#include <chrono>
#include <functional>

// Profiling utility function
template<typename Func>
void ProfileOperation(Func operation, std::chrono::microseconds& elapsedTime) {
    auto start = std::chrono::high_resolution_clock::now();
    operation(); // Execute the passed function
    auto end = std::chrono::high_resolution_clock::now();
    elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}
