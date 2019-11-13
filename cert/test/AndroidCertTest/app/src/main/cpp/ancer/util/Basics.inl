#pragma once
#include "Basics.hpp"


namespace ancer::util_detail {
    void ForceCompute(void*);
}


template <typename... Args>
void ancer::ForceCompute(Args&&... args) {
    (util_detail::ForceCompute(&args), ...);
}
