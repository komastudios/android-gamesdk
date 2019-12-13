package com.google.tfmonitor

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import java.sql.Timestamp

import com.google.protobuf.DescriptorProtos.FileDescriptorSet
import com.google.tuningfork.*
import com.google.tuningfork.Annotation

class MonitorViewModel : ViewModel() {

    private var activeApp : AppKey? = null

    enum class AppType { LIVE, STORED}
    private val liveApplications = MutableLiveData<List<AppKey>>()
    private val storedApplications = MutableLiveData<List<AppKey>>()

    private val applicationData = mutableMapOf<AppKey, MutableLiveData<AppData>>()

    private val log = MutableLiveData<String>()
    val logBuilder = StringBuilder()

    init {
        // Trigger user load
    }

    fun getLiveApps(): LiveData<List<AppKey>> {
        return liveApplications
    }
    fun getStoredApps(): LiveData<List<AppKey>> {
        return storedApplications
    }
    fun getActiveAppData() : LiveData<AppData>? {
        if (activeApp == null)
            return null
        else
            return applicationData.get(activeApp!!)
    }
    fun applicationsForType(type: AppType) : MutableLiveData<List<AppKey>> {
        when (type) {
            AppType.LIVE -> return liveApplications
            AppType.STORED -> return storedApplications
        }
    }
    fun setActiveApp(app: AppKey?) {
        activeApp = app
    }
    fun getActiveApp() = activeApp

    fun addAppMaybe(a:AppKey, type: AppType) {
        val applications = applicationsForType(type)
        if (! (applications.value?.contains(a) ?: false)) {
            val apps = applications.value?.toMutableList() ?: mutableListOf<AppKey>()
            apps.add(a)
            applications.postValue(apps)
        }
    }

    fun addAppData(a: AppKey, d: AppData) {
        if (!applicationData.containsKey(a)) {
            applicationData[a] = MutableLiveData<AppData>()
        }
        applicationData[a]?.postValue(d)
    }

    fun getLog(): LiveData<String> {
        System.out.println(log.value)
        return log
    }
    fun addLogLine(line: String) {
        logBuilder.appendln(Timestamp(System.currentTimeMillis()).toString() + " " + line)
        val s = logBuilder.toString()
        System.out.println(s)
        log.postValue(s) // only works on background thread
    }

    fun generateTuningParameters(app: AppKey, req: GenerateTuningParametersRequest) : String {
        val app = AppKey.parse(req.name)
        addLogLine(app.name + " " + app.version + " generateTuningParameters")
        addAppMaybe(app, AppType.LIVE)
        return ""
    }

    fun uploadTelemetry(app: AppKey, req: UploadTelemetryRequest) : String {
        val app = AppKey.parse(req.name)
        addLogLine(app.name + " " + app.version + " uploadTelemetry")
        addAppMaybe(app, AppType.LIVE)
        return ""
    }

    fun debugInfo(app: AppKey, req: DebugInfoRequest) : String {
        if (req.dev_tuningfork_descriptor!=null) {
            val desc = deserializeFileDescriptor(req.dev_tuningfork_descriptor)
            val settings = Tuningfork.Settings.parseFrom(req.settings)
            addAppData(app, AppData(desc, settings))
        }
        return ""
    }

    fun deserializeFileDescriptor(pbytes: ByteArray) : DeserializedTuningForkDescriptor {
        val p = FileDescriptorSet.parseFrom(pbytes)
        var fidelityParameters = mutableListOf<FidelityParameter>()
        var annotations = mutableListOf<com.google.tuningfork.Annotation>()
        var enums = mutableMapOf<String, List<String>>()
        var package_name = ""
        for (i in 0 until p.fileCount) {
            val f = p.getFile(i)
            if (f.name == "dev_tuningfork.proto") {
                package_name = f.`package`
                for (j in 0 until f.enumTypeCount) {
                    val e = f.getEnumType(j)
                    val enum_fields = mutableListOf<String>()
                    for (k in 0 until e.valueCount) {
                        enum_fields.add(e.getValue(k).name)
                    }
                    val fullName = "." + f.`package` + "." + e.name
                    enums[fullName] = enum_fields
                }
                for (j in 0 until f.messageTypeCount) {
                    val m = f.getMessageType(j)
                    if (m.name=="FidelityParams") {
                        for(k in 0 until m.fieldCount) {
                            val field =m.getField(k)
                            fidelityParameters.add(FidelityParameter(field.name,
                                field.type.toString(), field.defaultValue))
                        }
                    }
                    if (m.name=="Annotation") {
                        for(k in 0 until m.fieldCount) {
                            val field =m.getField(k)
                            annotations.add(Annotation(field.name,field.typeName, 0))
                        }
                    }
                }
            }
        }
        return DeserializedTuningForkDescriptor(package_name, enums, fidelityParameters, annotations)
    }

}
