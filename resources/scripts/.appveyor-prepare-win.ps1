$qt_stub = "qt-5.12.6-dynamic-msvc2017-x86_64"
$qt_stub_url = $qt_stub + "-openssl111a"

[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
$url = "https://bitbucket.org/skunkos/qt5-minimalistic-builds/downloads/$qt_stub_url.7z"
$output = "qt.7z"

cd C:\textosaurus

Invoke-WebRequest -Uri $url -OutFile $output

& ".\resources\scripts\7za\7za.exe" x $output

cd .\$qt_stub\bin
& ".\qtbinpatcher.exe"