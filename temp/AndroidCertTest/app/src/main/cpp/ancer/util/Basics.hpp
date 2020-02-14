#pragma once


namespace ancer {
    // Forces the compiler to make sure the given variables' states are calculated at a given point.
    // For CPU stress operations where the compiler can tell that the result is actually ignored.
    template <typename... Args>
    void ForceCompute(Args&&... args);


    template<typename T>
    [[nodiscard]] constexpr auto NextAlignedValue(T value, T align);
}

#include "Basics.inl"
