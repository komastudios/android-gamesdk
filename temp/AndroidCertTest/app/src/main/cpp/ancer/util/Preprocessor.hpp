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


#define ANCER_CONCAT(x, y) ANCER_CONCAT_( x, y )
#define ANCER_CONCAT_(x, y) x##y

// Gives an unique & anonymous name for a local variable. Primarily useful for
// RAII objects.
#define ANCER_ANON_LOCAL ANCER_CONCAT(_anon_local_,__COUNTER__)
