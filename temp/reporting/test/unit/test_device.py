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
"""Functions for deploying locally and to FTL, and handing the test data on
for subsequent processing

Meant to be called from ./run.py
"""

from unittest import TestCase

from lib.device import DeviceCatalog, DeviceInfo

# These are our witness devices. Notice that their codenames go in ascending
# order whereas their brand and model go in descending order.
DEVICE_1 = DeviceInfo("111", "ZZZ", "zzzzz")
DEVICE_2 = DeviceInfo("222", "YYY", "yyyyy")
DEVICE_3 = DeviceInfo("333", "XXX", "xxxxx")
# Through these tests we'll check that traversing the container, no matter the
# insertion order, always retrieves devices ordered by brand and model.


class TestDeviceInfo(TestCase):
    """Stresses different DeviceInfo and DeviceCatalog features."""

    def setUp(self):
        self.__device_catalog = DeviceCatalog()

        self.__device_catalog.push(DEVICE_2)
        self.__device_catalog.push(DEVICE_1)
        self.__device_catalog.push(DEVICE_3)

    def test_sort_by_brand_model(self):
        """Checks that inserting devices in any arbitrary order still gets them
        iterable in a lexicographic order based on brand and model."""
        sorted_iterable = iter(self.__device_catalog)

        self.assertEqual("Xxxxx", next(sorted_iterable).model)
        self.assertEqual("Yyyyy", next(sorted_iterable).model)
        self.assertEqual("Zzzzz", next(sorted_iterable).model)
        with self.assertRaises(StopIteration):
            next(sorted_iterable)

    def test_codename_is_contained(self):
        """Tests that the catalog can be checked for containment."""
        self.assertIn(DEVICE_1.codename, self.__device_catalog)
        self.assertIn(DEVICE_2.codename, self.__device_catalog)
        self.assertIn(DEVICE_3.codename, self.__device_catalog)

    def test_codename_as_index(self):
        """Tests that the catalog can be checked for containment."""
        self.assertEqual(DEVICE_1, self.__device_catalog[DEVICE_1.codename])
        self.assertEqual(DEVICE_2, self.__device_catalog[DEVICE_2.codename])
        self.assertEqual(DEVICE_3, self.__device_catalog[DEVICE_3.codename])

    def test_push_once_codename(self):
        """Checks that a same codename can be only pushed once."""
        self.assertEqual(3, len(self.__device_catalog))
        # Codename 333 was already in

        # Let's confirm that pushing that codename with another brand doesn't
        # succeed
        self.assertFalse(
            self.__device_catalog.push(DeviceInfo("333", "AAA", "aaaaa")))
        # Catalog length doesn't change
        self.assertEqual(3, len(self.__device_catalog))
        # Brand remains the original one for that codename in the catalog
        self.assertEqual("XXX", self.__device_catalog["333"].brand)

        # Now let's try the same but with a not-yet-pushed codename
        self.assertTrue(
            self.__device_catalog.push(DeviceInfo("444", "AAA", "aaaaa")))
        # Container length this time increases
        self.assertEqual(4, len(self.__device_catalog))
        # New device gets correctly inserted
        self.assertEqual("AAA", self.__device_catalog["444"].brand)

    def test_container_is_iterable(self):
        """Check that the container can be iterated thru a loop."""
        sorted_list = []

        # Here's what we want to confirm: the container can be traversed,
        # sorted by brand and model.
        for device in self.__device_catalog:
            sorted_list.append(device)

        self.assertEqual("XXX", sorted_list[0].brand)
        self.assertEqual("YYY", sorted_list[1].brand)
        self.assertEqual("ZZZ", sorted_list[2].brand)

    def tearDown(self):
        DeviceCatalog.dispose()
