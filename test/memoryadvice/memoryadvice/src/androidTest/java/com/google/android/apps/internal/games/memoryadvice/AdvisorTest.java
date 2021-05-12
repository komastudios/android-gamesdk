package com.google.android.apps.internal.games.memoryadvice;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.is;

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
public class AdvisorTest {
  private final Validator validator;
  private final Schema advisorParametersSchema;

  public AdvisorTest() throws GenerationException {
    SchemaStore schemaStore = new SchemaStore();
    advisorParametersSchema =
        schemaStore.loadSchema(Schemas.getSchema("advisorParameters.schema.json"));
    validator = new Validator();
  }

  @Test
  public void intitialize() throws SchemaException {
    ImmutableMap<String, Object> params =
        ImmutableMap.of("metrics", ImmutableMap.of("baseline", ImmutableMap.of()));
    validator.validate(advisorParametersSchema, params);
    MemoryAdvisor memoryAdvisor =
        new MemoryAdvisor(InstrumentationRegistry.getInstrumentation().getTargetContext(), params);
  }

  @Test
  public void predictRealtime() throws SchemaException {
    ImmutableMap<String, Object> params = ImmutableMap.of("metrics",
        ImmutableMap.of("baseline", ImmutableMap.of("debug", true, "meminfo", true, "status", true),
            "variable", ImmutableMap.of("predictRealtime", true, "debug", true, "proc", true)));
    validator.validate(advisorParametersSchema, params);
    MemoryAdvisor memoryAdvisor =
        new MemoryAdvisor(InstrumentationRegistry.getInstrumentation().getTargetContext(), params);
    Map<String, Object> advice = memoryAdvisor.getAdvice();
    Map<String, Object> metrics = (Map<String, Object>) advice.get("metrics");
    assertThat(metrics.containsKey("predictedUsage"), is(true));
  }

  @Test
  public void availableRealtime() throws SchemaException {
    ImmutableMap<String, Object> params = ImmutableMap.of("metrics",
        ImmutableMap.of("constant",
            ImmutableMap.of("ActivityManager", true, "MemoryInfo", true, "meminfo", true),
            "baseline",
            ImmutableMap.of(
                "debug", true, "meminfo", true, "proc", true, "status", true, "MemoryInfo", true),
            "variable", ImmutableMap.of("availableRealtime", true, "debug", true, "proc", true)));
    validator.validate(advisorParametersSchema, params);
    MemoryAdvisor memoryAdvisor =
        new MemoryAdvisor(InstrumentationRegistry.getInstrumentation().getTargetContext(), params);
    Map<String, Object> advice = memoryAdvisor.getAdvice();
    Map<String, Object> metrics = (Map<String, Object>) advice.get("metrics");
    assertThat(metrics.containsKey("predictedAvailable"), is(true));
  }
}