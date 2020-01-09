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
"""Google Drive summary upload tests.
"""

# We need, at test level, to be able to catch and report any uncaught exception
# pylint: disable=broad-except

from os import chdir, getcwd, makedirs, unlink
from pathlib import Path
from shutil import copy, rmtree
import unittest

from pydrive.settings import InvalidConfigError

from lib.gdrive import GDrive, GoogleDriveRelatedError

TMP_DIR = "./tmp/integration/test_google_drive"


class TestGoogleDriveAccess(unittest.TestCase):
    """Google Drive access test suite."""

    def setUp(self):
        """Before each test, we try to move to a working directory. Our
        isolated environment. We create such environment from scratch."""
        self.__pwd = Path(getcwd())
        self.__working_dir = Path(TMP_DIR)

        if self.__working_dir.exists():
            rmtree(self.__working_dir)

        # (re)-create it
        if not self.__working_dir.exists():
            makedirs(self.__working_dir)

        chdir(self.__working_dir)

        # Copy credentials to working dir (tests may alter these copies).
        copy(self.__pwd.joinpath("client_secrets.json"),
             Path("client_secrets.json"))
        copy(self.__pwd.joinpath("gdrive.json"), Path("gdrive.json"))
        copy(self.__pwd.joinpath("gdrive.creds"), Path("gdrive.creds"))

    def test_missing_client_secrets(self):
        """It should raise exception if current directory doesn't have a
        client_secrets.json."""

        # Let's loose client_secrets.json (required by Google Drive API).
        unlink("client_secrets.json")

        exception_occurred = False  # Not yet
        try:
            GDrive()
        except GoogleDriveRelatedError:
            exception_occurred = True  # Failing here means success
        except Exception as error:
            print(f"Uncaught exception: {repr(error)}")
        finally:
            self.assertTrue(exception_occurred,
                            "GoogleDriveRelatedError expected")

    def test_missing_gdrive_json(self):
        """It should raise exception if current directory doesn't have a
        gdrive.json."""

        # Let's loose gdrive.json (required by our GDrive proxy).
        unlink("gdrive.json")

        exception_occurred = False  # Not yet
        try:
            GDrive()
        except GoogleDriveRelatedError:
            exception_occurred = True  # Failing here means success
        except Exception as error:
            print(f"Uncaught exception: {repr(error)}")
        finally:
            self.assertTrue(exception_occurred,
                            "GoogleDriveRelatedError expected")

    def test_invalid_client_secrets_format(self):
        """It should raise exception if the client_secrets.json metadata is
        invalid."""

        # Let's replace client_secrets.json with an invalid one.
        copy(
            self.__pwd.joinpath("test/integration/data/gdrive/client_secrets-"
                                "invalid_format.json"),
            Path("client_secrets.json"))
        # Also, let's drop credentials to force
        unlink("gdrive.creds")

        exception_occurred = False  # Not yet
        try:
            GDrive()
        except InvalidConfigError:
            exception_occurred = True  # Failing here means success
        except Exception as error:
            print(f"Uncaught exception: {repr(error)}")
        finally:
            self.assertTrue(exception_occurred, "InvalidConfigError expected")

    def test_invalid_gdrive_json_format(self):
        """It should raise exception if the gdrive.json metadata is invalid."""

        # Let's replace client_secrets.json with an invalid one.
        copy(
            self.__pwd.joinpath("test/integration/data/gdrive/gdrive-"
                                "invalid_format.json"), Path("gdrive.json"))

        exception_occurred = False  # Not yet
        try:
            GDrive()
        except GoogleDriveRelatedError:
            exception_occurred = True  # Failing here means success
        except Exception as error:
            print(f"Uncaught exception: {repr(error)}")
        finally:
            self.assertTrue(exception_occurred,
                            "GoogleDriveRelatedError expected")

    def tearDown(self):
        """We move back to the original directory and remove the working dir.
        """
        chdir(self.__pwd)

        if self.__working_dir.exists():
            rmtree(self.__working_dir)

        # Let's make sure that the singleton gets recycled, so upcoming tests
        # procure their own
        GDrive.dispose()
