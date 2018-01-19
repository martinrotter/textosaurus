#!/bin/bash
PATH="/mingw64/bin:$PATH"

cd "/c/textilosaurus"
mkdir "textilosaurus-build"
cd "textilosaurus-build"
qmake.exe "C:\textilosaurus\textilosaurus.pro" "CONFIG+=release"
mingw32-make -j
mingw32-make windows_all