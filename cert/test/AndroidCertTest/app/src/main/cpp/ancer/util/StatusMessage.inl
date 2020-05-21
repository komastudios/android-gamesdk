/*
 * Copyright 2020 The Android Open Source Project
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

#ifndef _STATUS_MESSAGE
#define _STATUS_MESSAGE

#include <string>

namespace ancer {
/**
 * A message box for the UI to get news from the native side.
 */
class StatusMessageManager {
 public:
  /**
   * True if there's a new message posted since last time retrieved. The intention is to avoid
   * expensive string operations if the last status hasn't changed.
   */
  bool IsNewStatusAvailable() const {
    return _is_new;
  }

  /**
   * Retrieves the current status and turns the new-status flag off.
   */
  const std::string &GetValue() {
    _is_new = false;
    return _value;
  }

  /**
   * Sets a new status as current and turns the new-status flag on.
   */
  void SetValue(const std::string &value) {
    _value = value;
    _is_new = true;
  }

 private:
  bool _is_new{false};
  std::string _value;
};
}

#endif  // _STATUS_MESSAGE