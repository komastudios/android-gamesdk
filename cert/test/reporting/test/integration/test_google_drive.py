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

from lib.gdrive import GDrive, GoogleDriveRelatedError

CURR_DIR = Path(getcwd())
DATA_DIR = CURR_DIR.joinpath("test/integration/data/gdrive")
WORK_DIR = CURR_DIR.joinpath("tmp/integration/test_google_drive")


class TestGoogleDriveAccess(unittest.TestCase):
    """Google Drive access test suite."""

    def setUp(self):
        """Before each test, we try to move to a working directory. Our
        isolated environment. We create such environment from scratch."""
        if WORK_DIR.exists():
            rmtree(WORK_DIR)

        # (re)-create it
        if not WORK_DIR.exists():
            makedirs(WORK_DIR)

        chdir(WORK_DIR)

        # Copy credentials to working dir (tests may alter these copies).
        copy(CURR_DIR.joinpath("client_secrets.json"),
             Path("client_secrets.json"))
        copy(CURR_DIR.joinpath("gdrive.json"), Path("gdrive.json"))
        copy(CURR_DIR.joinpath("gdrive.creds"), Path("gdrive.creds"))

        self.__file_ids_to_delete = []

    def test_missing_client_secrets(self):
        """It should raise exception if current directory doesn't have a
        client_secrets.json."""

        # Let's loose client_secrets.json (required by Google Drive API).
        unlink("client_secrets.json")

        exception_occurred = False  # Not yet
        try:
            GDrive().publish(
                DATA_DIR.joinpath("summary_21200110-012345_test_type.txt"))
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
            GDrive().publish(
                DATA_DIR.joinpath("summary_21200110-012345_test_type.txt"))
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
        copy(DATA_DIR.joinpath("client_secrets-invalid_format.json"),
             Path("client_secrets.json"))
        # Also, let's drop credentials to force re-authorization
        unlink("gdrive.creds")

        exception_occurred = False  # Not yet
        try:
            GDrive().publish(
                DATA_DIR.joinpath("summary_21200110-012345_test_type.txt"))
        except GoogleDriveRelatedError:
            exception_occurred = True  # Failing here means success
        except Exception as error:
            print(f"Uncaught exception: {repr(error)}")
        finally:
            self.assertTrue(exception_occurred,
                            "GoogleDriveRelatedError expected")

    def test_invalid_gdrive_json_format(self):
        """It should raise exception if the gdrive.json metadata is invalid."""

        # Let's replace client_secrets.json with an invalid one.
        copy(DATA_DIR.joinpath("gdrive-invalid_format.json"),
             Path("gdrive.json"))

        exception_occurred = False  # Not yet
        try:
            GDrive().publish(
                DATA_DIR.joinpath("summary_21200110-012345_test_type.txt"))
        except GoogleDriveRelatedError:
            exception_occurred = True  # Failing here means success
        except Exception as error:
            print(f"Uncaught exception: {repr(error)}")
        finally:
            self.assertTrue(exception_occurred,
                            "GoogleDriveRelatedError expected")

    def test_invalid_summary_name(self):
        """It should reject summaries whose stems don't follow the pattern
        summary_YYYYMMDD-HHMMSS_some_test_name.
        """
        exception_occurred = False  # Not yet
        try:
            GDrive().publish(
                DATA_DIR.joinpath("invalid_21200110-012345_test_type.txt"))
        except GoogleDriveRelatedError:
            exception_occurred = True  # Failing here means success
        except Exception as error:
            print(f"Uncaught exception: {repr(error)}")
        finally:
            self.assertTrue(exception_occurred,
                            "GoogleDriveRelatedError expected")

    def test_publish_summary(self):
        """Happy path: successfully uploads a file to Google Drive."""
        try:
            summary_id = GDrive().publish(
                DATA_DIR.joinpath("summary_21200110-012345_test_type.txt"))
            self.__file_ids_to_delete.append(summary_id)

            test_folder_id = GDrive().get_file_id("test_type")
            self.assertIsNotNone(test_folder_id)
            self.__file_ids_to_delete.append(test_folder_id)

            self.assertEqual(
                summary_id,
                GDrive().get_file_id("summary_21200110-012345_test_type.txt",
                                     test_folder_id))

        except Exception as error:
            self.fail(f"Unexpected error: {repr(error)}")

    def tearDown(self):
        """We move back to the original directory and remove the working dir.
        """
        chdir(CURR_DIR)

        if WORK_DIR.exists():
            rmtree(WORK_DIR)

        for file_id in self.__file_ids_to_delete:
            GDrive().delete_file(file_id)

        # Let's make sure that the singleton gets recycled, so upcoming tests
        # procure their own
        GDrive.dispose()
