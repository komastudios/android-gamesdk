#
# Copyright 2020 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
"""Google is working with partners to enable GPU profiling through a common
front-end called Android GPU inspector (AGI, https://gpuinspector.dev/).
AGI runs from an attached PC or Mac, and can access the device to instrument
and collect GPU performance data. For that to happen, the GPU driver provided
by the GPU manufacturer must include some bits that interoperate with AGI
during the instrumentation. Hence, these bits support GPU profiling via AGI.
This test verifies whether such supporting bits are available. It requires
Android GPU Inspector (AGI) installed and in the environment path. For example:

export PATH=${PATH}:/opt/agi

At the time of this writing, AGI is in incubation phase. Consequently, it can
be installed only from https://github.com/google/agi-dev-releases/releases

Input configuration: None.

Output report:
- supported: True, False.
"""

from lib.common import run_command_with_stdout
from .outer_operation import OuterOperation


class GPUProfilingSupportOperation(OuterOperation):
    """Implements operation GPU Profiling Tools Support."""

    @classmethod
    def availability_error_message(cls) -> str:
        stdout = run_command_with_stdout(f'which gapit', False)
        if not stdout:
            return 'Android GPU Inspector not found on path. Is it installed?'

    def start(self):
        datum = {'supported': True}
        stdout = run_command_with_stdout(
            'gapit validate_gpu_profiling -os android '
            f'-serial {self.device_serial} ', False)
        for line in stdout:
            if 'No GPU profiling capabilities found on device' in line:
                datum['supported'] = False

        self._report(datum)
