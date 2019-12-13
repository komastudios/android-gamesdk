/*
 * The MIT License
 *
 * Copyright 2018 Sonu Auti http://sonuauti.com twitter @SonuAuti
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
package appapis.queryfiles;

import android.util.Log;

import java.util.HashMap;

import com.google.tfmonitor.RequestListener;
import com.google.tuningfork.AppKey;

public class AppApis {

    static final String TAG = "AppApis";

    public AppApis(){
    }

    public String generateTuningParameters(HashMap qparms) {
        AppKey app = AppKey.Companion.parse((String)qparms.get("PATH"));
        Log.i(TAG, "generateTuningParameters  (" + app.getName() + " / " + app.getVersion() + ")");
        return RequestListener.INSTANCE.generateTuningParameters(app, (String)qparms.get("_POST"));
    }
    public String uploadTelemetry(HashMap qparms) {
        AppKey app = AppKey.Companion.parse((String)qparms.get("PATH"));
        Log.i(TAG, "uploadTelemetry  (" + app.getName() + " / " + app.getVersion() + ")");
        return RequestListener.INSTANCE.uploadTelemetry(app, (String)qparms.get("_POST"));
    }
    public String debugInfo(HashMap qparms) {
        AppKey app = AppKey.Companion.parse((String)qparms.get("PATH"));
        Log.i(TAG, "uploadTelemetry  (" + app.getName() + " / " + app.getVersion() + ")");
        return RequestListener.INSTANCE.debugInfo(app, (String)qparms.get("_POST"));
    }
}