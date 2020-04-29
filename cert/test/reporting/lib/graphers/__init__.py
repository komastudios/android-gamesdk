#
# Copyright 2019 The Android Open Source Project
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
"""The Graphers Module

All SuiteHandler implementations live here. Each must have its class
registered with the HANDLERS global.
"""

from .affinity_test_suite_handler import AffinityTestSuiteHandler
from .buffer_storage_suite_handler import BufferStorageSuiteHandler
from .calculate_wait_pi_suite_handler import CalculateWaitPiSuiteHandler
from .choreographer_timestamps_suite_handler import \
    ChoreographerTimestampsSuiteHandler
from .cpuset_suite_handler import CpusetSuiteHandler
from .dependent_read_suite_handler import DependentReadSuiteHandler
from .depth_clear_suite_handler import DepthClearSuiteHandler
from .egl_presentation_time_suite_handler import EGLPresentationTimeSuiteHandler
from .file_performance_suite_handler import FilePerformanceSuiteHandler
from .fill_rate_suite_handler import FillRateSuiteHandler
from .gpu_profiling_support_suite_handler import \
    GPUProfilingSupportSuiteHandler
from .half_float_precision import HalfFloatPrecisionSuiteHandler
from .marching_cubes_suite_handler import MarchingCubesSuiteHandler
from .mediump_vec_norm_suite_handler import MediumPVecNormSuiteHandler
from .memory_access.single_core_comparison import SingleCoreComparisonHandler
from .memory_allocation_suite_handler import MemoryAllocationSuiteHandler
from .mprotect_suite_handler import MProtectSuiteHandler
from .nougat_crash_suite_handler import NougatCrashSuiteHandler
from .temperature_suite_handler import TemperatureSuiteHandler
from .vertex_suite_handler import VertexRateSuiteHandler
from .vulkan_varyings_handler import VulkanVaryingsHandler

# Keep each suite handler alone in its line and we'll avoid zillions of really
# avoidable merge conflicts
HANDLERS = [
    AffinityTestSuiteHandler, \
    BufferStorageSuiteHandler, \
    CalculateWaitPiSuiteHandler, \
    ChoreographerTimestampsSuiteHandler, \
    CpusetSuiteHandler, \
    DependentReadSuiteHandler, \
    DepthClearSuiteHandler, \
    EGLPresentationTimeSuiteHandler, \
    FilePerformanceSuiteHandler, \
    FillRateSuiteHandler, \
    GPUProfilingSupportSuiteHandler, \
    HalfFloatPrecisionSuiteHandler, \
    MarchingCubesSuiteHandler, \
    MediumPVecNormSuiteHandler, \
    MemoryAllocationSuiteHandler, \
    MProtectSuiteHandler, \
    NougatCrashSuiteHandler, \
    SingleCoreComparisonHandler, \
    TemperatureSuiteHandler, \
    VertexRateSuiteHandler, \
    VulkanVaryingsHandler
]
"""List containing all registered SuiteHandler implementations to
render charts."""
