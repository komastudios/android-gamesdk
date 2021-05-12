package com.google.android.apps.internal.games.memoryadvice;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;
import com.google.android.apps.internal.games.memoryadvice_common.Schemas;
import com.google.common.collect.ImmutableMap;
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
}