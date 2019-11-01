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

#include "TestResultPrinter.hpp"

#include <cstdio>

// TODO(tmillican@google.com): Investigate if this is the
// best way to reduce reimplementation
#include <gtest/internal/gtest-port.h>
#include <../src/gtest-internal-inl.h>

using namespace ancer;
using namespace testing;
namespace posix = testing::internal::posix;


//==================================================================================================

namespace {
    std::string FormatCountableNoun(
            int count, const char * singular_form, const char * plural_form) {
        return std::to_string(count) + " " +
                (count == 1 ? singular_form : plural_form);
    }

    std::string FormatTestCount(int test_count) {
        return FormatCountableNoun(test_count, "test", "tests");
    }

    std::string FormatTestSuiteCount(int test_suite_count) {
        return FormatCountableNoun(test_suite_count, "test suite", "test suites");
    }
}

//==================================================================================================

ResultPrinter::ResultPrinter(JNIEnv* env)
: _env(env) {
    // Get System.out
    jclass syscls = _env->FindClass("java/lang/System");
    jfieldID fid = _env->GetStaticFieldID(syscls, "out", "Ljava/io/PrintStream;");
    _out = _env->GetStaticObjectField(syscls, fid);
    // Get print() & flush()
    jclass pscls = _env->FindClass("java/io/PrintStream");
    _print = _env->GetMethodID(pscls, "print", "(Ljava/lang/String;)V");
    _flush = _env->GetMethodID(pscls, "flush", "()V");
}

//==================================================================================================

ResultPrinter::~ResultPrinter() {
    Flush();
}

//==================================================================================================

template <typename... Args>
void ResultPrinter::Print(const char* fmt, Args&&... args) {
    // TODO: Check snprintf result.
    char buf[512];
    if constexpr (sizeof...(args) == 0)
        snprintf(buf, std::size(buf), "%s", fmt);
    else
        snprintf(buf, std::size(buf), fmt, std::forward<Args>(args)...);

    _buffer.append(buf);
}

//==================================================================================================

namespace {
    bool ShouldUseColor(bool stdout_is_tty) {
        const auto color = std::string_view{GTEST_FLAG(color)};

        if ( color == "auto" ) {
            // TODO: Just defaulting to off for now
            return false;
        }
        return color == "yes" || color == "true" || color == "t" || color == "1";
    }
}

template <typename... Args>
void ResultPrinter::ColoredPrintf(PrintColor color, const char* fmt, Args&&... args) {
#if GTEST_OS_WINDOWS_MOBILE || GTEST_OS_ZOS || GTEST_OS_IOS || \
    GTEST_OS_WINDOWS_PHONE || GTEST_OS_WINDOWS_RT
    static constexpr bool use_color = false;
#else
    static const bool in_color_mode = ShouldUseColor(posix::IsATTY(posix::FileNo(stdout)) != 0);
    const bool use_color = in_color_mode && (color != PrintColor::Default);
#endif

#if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE && \
    !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT && !GTEST_OS_WINDOWS_MINGW
    static_assert(false); // Not ported over since it'll probably never happen.
#else
    if (!use_color) {
        Print(fmt, std::forward<Args>(args)...);
    } else {
        Print("\033[0;3%sm", std::to_string(static_cast<int>(color)).c_str());
        Print(fmt, std::forward<Args>(args)...);
        Print("\033[m");
    }
#endif
}

//==================================================================================================

void ResultPrinter::Flush() {
    if (_buffer.size()) {
        jstring str = _env->NewStringUTF(_buffer.c_str());
        _env->CallVoidMethod(_out, _print, str);
        _buffer.clear();
    }
    _env->CallVoidMethod(_out, _flush);
}

//==================================================================================================

// Fired before each iteration of tests starts.
void ResultPrinter::OnTestIterationStart(
        const UnitTest& unit_test, int iteration) {
    if (GTEST_FLAG(repeat) != 1)
        Print("\nRepeating all tests (iteration %d) . . .\n\n", iteration + 1);

    const char* const filter = GTEST_FLAG(filter).c_str();

    // Prints the filter if it's not *.  This reminds the user that some
    // tests may be skipped.
    if (!(std::string_view{filter} == "*")) {
        ColoredPrintf(PrintColor::Yellow,
                "Note: %s filter = %s\n", GTEST_NAME_, filter);
    }

    if (testing::internal::ShouldShard("GTEST_TOTAL_SHARDS", "GTEST_SHARD_INDEX", false)) {
        const auto shard_index = testing::internal::Int32FromEnvOrDie("GTEST_SHARD_INDEX", -1);
        ColoredPrintf(PrintColor::Yellow,
                "Note: This is test shard %d of %s.\n",
                static_cast<int>(shard_index) + 1,
                testing::internal::posix::GetEnv("GTEST_TOTAL_SHARDS"));
    }

    if (GTEST_FLAG(shuffle)) {
        ColoredPrintf(PrintColor::Yellow,
                "Note: Randomizing tests' orders with a seed of %d .\n",
                unit_test.random_seed());
    }

    ColoredPrintf(PrintColor::Green, "[==========] ");
    Print("Running %s from %s.\n",
            FormatTestCount(unit_test.test_to_run_count()).c_str(),
            FormatTestSuiteCount(unit_test.test_suite_to_run_count()).c_str());
    Flush();
}

//==================================================================================================

void ResultPrinter::OnEnvironmentsSetUpStart(
        const UnitTest& /*unit_test*/) {
    ColoredPrintf(PrintColor::Green, "[----------] ");
    Print("Global test environment set-up.\n");
    Flush();
}

//==================================================================================================

void ResultPrinter::OnTestCaseStart(const TestSuite& test_suite) {
    const std::string counts =
            FormatCountableNoun(test_suite.test_to_run_count(), "test", "tests");
    ColoredPrintf(PrintColor::Green, "[----------] ");
    Print("%s from %s", counts.c_str(), test_suite.name());
    if (test_suite.type_param() == nullptr) {
        Print("\n");
    } else {
        Print(", where %s = %s\n", "TypeParam", test_suite.type_param());
    }
    Flush();
}

//==================================================================================================

void ResultPrinter::OnTestStart(const TestInfo& test_info) {
    ColoredPrintf(PrintColor::Green, "[ RUN      ] ");
    PrintTestName(test_info.test_suite_name(), test_info.name());
    Print("\n");
    Flush();
}

//==================================================================================================

namespace {
    const char* TestPartResultTypeToString(TestPartResult::Type type) {
        switch (type) {
        case TestPartResult::kSkip:
            return "Skipped";
        case TestPartResult::kSuccess:
            return "Success";
        case TestPartResult::kNonFatalFailure:
        case TestPartResult::kFatalFailure:
            return "Failure\n";
        default:
            return "Unknown result type";
        }
    }
}

// Called after an assertion failure.
void ResultPrinter::OnTestPartResult(
        const TestPartResult& result) {
    switch (result.type()) {
        // If the test part succeeded, or was skipped,
        // we don't need to do anything.
    case TestPartResult::kSkip:
    case TestPartResult::kSuccess:
        return;
    default:
        // Print failure message from the assertion
        // (e.g. expected this and got that).
        Print("%s :%d: ", result.file_name(), result.line_number());
        // '\n' is appended on failure for the message to follow.
        Print("%s", TestPartResultTypeToString(result.type()));
        Print("%s\n", result.message());
    }
}

//==================================================================================================

void ResultPrinter::OnTestEnd(const TestInfo& test_info) {
    if (test_info.result()->Passed()) {
        ColoredPrintf(PrintColor::Green, "[       OK ] ");
    } else if (test_info.result()->Skipped()) {
        ColoredPrintf(PrintColor::Green, "[  SKIPPED ] ");
    } else {
        ColoredPrintf(PrintColor::Red, "[  FAILED  ] ");
    }
    PrintTestName(test_info.test_suite_name(), test_info.name());
    if (test_info.result()->Failed())
        PrintFullTestCommentIfPresent(test_info);

    if (GTEST_FLAG(print_time)) {
        Print(" (%s ms)\n", std::to_string(test_info.result()->elapsed_time()).c_str());
    } else {
        Print("\n");
    }
    Flush();
}

//==================================================================================================

void ResultPrinter::OnTestCaseEnd(const TestSuite& test_suite) {
    if (!GTEST_FLAG(print_time)) return;

    const std::string counts =
            FormatCountableNoun(test_suite.test_to_run_count(), "test", "tests");
    ColoredPrintf(PrintColor::Green, "[----------] ");
    Print("%s from %s (%s ms total)\n\n", counts.c_str(), test_suite.name(),
            std::to_string(test_suite.elapsed_time()).c_str());
    Flush();
}

//==================================================================================================

void ResultPrinter::OnEnvironmentsTearDownStart(
        const UnitTest& /*unit_test*/) {
    ColoredPrintf(PrintColor::Green, "[----------] ");
    Print("Global test environment tear-down\n");
    Flush();
}

//==================================================================================================

void ResultPrinter::OnTestIterationEnd(const UnitTest& unit_test, int /*iteration*/) {
    ColoredPrintf(PrintColor::Green, "[==========] ");
    Print("%s from %s ran.",
            FormatTestCount(unit_test.test_to_run_count()).c_str(),
            FormatTestSuiteCount(unit_test.test_suite_to_run_count()).c_str());
    if (GTEST_FLAG(print_time)) {
        Print(" (%s ms total)",
                std::to_string(unit_test.elapsed_time()).c_str());
    }
    Print("\n");
    ColoredPrintf(PrintColor::Green, "[  PASSED  ] ");
    Print("%s.\n", FormatTestCount(unit_test.successful_test_count()).c_str());

    const int skipped_test_count = unit_test.skipped_test_count();
    if (skipped_test_count > 0) {
        ColoredPrintf(PrintColor::Green, "[  SKIPPED ] ");
        Print("%s, listed below:\n", FormatTestCount(skipped_test_count).c_str());
        PrintSkippedTests(unit_test);
    }

    int num_failures = unit_test.failed_test_count();
    if (!unit_test.Passed()) {
        const int failed_test_count = unit_test.failed_test_count();
        ColoredPrintf(PrintColor::Red, "[  FAILED  ] ");
        Print("%s, listed below:\n", FormatTestCount(failed_test_count).c_str());
        PrintFailedTests(unit_test);
        Print("\n%2d FAILED %s\n", num_failures,
                num_failures == 1 ? "TEST" : "TESTS");
    }

    int num_disabled = unit_test.reportable_disabled_test_count();
    if (num_disabled && !GTEST_FLAG(also_run_disabled_tests)) {
        if (!num_failures) {
            Print("\n");  // Add a spacer if no FAILURE banner is displayed.
        }
        ColoredPrintf(PrintColor::Yellow,
                "  YOU HAVE %d DISABLED %s\n\n",
                num_disabled,
                num_disabled == 1 ? "TEST" : "TESTS");
    }
    // Ensure that Google Test output is printed before, e.g., heapchecker output.
    Flush();
}

//==================================================================================================

void ResultPrinter::PrintTestName(const char* test_suite, const char* test) {
    Print("%s.%s", test_suite, test);
}

//==================================================================================================

// Internal helper for printing the list of failed tests.
void ResultPrinter::PrintFailedTests(const UnitTest& unit_test) {
    const int failed_test_count = unit_test.failed_test_count();
    if (failed_test_count == 0) {
        return;
    }

    for (int i = 0; i < unit_test.total_test_suite_count(); ++i) {
        const TestSuite& test_suite = *unit_test.GetTestSuite(i);
        if (!test_suite.should_run() || (test_suite.failed_test_count() == 0)) {
            continue;
        }
        for (int j = 0; j < test_suite.total_test_count(); ++j) {
            const TestInfo& test_info = *test_suite.GetTestInfo(j);
            if (!test_info.should_run() || !test_info.result()->Failed()) {
                continue;
            }
            ColoredPrintf(PrintColor::Red, "[  FAILED  ] ");
            Print("%s.%s", test_suite.name(), test_info.name());
            PrintFullTestCommentIfPresent(test_info);
            Print("\n");
        }
    }
}

//==================================================================================================

// Internal helper for printing the list of skipped tests.
void ResultPrinter::PrintSkippedTests(const UnitTest& unit_test) {
    const int skipped_test_count = unit_test.skipped_test_count();
    if (skipped_test_count == 0) {
        return;
    }

    for (int i = 0; i < unit_test.total_test_suite_count(); ++i) {
        const TestSuite& test_suite = *unit_test.GetTestSuite(i);
        if (!test_suite.should_run() || (test_suite.skipped_test_count() == 0)) {
            continue;
        }
        for (int j = 0; j < test_suite.total_test_count(); ++j) {
            const TestInfo& test_info = *test_suite.GetTestInfo(j);
            if (!test_info.should_run() || !test_info.result()->Skipped()) {
                continue;
            }
            ColoredPrintf(PrintColor::Green, "[  SKIPPED ] ");
            Print("%s.%s", test_suite.name(), test_info.name());
            Print("\n");
        }
    }
}

//==================================================================================================

void ResultPrinter::PrintFullTestCommentIfPresent(const TestInfo& test_info) {
    const char* const type_param = test_info.type_param();
    const char* const value_param = test_info.value_param();

    if (type_param != nullptr || value_param != nullptr) {
        Print(", where ");
        if (type_param != nullptr) {
            Print("%s = %s", "TypeParam", type_param);
            if (value_param != nullptr) printf(" and ");
        }
        if (value_param != nullptr) {
            Print("%s = %s", "GetParam()", value_param);
        }
    }
}
