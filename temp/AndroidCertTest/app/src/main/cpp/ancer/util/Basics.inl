#pragma once
#include "Basics.hpp"


namespace ancer::util_detail {
    void ForceCompute(void*);
}


template <typename... Args>
void ancer::ForceCompute(Args&&... args) {
    (util_detail::ForceCompute(&args), ...);
}


template<typename T>
[[nodiscard]] constexpr auto ancer::NextAlignedValue(T value, T align) {
    const auto offset = align ? value % align : 0;
    return value + (offset ? align - offset : 0);
}
