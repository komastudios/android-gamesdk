# Reporting

This folder contains scripts for running tests on local devices and devices on the "Firebase Test Lab" (FTL), and generating reports on the data.

## Dependencies

The reporting scripts require `python3.7x` and `pipenv` to manage dependencies.

``` bash
#(glinux) install dev dependencies for matplotlib
# see here for other platforms: https://pygobject.readthedocs.io/en/latest/getting_started.html
sudo apt install libgirepository1.0-dev gcc libcairo2-dev pkg-config python3-dev gir1.2-gtk-3.0

# install pipenv
pip3 install --user pipenv

# load the dependencies
pipenv install

# load the environment
pipenv shell
```

After installation, pipenv may warn that you need to add its path to your `$PATH` - if that's the case:

``` bash
# in your ~/.bashrc or preferred shell config:
export $PATH=$PATH:/~/.local/bin
```

Then run `pipenv install` from this directory to install dependencies. Finally, after doing so you can run `pipenv shell` to get a local virtualenv in your terminal, or if using VSCode or PyCharm, select the virtualenv matching this folder.

## Getting Started

The preferred environment for development is VS Studio Code. As such we have prepared a template settings file in `.vscode/settings.template.json`.

```bash
cp .vscode/settings.template.json .vscode/settings.json
```

Then, when starting VSCode in this directory, open a python file, and when VSCode asks for an environment to use, select the appropriate virtual env (it should have a name containing 'reporting')

NOTE: Because VSCode saves a user-specific python env path in the `settings.json` file, we can't simply commit a default `settings.json` to our repo.

---

## Running the pipeline

### `run.py`

``` bash
# this will run the recipe on all local devices as well as on FTL
python run.py --local --ftl --recipe path/to/recipe.yaml

# this will run the recipe on just local devices
python run.py --local --recipe path/to/recipe.yaml

```

The entrypoint for the pipeline is `run.py` , which consumes a "recipe" YAML file. Usage is simple: `python run.py --recipe whatever.yaml` . The recipe file structure is as follows:

NOTE: defaults are sourced from `recipes/defaults.yaml`, meaning a minimal valid recipe may simply consist of:
```yaml
build:
  configuration: path/to/configuration.json
```

``` yaml
build:
  # if true, the APK will be cleaned, then built
  clean: [true|false]
  # if true, build in release; this may require key signing configuration
  release: [true|false]
  # if provided, the configuration json will be inserted into the APK
  # this is a convenient way to set up a specific set of suites to run
  configuration: path/to/configuration.json

systrace:
  # if enabled, a systrace will be collected and merged into the output csv data
  enabled: [true|false]

  # systrace categories to record; at present only applies
  # to local deployments, not ftl
  categories: "sched freq idle am wm gfx view binder_driver hal dalvik input res"

  # only the systrace lines which contain the following keywords will be included
  keywords:

    - MonitorOperation

deployment:

  # rules for local (attached USB) deployment
  # optional; if left out no local run will be performed
  local:
    # optional, defaulting to true; if false, local deployment
    # will be skipped
    enabled: [true|false]

    # if true, the test will be run on all attached devices, this
    # overrides - device_ids: below
    all_attached_devices: false

    # if all_attached_devices is false, manually specify device ids to
    # run the test on. These IDs are sourced via `adb devices`
    device_ids:

      - 94CX1Z4AJ

    # tasks to run before test execution
    # NOTE: preflight tasks are only supported for local deployment
    # see recipes/~example-pre-post-flight.yaml for examples
    preflight:

        - {

          # copies ./tmp/foo.png to the app's private files/ folder
          action: copy,
          src: "${WORKSPACE_DIR}/tmp/foo.png",
          dst: "${APP_FILES_DIR}/foo.png",
        }

    # tasks to run after test execution
    # NOTE: postflight tasks are only supported for local deployment
    # see recipes/~example-pre-post-flight.yaml for examples
    postflight:

      - {

          # delete files from device (or locally, using ${WORKSPACE_DIR})
          action: delete,
          files: [
              # delete files from device
              "${APP_OOB_DATA_DIR}/big_file.apk",
            ],
        }

  # rules for testing on firebase test lab
  # optional; if left out no ftl run will be performed
  ftl:
    # optional, defaulting to true; if false, FTL deployment
    # will be skipped
    enabled: [true|false]

    # which test from -args: to execute; multiple can be defined
    # in -args:; if none specified, one will be selected from args
    # so this is only safely skippable if there's only one test
    # definition in -args:
    test: test-definition

    # specify certain devices to run on or leave field out to run on all
    # physical devices available to FTL
    devices:
      - { model: "flo", version: 21 }
      - { model: "mlv1", version: 23 }

    # flags: this maps directly to the `--flags-file` argument
    # when performing ftl deployment
    flags:
      # the gcloud/firebase project id
      project: android-games-device-research
      # format for logging
      format: json

    # args: this maps directly to the arguments yaml file provided
    # as final argument when performing an ftl deployment
    args:

      # this key "test-definition" is referenced above in -test:
      # you can name it whatever you want, but must reference it
      # in test if you specify more than one
      test-definition:
        type: game-loop
        timeout: 1500 # 25 minutes

# generation of summary report
summary:
  # if true, report will be generated
  enabled: [true|false]

  # format for report, presently only "md" and "html" are supported
  format: [md|html]

```

---

## Helper scripts

### `extract-report.sh`

``` bash

# if only one device is attached, extract its report and save it
# to the default file ./out/report.json
./extract-report.sh

# if only one device is attached, extract its report and save it
# to specified file
./extract-report.sh -o ~/Desktop/report.json

# if multiple devices are attached, extract the report from
# specified device, saving it to a specified file
./extract-report.sh -s 94CX1Z4AJ -o ~/Desktop/report.json
```

### `graph.py`

``` bash
# display a chart for the provided csv file
python graph.py --interactive path/to/csv.py

# generate an html report with images at 150 dpi using a folder full of reports
python graph.py --dpi 150 --html path/to/csvs

```

### `systrace.sh`

``` bash
# if only one device is attached, start systrace on it; it will run until
# you tap <enter>; saves trace to default location of ./out/trace.html
./systrace.sh

# if multiple devices are attached, start systrace on specified device.
# It will run unti lyou tap <enter>. Saves trace file to ~/Desktop/trace.html
./systrace.sh -s 94CX1Z4AJ -o ~/Desktop/trace.html
```

---

## The Report format

The reports generated by `AndroidCertTest` are "JSON-like", but not JSON. Each line of the report is a single valid compact (not-pretty-printed) single-line JSON blob.
The first line of the report is always the build info, which is simply a dict of build information key/value pairs, e.g.:

``` json
{"_d": {"_d": {"_d": {"_d": {"ID": "QP1A.191105.003", "DISPLAY": "QP1A.191105.003", "PRODUCT": "blueline", "DEVICE": "blueline", "BOARD": "blueline", "MANUFACTURER": "Google", "BRAND": "google", "MODEL": "Pixel 3", "BOOTLOADER": "b1c1-0.2-5672671", "HARDWARE": "blueline", "CODENAME": "REL", "INCREMENTAL": "5899767", "RELEASE": "10", "SDK_INT": 29, "FINGERPRINT": "google/blueline/blueline:10/QP1A.191105.003/5899767:user/release-keys", "OPENGLES": "3.2", "BASE_OS": "", "PREVIEW_SDK_INT": 0, "SECURITY_PATCH": "2019-11-05"}}}}}
```

Each subsequent line of the report is a "Datum", which is a standardized JSON payload describing:

* `suite_id` : The test suite that was executing when this datum was written
* `operation_id` : The operation that emitted the datum
* `cpu_id` : The CPU on which the datum's information was sourced
* `thread_id` : The thread on which the datum's information was sourced
* `timestamp` : The time in millis when the datum's information was sourced
* `custom` : A custom payload of domain-specific information for the operation

``` json
{"cpu_id":6,"custom":{},"operation_id":"MemoryAllocOperation","suite_id":"Memory allocation","thread_id":"539982699856","timestamp":108342777}
```

The `custom` field might look like so:

``` json
// "custom" field contents for MemoryAllocOperation
{"sys_mem_info":{"available_memory":2427056128,"low_memory":false,"native_allocated":24254280,"oom_score":16},"timestamp_millis":108342777,"total_allocation_bytes":10485760,"total_allocation_mb":10.0}
```

