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

#include <gtest/gtest.h>
#include <jni.h>


namespace ancer {
    // Shameless copy of gtest's PrettyUnitTestResultPrinter tweaked for our environment.
    class ResultPrinter : public testing::TestEventListener {
    public:
        enum class PrintColor {
            Default = 0,
            Red = 1,
            Green = 2,
            Yellow = 3,
        };

        using TestInfo = testing::TestInfo;
        using TestPartResult = testing::TestPartResult;
        using TestSuite = testing::TestSuite;
        using UnitTest = testing::UnitTest;

        ResultPrinter(JNIEnv* env);
        ~ResultPrinter();

        template <typename... Args>
        void Print(const char* fmt, Args&&... args);
        template <typename... Args>
        void ColoredPrintf(PrintColor color, const char* fmt, Args&&... args);
        void Flush();

        void OnTestProgramStart(const UnitTest& /*unit_test*/) override {}
        void OnTestIterationStart(const UnitTest& unit_test, int iteration) override;
        void OnEnvironmentsSetUpStart(const UnitTest& unit_test) override;
        void OnEnvironmentsSetUpEnd(const UnitTest& /*unit_test*/) override {}
        void OnTestCaseStart(const TestSuite& test_suite) override;
        void OnTestStart(const TestInfo& test_info) override;
        void OnTestPartResult(const TestPartResult& result) override;
        void OnTestEnd(const TestInfo& test_info) override;
        void OnTestCaseEnd(const TestSuite& test_suite) override;
        void OnEnvironmentsTearDownStart(const UnitTest& unit_test) override;
        void OnEnvironmentsTearDownEnd(const UnitTest& /*unit_test*/) override {}
        void OnTestIterationEnd(const UnitTest& unit_test, int iteration) override;
        void OnTestProgramEnd(const UnitTest& /*unit_test*/) override {}

    private:
        JNIEnv* _env;
        jobject _out;
        jmethodID _print;
        jmethodID _flush;

        // We save things in a buffer to the side so we can flush output all-at-once.
        // This is to avoid lines being interrupted by other output in Logcat.
        std::string _buffer;

        void PrintTestName(const char* test_suite, const char* test);
        void PrintFailedTests(const UnitTest& unit_test);
        void PrintSkippedTests(const UnitTest& unit_test);
        void PrintFullTestCommentIfPresent(const TestInfo& test_info);
    };
}