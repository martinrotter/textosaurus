set "PATH=C:\textilosaurus\qt-5.10-static-msvc2017-ltcg-x86_64\bin;%PATH%"
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"

mkdir textilosaurus-build
cd textilosaurus-build

C:\textilosaurus\qt-5.10-static-msvc2017-ltcg-x86_64\bin\qmake.exe C:\textilosaurus\textilosaurus.pro
nmake
nmake windows_all