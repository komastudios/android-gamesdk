package com.google.android.apps.internal.games.memoryadvice;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.is;
import static org.hamcrest.Matchers.lessThan;
import static org.hamcrest.Matchers.notNullValue;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;
import com.google.android.apps.internal.games.memoryadvice_common.Schemas;
import com.google.common.collect.ImmutableMap;
import java.util.Map;
import net.jimblackler.jsonschemafriend.GenerationException;
import net.jimblackler.jsonschemafriend.Schema;
import net.jimblackler.jsonschemafriend.SchemaException;
import net.jimblackler.jsonschemafriend.SchemaStore;
import net.jimblackler.jsonschemafriend.Validator;
import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class MonitorTest {
  private final Validator validator;
  private final Schema monitorParametersSchema;
  private final Schema metricsResultsSchema;

  public MonitorTest() throws GenerationException {
    SchemaStore schemaStore = new SchemaStore();
    monitorParametersSchema =
        schemaStore.loadSchema(Schemas.getSchema("monitorParameters.schema.json"));
    metricsResultsSchema = schemaStore.loadSchema(Schemas.getSchema("metricsResults.schema.json"));
    validator = new Validator();
  }

  @Test
  public void nullFields() throws SchemaException {
    ImmutableMap<String, Object> params = ImmutableMap.of();
    validator.validate(monitorParametersSchema, params);
    MemoryMonitor memoryMonitor =
        new MemoryMonitor(InstrumentationRegistry.getInstrumentation().getTargetContext(), null);

    Map<String, Object> memoryMetrics = memoryMonitor.getMemoryMetrics(null);
    validator.validate(metricsResultsSchema, memoryMetrics);

    // The only data we are guaranteed to get back is the sample time.
    Map<String, Object> meta = (Map<String, Object>) memoryMetrics.get("meta");
    assertThat(meta, notNullValue());
    assertThat(meta.containsKey("time"), is(true));
  }
}