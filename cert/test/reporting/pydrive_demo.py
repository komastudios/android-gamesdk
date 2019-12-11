from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive

gauth = GoogleAuth()
gauth.LocalWebserverAuth()
# I created "Android Certification Test" OAuth2 credential for this.
drive = GoogleDrive(gauth)

parent_id = '1V7PDt8mTRMq6dWyU05R63wz236XgfJxt'

newFolder = drive.CreateFile({
  'title': 'Pepe Curdele',
  "parents": [{
    "kind": "drive#fileLink", 
    "id": parent_id
  }],
  "mimeType": "application/vnd.google-apps.folder"
})
newFolder.Upload()
