ls

$qt_stub = "qt-5.10-static-ltcg-msvc2017-x86_64"

cd "C:\textosaurus"

ls

mkdir "textosaurus-build"
cd "textosaurus-build"

ls

$mak_path = "C:\textosaurus\textosaurus-build\Makefile"
$env:PATH = "C:\textosaurus\$qt_stub\bin\;" + $env:PATH

& "C:\textosaurus\$qt_stub\bin\qmake.exe" "C:\textosaurus\textosaurus.pro" "CONFIG+=release"

$mak_contents = [System.IO.File]::ReadAllText($mak_path).Replace(" qjp2.lib", "")
[System.IO.File]::WriteAllText($mak_path, $mak_contents)

& "nmake"
& "nmake" windows_all