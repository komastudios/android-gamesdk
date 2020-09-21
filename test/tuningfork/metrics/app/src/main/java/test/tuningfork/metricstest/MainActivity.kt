package test.tuningfork.metricstest

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Debug
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        info_text.text = measureMetricsPerformance(Debug::class.java)
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun measureMetricsPerformance(d: Class<Debug>): String

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}
