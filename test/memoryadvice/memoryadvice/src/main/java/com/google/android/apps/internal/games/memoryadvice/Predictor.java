package com.google.android.apps.internal.games.memoryadvice;

import static com.google.android.apps.internal.games.memoryadvice.ByteArrayUtils.streamToDirectByteBuffer;
import static com.google.android.apps.internal.games.memoryadvice.JsonUtils.getFromPath;
import static com.google.android.apps.internal.games.memoryadvice_common.StreamUtils.readStream;

import com.fasterxml.jackson.databind.ObjectMapper;
import java.io.IOException;
import java.io.InputStream;
import java.nio.FloatBuffer;
import java.util.List;
import java.util.Map;
import org.tensorflow.lite.Interpreter;

class Predictor {
  private final Interpreter interpreter;
  private final List<String> features;

  /**
   * Construct a prediction service using the model found at the specified resource name and the
   * feature list found at the specified resurce name.
   * @param model The resource path of the TensorFlow Lite model.
   * @param features The resource path of the features.
   */
  Predictor(String model, String features) {
    try {
      this.features = new ObjectMapper().reader().readValue(
          Predictor.class.getResourceAsStream(features), List.class);
    } catch (IOException e) {
      throw new IllegalStateException(e);
    }

    try (InputStream inputStream = Predictor.class.getResourceAsStream(model)) {
      interpreter = new Interpreter(streamToDirectByteBuffer(inputStream));
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }

  /**
   * Invoke the TensorFlow Lite model after extracting the features from the input data.
   * @param data The map containing the input set.
   * @return The prediction.
   */
  float predict(Map<String, Object> data) throws MissingPathException {
    float[] featuresArray = new float[features.size()];
    for (int idx = 0; idx != features.size(); idx++) {
      String feature = features.get(idx);
      Object o = getFromPath(feature, data);
      float value;
      if (o instanceof Boolean) {
        value = (Boolean) o ? 1.0f : 0.0f;
      } else {
        value = ((Number) o).floatValue();
      }
      featuresArray[idx] = value;
    }

    FloatBuffer output = FloatBuffer.allocate(1);
    interpreter.run(featuresArray, output);
    return output.get(0);
  }
}
