package com.google.tfmonitor

import androidhttpweb.LogHandler
import com.google.tuningfork.AppKey
import com.google.tuningfork.Deserializer

object RequestListener {

    private var logHandler : LogHandler? = null

    private var viewModel : MonitorViewModel? = null;

    fun generateTuningParameters(app_key: AppKey, request_string: String) : String {
        val deser = Deserializer()
        val req = deser.parseGenerateTuningParametersRequest(request_string)
        logHandler?.log(LogHandler.Level.INFO,
            app_key.name + " " + app_key.version + " generateTuningParameters")
        return viewModel?.generateTuningParameters(app_key, req) ?: ""
    }
    fun uploadTelemetry(app_key: AppKey, request_string: String) : String {
        val deser = Deserializer()
        val req = deser.parseUploadTelemetryRequest(request_string)
        logHandler?.log(LogHandler.Level.INFO,
            app_key.name + " " + app_key.version + " uploadTelemetry")
        return viewModel?.uploadTelemetry(app_key, req) ?: ""
    }

    fun debugInfo(app_key: AppKey, request_string: String) : String {
        val deser = Deserializer()
        val req = deser.parseDebugInfoRequest(request_string)
        logHandler?.log(LogHandler.Level.INFO,
            app_key.name + " " + app_key.version + " debugInfo")
        return viewModel?.debugInfo(app_key, req) ?: ""
    }
    fun setViewModel(model: MonitorViewModel) {
        viewModel = model
    }

}
