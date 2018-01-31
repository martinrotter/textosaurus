ls

$qt_stub = "qt-5.10-static-ltcg-msvc2017-x86_64"

cd "C:\textilosaurus"

ls

mkdir "textilosaurus-build"
cd "textilosaurus-build"

ls

$mak_path = "C:\textilosaurus\textilosaurus-build\Makefile"
$env:PATH = "C:\textilosaurus\$qt_stub\bin\;" + $env:PATH

& "C:\textilosaurus\$qt_stub\bin\qmake.exe" "C:\textilosaurus\textilosaurus.pro" "CONFIG+=release"

$mak_contents = [System.IO.File]::ReadAllText($mak_path).Replace(" qjp2.lib", "")
[System.IO.File]::WriteAllText($mak_path, $mak_contents)

& "nmake"
& "nmake" windows_all