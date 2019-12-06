ls

$qt_stub = "qt-5.12.6-dynamic-msvc2017-x86_64"

cd "C:\textosaurus"

ls

mkdir "textosaurus-build"
cd "textosaurus-build"

ls

#$mak_path = "C:\textosaurus\textosaurus-build\Makefile"
$env:PATH = "C:\textosaurus\$qt_stub\bin\;" + $env:PATH

& "C:\textosaurus\$qt_stub\bin\qmake.exe" "C:\textosaurus\build.pro" "-r" "CONFIG+=release"

#$mak_contents = [System.IO.File]::ReadAllText($mak_path).Replace(" qjp2.lib", "")
#[System.IO.File]::WriteAllText($mak_path, $mak_contents)

& "nmake"

cd "src\textosaurus"

& "nmake" install

cd "app"

& "C:\textosaurus\$qt_stub\bin\windeployqt.exe" --no-translations "textosaurus.exe" "libtextosaurus.dll"
Copy-Item -Path C:\textosaurus\$qt_stub\bin\libcrypto*.dll -Destination .\
Copy-Item -Path C:\textosaurus\$qt_stub\bin\libssl*.dll -Destination .\
Copy-Item -Path C:\textosaurus\$qt_stub\bin\openssl.exe -Destination .\

cd ..

& "nmake" windows_all