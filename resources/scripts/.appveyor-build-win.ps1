cd "C:\textilosaurus"
mkdir "textilosaurus-build"
cd "textilosaurus-build"

$mak_path = "C:\textilosaurus\textilosaurus-build\Makefile"

'C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat'

$env:PATH = "c:\textilosaurus\qt-5.10-static-msvc2017-ltcg-x86_64\bin\;" + $env:PATH

'c:\textilosaurus\qt-5.10-static-msvc2017-ltcg-x86_64\bin\qmake.exe' "C:\textilosaurus\textilosaurus.pro" "CONFIG+=release"

$mak_contents = [System.IO.File]::ReadAllText($mak_path).Replace(" qjp2.lib", "")
[System.IO.File]::WriteAllText($mak_path, $mak_contents)

nmake
nmake windows_all