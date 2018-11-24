$qt_stub = "qt-5.11.2-dynamic-msvc2017-x86_64"
$qt_stub_url = $qt_stub + "-openssl111a"

$url = "https://bitbucket.org/skunkos/qt5-minimalistic-builds/downloads/$qt_stub_url.7z"
$output = "qt.7z"

cd C:\textosaurus

Invoke-WebRequest -Uri $url -OutFile $output

& ".\resources\scripts\7za\7za.exe" x $output

cd .\$qt_stub\bin
& ".\qtbinpatcher.exe"