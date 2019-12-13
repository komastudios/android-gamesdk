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

package com.google.tfmonitor

import com.google.tuningfork.AppKey
import com.google.tuningfork.Deserializer

object RequestListener {

    private var viewModel : MonitorViewModel? = null;

    fun generateTuningParameters(app_key: AppKey, request_string: String) : String {
        val deser = Deserializer()
        val req = deser.parseGenerateTuningParametersRequest(request_string)
        return viewModel?.generateTuningParameters(app_key, req) ?: ""
    }
    fun uploadTelemetry(app_key: AppKey, request_string: String) : String {
        val deser = Deserializer()
        val req = deser.parseUploadTelemetryRequest(request_string)
        return viewModel?.uploadTelemetry(app_key, req) ?: ""
    }

    fun debugInfo(app_key: AppKey, request_string: String) : String {
        val deser = Deserializer()
        val req = deser.parseDebugInfoRequest(request_string)
        return viewModel?.debugInfo(app_key, req) ?: ""
    }
    fun setViewModel(model: MonitorViewModel) {
        viewModel = model
    }

}
