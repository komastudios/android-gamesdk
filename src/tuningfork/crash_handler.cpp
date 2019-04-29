/*
 * Copyright 2018 The Android Open Source Project
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

#include <pthread.h>
#include "crash_handler.h"
#include "Log.h"
#include <vector>

#define LOG_TAG "Tuningfork Crash Handler"

namespace tuningfork{

std::vector<int> signals{
    SIGILL  /*4*/,
    SIGTRAP /*5*/,
    SIGABRT /*6*/,
    SIGBUS  /*7*/,
    SIGFPE  /*8*/,
    SIGSEGV /*11*/
};
pthread_mutex_t handler_mutex = PTHREAD_MUTEX_INITIALIZER;
bool handlers_installed = false;

void CrashHandler::Init() {
    pthread_mutex_lock(&handler_mutex);
    if(InstallHandler()) {
        ALOGI("%s", "Crash Handler is installed");
    }
    pthread_mutex_unlock(&handler_mutex);
}

bool CrashHandler::InstallHandler() {
    if (handlers_installed) return false;

    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = Handler;
    sa.sa_flags = SA_ONSTACK | SA_SIGINFO;

    for(int signal : signals) {
        if (sigaction(signal, &sa, NULL) == -1) {
            return -1;
        }
    }

    handlers_installed = true;
    return true;
}

void CrashHandler::Handler(int sig, siginfo_t* info, void* ucontext) {
    ALOGI("Crash handler with (sig : %d)", sig);
}
}  // namespace




