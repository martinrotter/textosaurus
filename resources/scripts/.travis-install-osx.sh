#!/bin/sh

# Build application.
mkdir textilosaurus-build && cd textilosaurus-build
lrelease -compress ../textilosaurus.pro
qmake ../textilosaurus.pro
make
make install

# Make DMG image.
make dmg
otool -L "Textilosaurus.app/Contents/MacOS/textilosaurus"

set -- *.dmg
dmgname="$1"
git_revision=$(git rev-parse --short HEAD)
dmgnamenospace="textilosaurus-${git_revision}-mac64.dmg"

mv "$dmgname" "$dmgnamenospace"
dmgname="$dmgnamenospace"

echo "File to upload: $dmgname"
echo "URL ending: $dmgnamenospace"

#git config --global user.email "rotter.martinos@gmail.com"
#git config --global user.name "martinrotter"
#git clone -q --depth=1 https://martinrotter:${GH_TOKEN}@github.com/martinrotter/textilosaurus.wiki.git ./build-wiki

url=$(curl --upload-file "./$dmgname" "https://transfer.sh/$dmgnamenospace" --silent)
echo "$url"
#echo "| $(date +'%m-%d-%Y %T') | [$git_revision](https://github.com/martinrotter/rssguard/commit/$git_revision) | [transfer.sh]($url) | $(echo "$USE_WEBENGINE") |  "$'\r' >> ./build-wiki/Mac-OS-X-development-builds.md
#
#cd ./build-wiki
#git add *.*
#git commit -m "New files."
#git pull origin master
#git push origin master