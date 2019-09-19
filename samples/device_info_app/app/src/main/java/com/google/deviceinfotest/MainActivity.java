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

package com.google.deviceinfotest;

import android.os.Bundle;
import android.app.Activity;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.os.Build;

import com.google.androidgamesdk.GameSdkDeviceInfoJni;
import com.google.androidgamesdk.GameSdkDeviceInfoProto;
import com.google.protobuf.util.JsonFormat;

import java.lang.Math;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button sample_button = (Button) findViewById(R.id.sample_button);
        sample_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                TextView tv = (TextView) findViewById(R.id.sample_text);
                String msg = "Fingerprint(JAVA):\n" + Build.FINGERPRINT;
                try {
                    GameSdkDeviceInfoProto.GameSdkDeviceInfoWithErrors proto;
                    byte[] nativeBytes = GameSdkDeviceInfoJni.tryGetProtoSerialized();
                    proto = GameSdkDeviceInfoProto
                            .GameSdkDeviceInfoWithErrors.parseFrom(nativeBytes);

                    logLongProto(proto);

                    GameSdkDeviceInfoProto.GameSdkDeviceInfo info = proto.getInfo();
                    msg += "\nFingerprint(ro.build.fingerprint):\n" + info.getRoBuildFingerprint();
                    msg += "\nro_build_version_sdk:\n" + info.getRoBuildVersionSdk();

                    msg += "\n\nCompressed textures support:\n";
                    msg += "\nglCompressedRgbaAstc4X4Khr: "
                            + info.getOpenGl().getRenderedCompressedTextureFormats()
                            .getGlCompressedRgbaAstc4X4Khr();
                    msg += "\nglCompressedRgbaAstc6X6Khr: "
                            + info.getOpenGl().getRenderedCompressedTextureFormats()
                            .getGlCompressedRgbaAstc6X6Khr();
                    msg += "\nglCompressedRgbaAstc8X8Khr: "
                            + info.getOpenGl().getRenderedCompressedTextureFormats()
                            .getGlCompressedRgbaAstc8X8Khr();

                    msg += "\nglCompressedRgbPvrtc2Bppv1Img: "
                            + info.getOpenGl().getRenderedCompressedTextureFormats()
                            .getGlCompressedRgbPvrtc2Bppv1Img();
                    msg += "\nglCompressedRgbPvrtc4Bppv1Img: "
                            + info.getOpenGl().getRenderedCompressedTextureFormats()
                            .getGlCompressedRgbPvrtc4Bppv1Img();
                    msg += "\nglCompressedRgbaPvrtc2Bppv1Img: "
                            + info.getOpenGl().getRenderedCompressedTextureFormats()
                            .getGlCompressedRgbaPvrtc2Bppv1Img();
                    msg += "\nglCompressedRgbaPvrtc4Bppv1Img: "
                            + info.getOpenGl().getRenderedCompressedTextureFormats()
                            .getGlCompressedRgbaPvrtc4Bppv1Img();

                    msg += "\nglCompressedRgbaS3TcDxt1Ext: "
                            + info.getOpenGl().getRenderedCompressedTextureFormats()
                            .getGlCompressedRgbaS3TcDxt1Ext();
                    msg += "\nglCompressedRgbaS3TcDxt5Ext: "
                            + info.getOpenGl().getRenderedCompressedTextureFormats()
                            .getGlCompressedRgbaS3TcDxt5Ext();

                    msg += "\nglCompressedRgb8Etc2: "
                            + info.getOpenGl().getRenderedCompressedTextureFormats()
                            .getGlCompressedRgb8Etc2();
                    msg += "\nglEtc1Rgb8Oes: "
                            + info.getOpenGl().getRenderedCompressedTextureFormats()
                            .getGlEtc1Rgb8Oes();

                } catch (Exception e) {
                    msg += "\nException while creating the proto:" + e.getMessage();
                    android.util.Log.e("device_info", "could not create proto.", e);
                }
                tv.setText(msg);
            }
        });
    }

    // There's a limit on logcat line length (around 4 kB), so we have to break the proto into
    // smaller chunks
    private void logLongProto(GameSdkDeviceInfoProto.GameSdkDeviceInfoWithErrors proto)
            throws Exception {
        Integer CHUNK_SIZE = 3900;
        String LOG_TAG = "device_info_json_proto";

        JsonFormat.Printer printer =
            JsonFormat.printer()
            .preservingProtoFieldNames()
            .omittingInsignificantWhitespace();
        String jsonProto = printer.print(proto);

        if (jsonProto.length() > CHUNK_SIZE) {
            Integer chunkCount = (jsonProto.length() / CHUNK_SIZE) + 1;
            for (int currentChunk = 0; currentChunk < chunkCount; currentChunk++) {
                Integer startPosition = currentChunk * CHUNK_SIZE;
                Integer endPosition = Math.min((currentChunk + 1) * CHUNK_SIZE, jsonProto.length());
                String chunk = jsonProto.substring(startPosition, endPosition);
                String chunkHeader = String.format("[%d:%d] ", currentChunk + 1, chunkCount);
                android.util.Log.i(LOG_TAG, chunkHeader + chunk);
            }
        } else {
            android.util.Log.i(LOG_TAG, jsonProto);
        }
    }
}
