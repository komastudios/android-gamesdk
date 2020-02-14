/*
 * Copyright 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <type_traits>

#include "BaseOperation.hpp"
#include "util/Time.hpp"


namespace ancer {
    // Helper to allow for easier reporting without passing BaseOperation around.
    // If a datum is attached, reporter() reports it and, if it exists, calls
    // Reset(datum).
    template<typename DatumType = void>
    class Reporter {
    public:
        template <typename = std::enable_if_t<std::is_default_constructible_v<DatumType>>>
        Reporter(const BaseOperation& op) : _op(op) {}

        template <typename... Args>
        Reporter(const BaseOperation& op, Args&&... args)
        : _op(op)
        , datum{std::forward<Args>(args)...} {
        }

        template <typename = std::enable_if_t<std::is_copy_constructible_v<DatumType>>>
        Reporter(const BaseOperation& op, const DatumType& d)
        : _op(op)
        , datum(d) {
        }

        template <typename = std::enable_if_t<std::is_move_constructible_v<DatumType>>>
        Reporter(const BaseOperation& op, DatumType&& d)
        : _op(op)
        , datum(std::move(d)) {
        }

        Reporter(const Reporter&) = default;
        Reporter(Reporter&&) = default;
        Reporter& operator = (const Reporter&) = default;
        Reporter& operator = (Reporter&&) = default;


        template<typename T>
        void operator()(T&& payload) const {
            _op.Report(std::forward<T>(payload));
        }

        void operator()() {
            (*this)(datum);
            TryReset();
        }

        DatumType datum;
    private:
        template <typename, typename = void> struct HasReset : std::false_type {};
        template <typename T>
        struct HasReset<T, std::void_t<decltype(Reset(std::declval<T&>()))>>
            : std::true_type {};

        void TryReset() {
            if constexpr (HasReset<DatumType>::value) {
                Reset(datum);
            }
        }

        const BaseOperation& _op;
    };

    // Specialization for datum-less reporting.
    template <>
    class Reporter<void> {
    public:
        Reporter(const BaseOperation& op) : _op(op) {}

        template<typename T>
        void operator()(T&& payload) const {
            _op.Report(std::forward<T>(payload));
        }
    private:
        const BaseOperation& _op;
    };
}
