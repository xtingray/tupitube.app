[Setup]
AppName=TupiTube App
AppVersion=1.0
AppPublisher=MaeFloresta
AppPublisherURL=http://www.maefloresta.com/
OutputBaseFilename=tupitube_app
DefaultDirName={pf}\TupiTube.App 
DefaultGroupName=TupiTube
OutputDir=c:\devel\installer
ChangesEnvironment=no
ChangesAssociations=no

[Dirs]
Name: "{app}\platforms"

[Files]
Source: "platforms\*"; DestDir: "{app}\platforms"
Source: "*"; DestDir: "{app}"

[Icons]
Name: "{group}\TupiTube App"; Filename: "{app}\TupiTube.exe"; IconFilename: "{app}\tupitube.ico"

