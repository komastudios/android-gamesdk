/*
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

#ifndef TUNINGFORK_TUNINGFORK_C_H
#define TUNINGFORK_TUNINGFORK_C_H

#include <stdint.h>

// These are reserved instrumentation keys
enum {
    TFTICK_SYSCPU = 0,
    TFTICK_SYSGPU = 1
};

typedef struct {
  uint8_t* bytes;
  size_t size;
  void (*dealloc)(void*);
} CProtobufSerialization;

// The instrumentation key identifies a tick point within a frame or a trace segment
typedef uint16_t InstrumentationKey;
typedef uint64_t TraceHandle;
typedef uint64_t TimePoint;
typedef uint64_t Duration;

#ifdef __cplusplus
extern "C" {
#endif

// init must be called before any other functions
//  If no backend is passed, a debug version is used which returns empty fidelity params
//   and outputs histograms in protobuf text format to logcat.
//  If no timeProvider is passed, std::chrono::steady_clock is used.
void TFInit(const CProtobufSerialization *settings);

// Blocking call to get fidelity parameters from the server.
// Returns true if parameters could be downloaded within the timeout, false otherwise.
// Note that once fidelity parameters are downloaded, any timing information is recorded
//  as being associated with those parameters.
bool TFGetFidelityParameters(CProtobufSerialization *params, size_t timeout_ms);

// Protobuf serialization of the current annotation
// Returns 0 if the annotation could be set, -1 if not
int TFSetCurrentAnnotation(const CProtobufSerialization *annotation);

// Record a frame tick that will be associated with the instrumentation key and the current
//   annotation
void TFFrameTick(InstrumentationKey id);

// Record a frame tick using an external time, rather than system time
void TFFrameDeltaTimeNanos(InstrumentationKey id, Duration dt);

// Start a trace segment
TraceHandle TFStartTrace(InstrumentationKey key);

// Record a trace with the key and annotation set using startTrace
void TFEndTrace(TraceHandle h);

#ifdef __cplusplus
}
#endif

#endif
