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

All SuiteSummarizer implementations live here. Each must have its class
registered with the SUMMARIZERS global.
"""

from .affinity_test_suite_handler import AffinityTestSummarizer
from .buffer_storage_suite_handler import BufferStorageSuiteSummarizer
from .calculate_wait_pi_suite_handler import CalculateWaitPiSummarizer
from .choreographer_timestamps_suite_handler import \
    ChoreographerTimestampsSummarizer
from .cpuset_suite_handler import CpusetSuiteSummarizer
from .dependent_read_suite_handler import DependentReadSummarizer
from .depth_clear_suite_handler import DepthClearSummarizer
from .egl_presentation_time_suite_handler import EGLPresentationTimeSummarizer
from .external_framebuffer import ExternalFramebufferSummarizer
from .file_performance_suite_handler import FilePerformanceSummarizer
from .fill_rate_suite_handler import FillRateSummarizer
from .gpu_profiling_support_suite_handler import \
    GPUProfilingSupportSummarizer
from .marching_cubes_suite_handler import MarchingCubesSummarizer
from .mediump_vec_norm_suite_handler import MediumPVecNormSummarizer
from .memory_access.single_core_comparison import SingleCoreComparisonSummarizer
from .memory_allocation_suite_handler import MemoryAllocationSuiteSummarizer
from .mprotect_suite_handler import MProtectSummarizer
from .nougat_crash_suite_handler import NougatCrashSummarizer
from .temperature_suite_handler import TemperatureSuiteSummarizer
from .vertex_suite_handler import VertexRateSuiteSummarizer
from .vulkan_varyings_handler import VulkanVaryingsSummarizer

# Keep each suite summarizer alone in its line and we'll avoid zillions
# of really avoidable merge conflicts.
SUMMARIZERS = [
    AffinityTestSummarizer, \
    BufferStorageSuiteSummarizer, \
    CalculateWaitPiSummarizer, \
    ChoreographerTimestampsSummarizer, \
    CpusetSuiteSummarizer, \
    DependentReadSummarizer, \
    DepthClearSummarizer, \
    EGLPresentationTimeSummarizer, \
    ExternalFramebufferSummarizer, \
    FilePerformanceSummarizer, \
    FillRateSummarizer, \
    GPUProfilingSupportSummarizer, \
    MarchingCubesSummarizer, \
    MediumPVecNormSummarizer, \
    MemoryAllocationSuiteSummarizer, \
    MProtectSummarizer, \
    NougatCrashSummarizer, \
    SingleCoreComparisonSummarizer, \
    TemperatureSuiteSummarizer, \
    VertexRateSuiteSummarizer, \
    VulkanVaryingsSummarizer
]
"""List containing all registered SuiteHandler implementations
to render summaries."""
