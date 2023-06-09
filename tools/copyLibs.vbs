REM Developed by Gustav Gonzalez

Set fileSystem = CreateObject("Scripting.FileSystemObject")

libsDir = "c:\devel\Qt\5.14.2\mingw73_32\bin"
pluginDir = "c:\devel\Qt\5.14.2\mingw73_32\plugins\platforms"
tupiAppDir = "c:\devel\installer\tupitube.app"
sourcePath = "c:\devel\sources\tupitube.app"
sslDir = "c:\devel\sources\ssl"
quazipDir = "c:\Quazip\bin"
zlibDir  = "c:\GnuWin32\bin"

If fileSystem.FolderExists(tupiDir) Then
fileSystem.DeleteFolder tupiAppDir
End If

fileSystem.CreateFolder tupiAppDir
fileSystem.CreateFolder tupiAppDir & "\platforms"

fileSystem.CopyFile libsDir & "\libgcc_s_dw2-1.dll", tupiAppDir & "\libgcc_s_dw2-1.dll"
fileSystem.CopyFile libsDir & "\libstdc++-6.dll", tupiAppDir & "\libstdc++-6.dll"
fileSystem.CopyFile libsDir & "\libwinpthread-1.dll", tupiAppDir & "\libwinpthread-1.dll"
fileSystem.CopyFile libsDir & "\Qt5Core.dll", tupiAppDir & "\Qt5Core.dll"
fileSystem.CopyFile libsDir & "\Qt5Gui.dll", tupiAppDir & "\Qt5Gui.dll"
fileSystem.CopyFile libsDir & "\Qt5Network.dll", tupiAppDir & "\Qt5Network.dll"
fileSystem.CopyFile libsDir & "\Qt5Widgets.dll", tupiAppDir & "\Qt5Widgets.dll"
fileSystem.CopyFile libsDir & "\Qt5Xml.dll", tupiAppDir & "\Qt5Xml.dll"
fileSystem.CopyFile pluginDir & "\qwindows.dll", tupiAppDir & "\platforms\qwindows.dll"
fileSystem.CopyFile sslDir & "\libcrypto-1_1.dll", tupiAppDir & "\libcrypto-1_1.dll"
fileSystem.CopyFile sslDir & "\libssl-1_1.dll", tupiAppDir & "\libssl-1_1.dll"
fileSystem.CopyFile quazipDir & "\quazip.dll", tupiAppDir & "\quazip.dll"
fileSystem.CopyFile zlibDir & "\zlib1.dll", tupiAppDir & "\zlib1.dll"

fileSystem.CopyFile sourcePath & "\src\release\TupiTube.exe", tupiAppDir & "\TupiTube.exe"
fileSystem.CopyFile sourcePath & "\tools\tupitube.ico", tupiAppDir & "\tupitube.ico"
fileSystem.CopyFile sourcePath & "\tools\tupitube.app.iss", tupiAppDir & "\tupitube.app.iss"

MsgBox "Copy is done! :D"