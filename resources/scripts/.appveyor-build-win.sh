#!/bin/bash
PATH="/mingw64/bin:$PATH"

cd "/c/textilosaurus"
mkdir "textilosaurus-build"
cd "textilosaurus-build"
qmake.exe "C:\textilosaurus\textilosaurus.pro"
mingw32-make
mingw32-make windows_all