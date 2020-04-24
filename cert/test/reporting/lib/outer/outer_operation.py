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
"""While the majority of the tests run from an app inside the device, there
might be a few that need to run from the outside. At the time of this writing,
F2.2 test "Support for GPU profiling tools" is an example.

These tests have specific needs that can't be satisfied or accessed from inside
the device. Instead, these needs must be addressed from the outside (typically,
from the PC or Mac where the device is attached and accessible via adb).

At least for now, these tests only run locally, not remote like in FLT or the
mobile harness (MH).

OuterOperation is a Python base class for these tests, intended to emulate what
the traditional app operations (the ones written in C++ and Java) produce: a
list of JSON reports that we can later graph through suite handlers.
"""

from abc import ABC, abstractmethod
from datetime import datetime
import importlib
import json
from pathlib import Path
from typing import Any, Dict, Optional, TypeVar

from lib.configuration import Configuration
from lib.device import Device


class OuterOperation(ABC):
    """Base class for outer, standalone operations.
    """

    def __init__(self):
        self.__id: str = None
        self.__suite_id: str = None
        self.__description: str = None
        self.__device: Device = None
        self.__report: Path = None
        self.__file_descriptor = None
        self.__issue_id = 0

    @property
    def id(self):
        """Operation id getter."""
        return self.__id

    @id.setter
    def id(self, operation_id: str):
        """Operation id setter."""
        self.__id = operation_id

    @property
    def suite_id(self):
        """Suite id getter."""
        return self.__suite_id

    @suite_id.setter
    def suite_id(self, suite_id: str):
        """Suite id setter."""
        self.__suite_id = suite_id

    @property
    def description(self):
        """Suite description getter."""
        return self.__description

    @description.setter
    def description(self, description: str):
        """Suite description setter."""
        self.__description = description

    @property
    def has_run(self) -> bool:
        """True if method run() was called at least once to this operation."""
        return self.device is not None

    @property
    def device_serial(self) -> Optional[str]:
        """Serial number of the device this operation runs for."""
        return self.device.serial if self.device else None

    @property
    def device(self) -> Optional[Device]:
        """Returns the device this operation runs for."""
        return self.__device

    @property
    def report(self) -> Path:
        """Path to the produced report."""
        return self.__report

    def run(self, device_serial: str, out_dir: Path) -> None:
        """If ready to run, it starts the operation.
        Args:
            device_serial: serial number of the device this operation is for.
            out_dir: Path where results are to be left.
        """
        if self.device:
            raise RuntimeError(f'{self.id} instance can run only ' 'once.')
        self.__device = Device(device_serial)
        try:
            self.__report = Path(
                f'{out_dir}/{self.device_serial}-00-local_report.json')
            self.__file_descriptor = self.initiate_report()
            self.start()
        finally:
            if self.__file_descriptor:
                self.__file_descriptor.close()
                self.__file_descriptor = None

    def initiate_report(self):
        """Opens the operation report for writing and initializes its first
        line."""
        file_descriptor = open(self.report, 'w')

        build_info: Dict[str, Any] = {}
        build_info['ID'] = self.device.id
        build_info['DISPLAY'] = self.device.display
        build_info['PRODUCT'] = self.device.product
        build_info['DEVICE'] = self.device.device
        build_info['BOARD'] = self.device.board
        build_info['MANUFACTURER'] = self.device.manufacturer
        build_info['BRAND'] = self.device.brand
        build_info['MODEL'] = self.device.model
        build_info['BOOTLOADER'] = self.device.bootloader
        build_info['HARDWARE'] = self.device.hardware
        build_info['CODENAME'] = self.device.codename
        build_info['INCREMENTAL'] = self.device.incremental
        build_info['RELEASE'] = self.device.release
        build_info['SDK_INT'] = self.device.sdk_int
        build_info['FINGERPRINT'] = self.device.fingerprint
        build_info['BASE_OS'] = self.device.base_os
        build_info['PREVIEW_SDK_INT'] = self.device.preview_sdk_int
        build_info['SECURITY_PATCH'] = self.device.security_patch

        file_descriptor.write(json.dumps(build_info, separators=(',', ':')))
        file_descriptor.write('\n')

        return file_descriptor

    def _report(self, datum: Any):
        """Dumps a JSON line to the report file with all relevant execution
        data and the custom information."""
        line = {}
        line['issue_id'] = self.__issue_id
        line['timestamp'] = int(datetime.now().timestamp() * 1000)
        line['thread_id'] = 0
        line['cpu_id'] = 0
        line['suite_id'] = self.suite_id
        line['operation_id'] = self.id
        line['custom'] = datum

        self.__file_descriptor.write(json.dumps(line, separators=(',', ':')))
        self.__file_descriptor.write('\n')

        self.__issue_id += 1

    @abstractmethod
    def start(self) -> None:
        """Triggers this operation test.
        """

    @classmethod
    def availability_error_message(cls) -> str:
        """Empty string if the outer operation can run in this system.
        Otherwise, the reason why the operation cannot run on this system.
        """
        return ''

    @classmethod
    def load(cls, operation_id: str, suite_id: str,
             description: str) -> TypeVar('OuterOperation'):
        """Creates an instance of an outer operation."""
        operation_class = getattr(importlib.import_module('lib.outer'),
                                  operation_id)
        error_message = operation_class.availability_error_message()
        if error_message:
            raise EnvironmentError(error_message)

        operation_instance = operation_class()
        operation_instance.id = operation_id
        operation_instance.suite_id = suite_id
        operation_instance.description = description

        return operation_instance

    @classmethod
    def load_from_configuration(cls, configuration: Configuration
                               ) -> TypeVar('OuterOperation'):
        """Creates an instance of an outer operation."""
        return cls.load(configuration.lookup('data_gatherer.id'),
                        configuration.lookup('name'),
                        configuration.lookup('description'))
