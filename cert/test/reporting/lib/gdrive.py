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
"""Isolates Google Drive access via PyDrive, decoupling the rest of the app
from its use."""

import json
from pathlib import Path
import re
from typing import Dict, Union

from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive

#------------------------------------------------------------------------------


class GoogleDriveRelatedError(Exception):
    """Exception raised when the running environment doesn't have everything it
    needs available to access Google Drive.
    """


#------------------------------------------------------------------------------


class SummaryInfo:
    """Summary details like test type or UTC time.
    """

    def __init__(self, path_to_summary: Path):
        metadata = re.match(r"^summary_(\d{8}-\d{6})_(.+)$",
                            path_to_summary.stem)
        self.test_type = metadata.group(2)
        self.utc_timestamp = metadata.group(1)


#------------------------------------------------------------------------------


class GDrive:
    """Singleton proxy for all things Google Drive."""

    class GDriveImpl:
        """Inner class that implements the low-level aspects in PyDrive."""

        def __init__(self):
            self.__raise_if_file_missing("client_secrets.json")
            self.__raise_if_file_missing("gdrive.json")

            with open("gdrive.json", "r") as gdrive_json_file:
                gdrive_config = json.load(gdrive_json_file)
                self.__team_drive_id = self.__raise_if_key_missing(
                    gdrive_config, "team_drive_id")
                self.__act_reports_folder_id = self.__raise_if_key_missing(
                    gdrive_config, "act_reports_id")

            self.api = GoogleDrive(self.__get_google_auth_token())

        def __raise_if_file_missing(self, file_path: str) -> type(None):
            """Checks for the existence of a file, raising exception if
            missing.

            Args:
                file_path: a string with the route to the file to check.
            """
            if not Path(file_path).exists():
                raise GoogleDriveRelatedError(
                    f"Make sure that {file_path} is in {Path.cwd()}")

        def __raise_if_key_missing(self, gdrive_config: Dict[str, str],
                                   key: str) -> str:
            """Checks for the existence of a key in the google drive
            configuration dictionary. If it exists, it's returned. Otherwise,
            an exception is raised.

            Args:
                gdrive_config: the google drive configuration dictionary.
                key: the string to look for.

            Returns: the value for the key if it exists in the configuration.
            """
            if key not in gdrive_config:
                raise GoogleDriveRelatedError(f"grive.json misses {key}")
            return gdrive_config[key]

        def __get_google_auth_token(self) -> GoogleAuth:
            """Gets an authorization token from Google. The first time, such
            authorization token has to be manually approved by the user. Once
            done, a credential is stored locally and automatically re-applied
            for the time being (i.e., without human intervention).
            """
            gauth_token = GoogleAuth()

            # Try to load saved client credentials
            gauth_token.LoadCredentialsFile("gdrive.creds")

            if gauth_token.credentials is None:
                # Authenticate if they're not there
                gauth_token.GetFlow()
                gauth_token.flow.params.update({
                    "access_type": "offline",
                    "approval_prompt": "force"
                })
                gauth_token.LocalWebserverAuth()
            elif gauth_token.access_token_expired:
                # Refresh them if expired
                gauth_token.Refresh()
            else:
                # Initialize the saved credentials
                gauth_token.Authorize()

            # Save the current credentials to a file
            gauth_token.SaveCredentialsFile("gdrive.creds")

            return gauth_token

        def get_file_id(self, file_name: str, parent_id: str) \
            -> Union[str, type(None)]:
            """Retrieves the Google Drive file id for a file in a folder.

            Args:
                file_name: file (or folder) name. Google Drive treats files and
                           folders as files.
                parent_id: Google Drive file id of the parent folder. By
                           default, the ACT Reports folder id.

            Returns: the Google Drive file id if it exists, or None.
            """
            if parent_id is None:
                parent_id = self.__act_reports_folder_id

            file_id = None

            files_in_folder = self.api.ListFile({
                "q": f"'{parent_id}' in parents and trashed=false",
                "corpora": "teamDrive",
                "teamDriveId": self.__team_drive_id,
                "includeTeamDriveItems": "true",
                "supportsTeamDrives": "true"
            }).GetList()

            for _file in files_in_folder:
                if _file["title"] == file_name:
                    file_id = _file["id"]
                    break

            return file_id

        def create_folder_if_nonexistent(self,
                                         folder_name: str,
                                         parent_id: str = None) -> str:
            """Creates a folder at parent_id in Google Drive, unless it already
            exists.

            Args:
                folder_name: self-descriptive.
                parent_id: Google Drive file id of the parent folder. By
                           default, the ACT Reports folder id.

            Returns: the Google Drive file id of the created (or existing)
                     folder.
            """
            if parent_id is None:
                parent_id = self.__act_reports_folder_id

            folder_id = self.get_file_id(folder_name, parent_id)
            if folder_id is None:
                folder = self.api.CreateFile({
                    "title": folder_name,
                    "parents": [{
                        "teamDriveId": self.__team_drive_id,
                        "id": parent_id
                    }],
                    "mimeType": "application/vnd.google-apps.folder"
                })
                folder.Upload(param={'supportsTeamDrives': True})
                folder_id = folder["id"]
            return folder_id

        def upload_file(self,
                        file_path: Path,
                        folder_id: str,
                        file_name: str = None) -> str:
            """Uploads a file to a Google Drive folder.

            Args:
                file_path: Path to the local file to upload.
                folder_id: Google Drive id of the folder where to upload the
                           file to.
                file_name: (optional) name of the file in Google Drive. By
                           default, the same name of file_path.
            """
            if file_name is None:
                file_name = file_path.name
            uploaded_file = self.api.CreateFile({
                "title":
                    file_name,
                "parents": [{
                    "kind": "drive#fileLink",
                    "teamDriveId": self.__team_drive_id,
                    "id": folder_id
                }]
            })
            uploaded_file.SetContentFile(str(file_path))
            uploaded_file.Upload(param={'supportsTeamDrives': True})

            return uploaded_file["id"]

        def publish(self, path_to_summary: Path) -> str:
            """
            Uploads a report to Google Drive, to the ACT reports folder.

            Args:
                path_to_summary: self-described.

            Returns: the Google Drive file id of the uploaded report. If
                     something goes wrong, throws GoogleDriveRelatedError.
            """
            metadata = SummaryInfo(path_to_summary)
            if metadata.test_type is None:
                raise GoogleDriveRelatedError(f"{path_to_summary.stem} " \
                    "doesn't contain test type.")

            return self.upload_file(
                path_to_summary,
                self.create_folder_if_nonexistent(metadata.test_type))

    # Class attribute
    __instance = None

    def __new__(cls):
        """Constructor that always 'constructs' the same single instance.
        In other words, calling GDrive() n times or just once always return the
        same GDrive instance.
        """
        if GDrive.__instance is None:
            GDrive.__instance = GDrive.GDriveImpl()

        return GDrive.__instance

    # This function intercepts all attribute access to GDrive instances, in
    # order to convert these to same attribute access, but to the singleton.
    def __getattr__(self, name):
        return getattr(GDrive.__instance, name)

    @classmethod
    def dispose(cls) -> type(None):
        """Recycles the singleton instance."""
        GDrive.__instance = None
