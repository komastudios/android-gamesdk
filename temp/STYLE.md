# Style Guide
**Rule #1: Readability is king.** Feel free to ignore any/all of this if it makes the code easier to read & decipher.
**Rule #2: Don't be Jarring.** Always try to remain consistent with the surrounding code. e.g., If a section of code uses a different naming scheme, be consistent with that when you're working with that code.
## General Formatting
The general formatting rules (whitespace, names, etc.) are semi-automatically handled by Android Studio. You can see these rules in File->Settings->Code Style.

If needed, automatic formatting can be enabled/disabled using:
```
// @formatter:off
...
// @formatter:on
```
## Specific C++ Features
* Use [[nodiscard]], noexcept, etc. whenever possible. These help to express intent,
  avoid bugs, and can even help performance.
* We have no strong preference regarding AAA (almost always auto). Just be consistent with surrounding code.
# Code Organization
## Includes
Includes should be sorted alphabetically and split into the following blocks:
```
#include "This.hpp" // The corresponding header (if we're a source file).

// Standard headers
#include <memory>
#include <vector>

// External library headers
#include <EGL/egl.h>
#include <nlohmann/json.hpp>

// 'External' project headers
// If you need to go "../" to use the relative path, it goes here.
#include <ancer/System.hpp>

// 'Internal' project headers
#include "WorkerThread.hpp"
```
## Namespaces
The namespace hierarchy and filesystem should generally match each other: If you have enough code to justify a namespace or folder, you probably have enough code to justify the other as well.
In source files, `using namespace XXX;` should be the first line after the includes. Note that this requires definitions be qualified if they're going to match any in-header declarations. (This mostly only matters for free functions.)
```
//===========================================================

namespace foo {
   void Func(int); // Header declaration
}

//===========================================================

using namespace foo;

namespace foo {
    // Oops! Signature mismatch declared a new function.
    void Func(double) { ... }
}

// Oops! Declared a new function in the global namespace.
void Func(int) { ... }

// Good! Forces us to match a pre-existing declaration.
void foo::Func(int) { ... }
```
## Blocking Out Files
In larger source files, split code into 'blocks' using page divisions:
```
#include <...>

using namespace foo;


//===========================================================

void foo::DoThing() {
    ...
}

//===========================================================

namespace {
    void Helper() { ... }
}

void foo::DoSomethingElse() {
    Helper();
}
```
Note that a double-split or "light" split (`//----`) are also options.
The idea is to make it so the eye naturally navigates with minimal mental overhead: Too many blocks is just as bad as a wall of undivided text. As a general rule of thumb, most functions should probably have a full division between them, with trivial-but-related functions maybe going in the same block.
