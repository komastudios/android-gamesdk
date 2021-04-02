package com.google.android.apps.internal.games.memoryadvice;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.core.IsNull.notNullValue;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;
import com.google.android.apps.internal.games.memoryadvice_common.Schemas;
import com.google.common.collect.ImmutableMap;
import java.util.Map;
import net.jimblackler.jsonschemafriend.GenerationException;
import net.jimblackler.jsonschemafriend.Schema;
import net.jimblackler.jsonschemafriend.SchemaException;
import net.jimblackler.jsonschemafriend.SchemaStore;
import net.jimblackler.jsonschemafriend.ValidationException;
import net.jimblackler.jsonschemafriend.Validator;
import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class GetMemoryMetricsTest {
  private final Validator validator;
  private final Schema monitorParametersSchema;

  public GetMemoryMetricsTest() throws GenerationException {
    SchemaStore schemaStore = new SchemaStore();
    monitorParametersSchema =
        schemaStore.loadSchema(Schemas.getSchema("monitorParameters.schema.json"));
    validator = new Validator();
  }

  @Test
  public void debugStats() throws SchemaException {
    ImmutableMap<Object, Object> variable = ImmutableMap.of("debug", true);
    Map<String, Object> metrics = getMetrics(variable);
    Map<String, Object> debug = (Map<String, Object>) metrics.get("debug");
    assertThat(debug, notNullValue());
    assertThat(debug.get("NativeHeapAllocatedSize"), notNullValue());
  }

  private Map<String, Object> getMetrics(ImmutableMap<Object, Object> variable)
      throws ValidationException {
    ImmutableMap<String, Object> metrics =
        ImmutableMap.of("variable", variable, "baseline", ImmutableMap.of());
    validator.validate(monitorParametersSchema, metrics);
    MemoryMonitor memoryMonitor =
        new MemoryMonitor(InstrumentationRegistry.getInstrumentation().getTargetContext(), metrics);
    return memoryMonitor.getMemoryMetrics();
  }
}