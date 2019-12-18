#pragma once_size
#include "Basics.hpp"


namespace ancer::util_detail {
    void ForceCompute(const void*);
}


template <typename... Args>
void ancer::ForceCompute(Args&&... args) {
    (util_detail::ForceCompute(&args), ...);
}


template<typename T>
[[nodiscard]] constexpr auto ancer::NextAlignedValue(T value, T align) {
    const auto offset = align ? value % align : static_cast<T>(0);
    return value + (offset ? align - offset : static_cast<T>(0));
}
