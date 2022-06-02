# Game Certification Operation Runner

The Game Certification Operation Runner is an android application which runs "operations" and gathers data as they run, for analysis after the fact.

---

## Notes on Structure

The application reads a configuration file at startup located at `app/src/main/res/raw/configuration.json` to enumerate "operations" to run. Operations are implemented in Java or CPP (via the cert-lib) and each is meant to perform a specific piece of functionality (performance testing, conformance testing, pass/fail).

Operations may require data to read or otherwise manipulate. In some cases this data is GLSL shader code, stored in `app/src/main/assets/Shaders` or textures stored in `app/src/main/assets/Textures`.

Note: the `JsonManipulatorOperation` has a JSON file `app/src/main/assets/JsonManipulatorOperation.json` made up of generated artifical data coarsely resembling a social media feed, which is meant to be fairly expensive to serialize/deserialize. Future operations which need some packaged structured data to act on may put it in this location if it makes sense to do so.
