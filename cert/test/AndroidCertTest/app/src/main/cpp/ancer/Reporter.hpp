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
    // Helper to make reporting easier.
    // reporter() reports the stored datum and, if it exists, calls
    // Reset(DatumType).
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

        Reporter(Reporter&&) = default;
        Reporter& operator = (Reporter&&) = default;


        template<typename T>
        void operator()(T&& payload) const {
            _op.ReportImpl(Json(std::forward<T>(payload)));
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
            _op.ReportImpl(Json(std::forward<T>(payload)));
        }
    private:
        const BaseOperation& _op;
    };
}

//==============================================================================

namespace ancer {
    // Helper for the extremely frequent 'report every X ms' pattern.
    template <typename ReportRate>
    [[nodiscard]] bool CheckReportRate(Timestamp& last_report, ReportRate rate) {
        const auto now = SteadyClock::now();
        if (last_report + rate < now) {
            // TODO(tmillican@google.com): Need to decide if we want to have
            //  reports be 'aligned' to rate or if it should be a minimum offset
            //  between reports. For now we're going with the latter.
            last_report = now + rate;
            return true;
        }
        return false;
    }
}
