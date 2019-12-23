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


from pathlib import Path
from typing import Union

from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive


# This is a Google Drive file id for the folder where all reports are left.
# TODO(dagum): this must come from a non-tracked config file. Let's not merge
#              this file until fully removing this from here.
__ACT_REPORTS_FOLDER_ID = None


def get_report_folder_id() -> Union[str, type(None)]:
    # TODO(dagum): try to get the value of act_reports_id from client_secrets.json
    pass


# TODO(dagum): rather than None we should return some status telling whether it
#              was possible to upload the report entirely.
def publish(path_to_report: Path) -> type(None):
    pass
