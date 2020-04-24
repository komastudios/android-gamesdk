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
"""A catalog of device data indexed by codename.
"""

import re
from typing import Any, Dict, Iterable, List, Optional, Union

from lib.common import run_command_with_stdout


class DeviceInfo:
    """Consolidates info related to a specific device."""

    def __init__(self,
                 codename: str,
                 brand: str,
                 model: str,
                 sdk_version: Union[str, int] = "",
                 tags: List[str] = None):
        self.__codename = codename.strip()

        brand = brand.strip()
        self.__brand = f"{brand[:1].capitalize()}{brand[1:]}"

        model = model.strip()
        self.__model = f"{model[:1].capitalize()}{model[1:]}"

        # A common vicious with devices is the reiteration of the device brand
        # in the model. For example, the brand is "Samsung" and model is
        # "Samsung Galaxy XX". When that's the case, we force model to be just
        # "Galaxy XX" so we can safely report brand and model.
        if len(self.__model) > len(self.__brand) and \
            self.__model.lower().startswith(f"{self.__brand.lower()} "):
            self.__model = self.__model[len(self.__brand) + 1:]

        self.__sdk_version = sdk_version

        self.__tags = tags if tags else []

        if (len(self.__codename) == 0 or len(self.__brand) == 0 or
                len(self.__model) == 0):
            raise ValueError(f'DeviceInfo codename="{self.__codename}" '
                             f'brand="{self.__brand}" model="{self.__model}" '
                             f"can't have empty properties")

    @property
    def codename(self) -> str:
        """Codename getter."""
        return self.__codename

    @property
    def brand(self) -> str:
        """Brand getter."""
        return self.__brand

    @property
    def model(self) -> str:
        """Model getter."""
        return self.__model

    @property
    def sdk_version(self) -> str:
        """API Level supported by the device model."""
        return self.__sdk_version

    @property
    def tags(self) -> List[str]:
        """List of firebase metadata tags about device"""
        return self.__tags

    def has_tag(self, tag: str) -> bool:
        """Return true if this device has a particular tag in its tags list"""
        return tag in self.__tags

    def __lt__(self, other: Any) -> bool:
        """Less-than lexicographic order on brand and model."""
        return self.__brand < other.brand or \
            (self.__brand == other.brand and self.__model < other.model)

    def __eq__(self, other: Any) -> bool:
        """Equal-to on brand and model."""
        return self.__brand == other.brand and self.__model == other.model

    def __le__(self, other: Any) -> bool:
        """Less-or-equal lexicographic order on brand and model."""
        return self < other or self == other

    def __ne__(self, other: Any) -> bool:
        """Not-equal-to on brand and model."""
        return not self == other

    def __gt__(self, other: Any) -> bool:
        """Greater-than lexicographic order on brand and model."""
        return other < self

    def __ge__(self, other: Any) -> bool:
        """Greater-or-equal lexicographic order on brand and model."""
        return other <= self

    def __repr__(self) -> str:
        """String representation of this type."""
        return f"{self.__brand} {self.__model} " \
            f"({self.__codename}) SDK {self.__sdk_version}"


#------------------------------------------------------------------------------


class DeviceCatalog:
    """Read-only dictionary that retrieves DeviceInfo items based on codename.
    Entries can be added only once. The key is a codename and the value a
    DeviceInfo (which in turns contains device brand and model).
    Its intention is twofold:

    * To have a way to easily map device codenames to brand and model.
    * To have a way to easily retrieve the whole list of devices sorted by
    * brand and model.
    """

    class DeviceCatalogImpl:
        """Inner class that implements the catalog logic."""

        def __init__(self):
            self.__store: Dict[str, DeviceInfo] = {}

        def push(self, device: DeviceInfo) -> bool:
            """Adds a device to the catalog (that must not contain an entry for
            that codename already).

            Args:
                device: a DeviceInfo.

            Returns: true if the device was added. False if the device codename
                     was already in.
            """
            if device.codename not in self.__store:
                self.__store[device.codename] = device
                return True
            return False

        def __getitem__(self, codename: str) -> Optional[DeviceInfo]:
            """Index function. Favors syntax like

            device_catalog.push(DeviceInfo("1", "Brand1", "Model1"))
            device_catalog["1"]  # returns device Brand1/Model1
            device_catalog["2"]  # returns None without raising exception

            Returns: None if codename isn't in the catalog. Otherwise the
                    corresponding DeviceInfo.
            """
            return self.__store.get(codename, None)

        def __contains__(self, codename: str) -> bool:
            """Favors syntax like

            device_catalog.push(DeviceInfo("1", "Brand1", "Model1"))
            if "1" in device_catalog:
                print("Found")

            Returns: True if the codename is a key in the catalog.
            """
            return codename in self.__store

        def __iter__(self) -> Iterable[DeviceInfo]:
            """Returns an iterable of DeviceInfo, sorted by brand and model.
            More importantly, this favors syntax like

            device_catalog.push(DeviceInfo("1", "YYY", "Model Y"))
            device_catalog.push(DeviceInfo("2", "ZZZ", "Model Z"))
            device_catalog.push(DeviceInfo("3", "XXX", "Model X"))
            for device in device_catalog:
                ...  # retrieves, in order, brands "XXX", "YYY", "ZZZ"
            """
            return iter(sorted(self.__store.values()))

        def __len__(self) -> int:
            """Returns the number of DeviceInfo entries in the catalog."""
            return len(self.__store)

        def clear(self) -> type(None):
            """Gets the store emptied."""
            self.__store.clear()

    __instance = None

    def __new__(cls):
        """Constructs a single device catalog instance."""
        if DeviceCatalog.__instance is None:
            DeviceCatalog.__instance = DeviceCatalog.DeviceCatalogImpl()
        return DeviceCatalog.__instance

    def __getattr__(self, name):
        """Forwars any attribute access from the catalog class to its single
        instance."""
        return getattr(DeviceCatalog.__instance, name)

    @classmethod
    def dispose(cls) -> type(None):
        """Recycles the singleton instance."""
        if DeviceCatalog.__instance is not None:
            DeviceCatalog.__instance.clear()
        DeviceCatalog.__instance = None


#------------------------------------------------------------------------------


class Device:
    """Models a physical, unique device. Not a generic device model but a
    concrete device with serial number."""

    def __init__(self, serial_number: str):
        self.__serial = serial_number
        self.__props = self.__getprops()

    def __getprops(self) -> Dict[str, str]:
        """Runs a shell command to capture all device props. The device must be
        attached to the host running this script."""
        stdout = run_command_with_stdout(
            f'adb -s {self.serial} shell getprop')

        props = {}
        for line in stdout:
            prop_parts = re.match(r"^\[(.+)\]: \[(.*)\]$", line)
            if prop_parts and len(prop_parts.groups()) == 2:
                props[prop_parts.group(1)] = prop_parts.group(2)

        return props

    @property
    def id(self):
        """Id."""
        return self.getprop('ro.build.id')

    @property
    def serial(self):
        """Serial number."""
        return self.__serial

    @property
    def display(self):
        """Display."""
        return self.getprop('ro.build.display.id')

    @property
    def product(self):
        """Product."""
        return self.getprop('ro.product.name')

    @property
    def device(self):
        """Device code."""
        return self.getprop('ro.product.device')

    @property
    def board(self):
        """Board."""
        return self.getprop('ro.product.board')

    @property
    def manufacturer(self):
        """Manufacturer."""
        return self.getprop('ro.product.manufacturer')

    @property
    def brand(self):
        """Brand."""
        return self.getprop('ro.product.brand')

    @property
    def model(self):
        """Model."""
        return self.getprop('ro.product.model')

    @property
    def bootloader(self):
        """Bootloader."""
        return self.getprop('ro.bootloader')

    @property
    def hardware(self):
        """Hardware."""
        return self.getprop('ro.hardware')

    @property
    def codename(self):
        """Codename."""
        return self.getprop('ro.build.version.codename')

    @property
    def incremental(self):
        """Incremental."""
        return self.getprop('ro.build.version.incremental')

    @property
    def release(self):
        """Release."""
        return self.getprop('ro.build.version.release')

    @property
    def sdk_int(self):
        """Android SDK version."""
        return int(self.getprop('ro.build.version.sdk'))

    @property
    def preview_sdk_int(self):
        """Android preview SDK version."""
        return int(self.getprop('ro.build.version.preview_sdk'))

    @property
    def fingerprint(self):
        """Fingerprint."""
        return self.getprop('ro.build.fingerprint')

    @property
    def base_os(self):
        """Base OS."""
        return self.getprop('ro.build.version.base_os')

    @property
    def security_patch(self):
        """Security patch."""
        return self.getprop('ro.build.version.security_patch')

    def getprop(self, prop: str) -> Optional[str]:
        """Returns the device property, similar to the value retrieved with
        $ adb shell getprop <prop>
        """
        return self.__props.get(prop)
