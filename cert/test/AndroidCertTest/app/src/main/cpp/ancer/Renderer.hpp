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

#pragma once

#include <memory>

namespace ancer {
    class BaseOperation;
}


namespace ancer {
    class Renderer {
    public:
        virtual ~Renderer() {};

        virtual void AddOperation(BaseOperation& operation) = 0;
        virtual void RemoveOperation(BaseOperation& operation) = 0;
        virtual void ClearOperations() = 0;

        // Where 'T' is something that can be converted to unique_ptr<Renderer>:
        // static T Create(...);
    };
}
