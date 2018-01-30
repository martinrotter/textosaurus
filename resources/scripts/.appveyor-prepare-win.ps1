$url = "https://bitbucket.org/skunkos/qt5-minimalistic-builds/downloads/qt-5.10-static-msvc2017-ltcg-x86_64.7z"
$output = "qt.7z"

cd C:\textilosaurus

Invoke-WebRequest -Uri $url -OutFile $output

.\resources\scripts\7za\7za.exe x $output

cd .\qt-5.10-static-msvc2017-ltcg-x86_64\bin
.\qtbinpatcher.exe