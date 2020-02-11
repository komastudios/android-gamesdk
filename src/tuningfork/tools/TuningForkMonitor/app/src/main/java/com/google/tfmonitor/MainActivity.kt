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

import android.content.Context
import android.net.Uri
import android.os.AsyncTask
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import android.view.Menu
import android.view.MenuItem

import kotlinx.android.synthetic.main.activity_main.*
import java.io.File
import androidx.lifecycle.ViewModelProvider
import androidx.navigation.NavController
import androidx.navigation.findNavController
import com.google.tuningfork.AppKey
import com.google.tuningfork.Datastore
import com.google.tuningfork.Deserializer

private class AsyncWebServer(val applicationContext: Context, val port: Int, val requestListener: RequestListener) :
    AsyncTask<Unit, Unit, Unit>() {

    private var idir: File? = null;

    override fun onPreExecute() {
        super.onPreExecute()
        var tempDir = applicationContext.cacheDir;

        idir = File(tempDir.absolutePath + "/TuningForkMonitor/public_html");
        idir?.mkdirs();

        var ifile = File(idir?.absolutePath + "/index.html");
        ifile.writeText("<html><body>The TuningForkMonitor web server is running.</body></html>");

    }

    override fun doInBackground(vararg params: Unit?) {
        TuningForkRequestServer.startServer("localhost", port, requestListener)
    }
}

class MainActivity : AppCompatActivity(), AppChoiceFragment.OnFragmentInteractionListener,
    AppDetailFragment.OnFragmentInteractionListener,
    FidelityParametersFragment.OnFragmentInteractionListener,
    AnnotationsFragment.OnFragmentInteractionListener, RequestListener {

    private var navController: NavController? = null
    private lateinit var datastore : Datastore

    private fun getViewModel(): MonitorViewModel {
        val factory = ViewModelProvider.AndroidViewModelFactory.getInstance(application)
        return ViewModelProvider(viewModelStore, factory).get(MonitorViewModel::class.java)
    }

    override fun generateTuningParameters(appKey: AppKey, requestString: String): String {
        val deser = Deserializer()
        val req = deser.parseGenerateTuningParametersRequest(requestString)
        return getViewModel().generateTuningParameters(appKey, req)
    }

    override fun uploadTelemetry(appKey: AppKey, requetString: String): String {
        val deser = Deserializer()
        val req = deser.parseUploadTelemetryRequest(requetString)
        datastore.uploadTelemetryRequest(appKey, req)
        return getViewModel().uploadTelemetry(appKey, req)
    }

    override fun debugInfo(appKey: AppKey, requetString: String): String {
        val deser = Deserializer()
        val req = deser.parseDebugInfoRequest(requetString)
        datastore.debugInfoRequest(appKey, req)
        return getViewModel().debugInfo(appKey, req)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        setSupportActionBar(toolbar)

        datastore = Datastore(this.applicationContext)

        loadStoredApps()

        navController = findNavController(R.id.nav_controller_view_tag)

        AsyncWebServer(applicationContext, 9000, this).execute();

    }

    override fun onDestroy() {
        super.onDestroy()
        TuningForkRequestServer.stopServer();
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        // Inflate the menu; this adds items to the action bar if it is present.
        menuInflater.inflate(R.menu.menu_main, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.

        return when (item.itemId) {
            R.id.action_show_log -> {
                if (navController?.currentDestination?.id == R.id.log_fragment) {
                    navController?.popBackStack()
                } else {
                    navController?.navigate(R.id.log_fragment)
                }
                true
            }
            R.id.action_settings -> true
            else -> super.onOptionsItemSelected(item)
        }
    }

    override fun onFragmentInteraction(uri: Uri): Unit {
        System.out.println(uri.toString())
    }

    fun loadStoredApps() {
        val apps = datastore.getAllKnownApps()
        for (a in apps) {
            getViewModel().addAppMaybe(a, MonitorViewModel.AppType.STORED)
        }
    }

}
