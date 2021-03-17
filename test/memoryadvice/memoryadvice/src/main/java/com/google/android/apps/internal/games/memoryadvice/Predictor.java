package com.google.android.apps.internal.games.memoryadvice;

import static com.google.android.apps.internal.games.memoryadvice.ByteArrayUtils.streamToDirectByteBuffer;
import static com.google.android.apps.internal.games.memoryadvice_common.StreamUtils.readStream;

import java.io.IOException;
import java.io.InputStream;
import java.nio.FloatBuffer;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.tensorflow.lite.Interpreter;

class Predictor {
  private final Interpreter interpreter;
  private final JSONArray features;

  /**
   * Construct a prediction service using the model found at the specified resource name and the
   * feature list found at the specified resurce name.
   * @param model The resource path of the TensorFlow Lite model.
   * @param features The resource path of the features.
   */
  Predictor(String model, String features) {
    try {
      this.features = new JSONArray(readStream(Predictor.class.getResourceAsStream(features)));
    } catch (IOException | JSONException e) {
      throw new IllegalStateException(e);
    }

    try (InputStream inputStream = Predictor.class.getResourceAsStream(model);) {
      interpreter = new Interpreter(streamToDirectByteBuffer(inputStream));
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }

  /**
   * Invoke the TensorFlow Lite model after extracting the features from the input data.
   * @param data The JSONObject containing the input set.
   * @return The prediction.
   */
  float predict(JSONObject data) {
    float[] featuresArray = new float[features.length()];
    for (int idx = 0; idx != features.length(); idx++) {
      try {
        String feature = features.getString(idx);
        Object o = JsonUtils.getFromPath(feature, data);
        float value;
        if (o instanceof Boolean) {
          value = (Boolean) o ? 1.0f : 0.0f;
        } else {
          value = ((Number) o).floatValue();
        }
        featuresArray[idx] = value;
      } catch (JSONException e) {
        throw new RuntimeException(e);
      }
    }

    FloatBuffer output = FloatBuffer.allocate(1);
    interpreter.run(featuresArray, output);
    return output.get(0);
  }
}
