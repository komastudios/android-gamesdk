package com.google.tfmonitor

import android.content.Context
import android.net.Uri
import android.os.AsyncTask
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import android.view.Menu
import android.view.MenuItem
import androidhttpweb.TinyWebServer

import kotlinx.android.synthetic.main.activity_main.*
import java.io.File
import androidx.lifecycle.ViewModelProvider
import androidx.navigation.NavController
import androidx.navigation.findNavController

private class AsyncWebServer(val applicationContext: Context, val port: Int) : AsyncTask<Unit,Unit,Unit>() {

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
        TinyWebServer.startServer("localhost", port, idir?.absolutePath);
    }
}

class MainActivity : AppCompatActivity(), AppChoiceFragment.OnFragmentInteractionListener,
    AppDetailFragment.OnFragmentInteractionListener,
    FidelityParametersFragment.OnFragmentInteractionListener,
    AnnotationsFragment.OnFragmentInteractionListener {

    private var navController: NavController? = null

    private fun getViewModel() : MonitorViewModel {
        val factory = ViewModelProvider.AndroidViewModelFactory.getInstance(application)
        return ViewModelProvider(viewModelStore, factory).get(MonitorViewModel::class.java)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        setSupportActionBar(toolbar)

        // Initialize Handler.
        createUpdateUiHandler();

        navController = findNavController(R.id.nav_controller_view_tag)

        AsyncWebServer(applicationContext, 9000).execute();

    }

    override fun onDestroy() {
        super.onDestroy()
        TinyWebServer.stopServer();
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
    /* Create Handler object in main thread. */
    private fun createUpdateUiHandler() {
        val model = getViewModel()
        RequestListener.setViewModel(model)
    }

    override fun onFragmentInteraction(uri: Uri): Unit {
        System.out.println(uri.toString())
    }

}
