#!/bin/bash

# Setup Qt build environment.
source /opt/qt59/bin/qt59-env.sh
mkdir textilosaurus-build && cd textilosaurus-build

# Build application.
#lrelease -compress ../textilosaurus.pro
qmake -spec linux-g++ "WITH_UBUNTU=true" ../textilosaurus.pro 
make
make install

# Obtain linuxdeployqt.
wget -c https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
chmod a+x linuxdeployqt-continuous-x86_64.AppImage 

# Create AppImage.
find . -type f
unset QTDIR; unset QT_PLUGIN_PATH ; unset LD_LIBRARY_PATH
./linuxdeployqt-continuous-x86_64.AppImage "./AppDir/usr/share/applications/com.github.textilosaurus.desktop" -bundle-non-qt-libs -no-translations
./linuxdeployqt-continuous-x86_64.AppImage "./AppDir/usr/share/applications/com.github.textilosaurus.desktop" -appimage -no-translations

# Upload image.
git config --global user.email "rotter.martinos@gmail.com"
git config --global user.name "martinrotter"
git clone https://martinrotter:${GH_TOKEN}@github.com/martinrotter/textilosaurus.wiki.git ./build-wiki

set -- T*.AppImage

rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

imagename="$1"
git_revision=$(git rev-parse --short HEAD)
git_tag_name=$(git describe --tags $(git rev-list --tags --max-count=1))
imagenamenospace="textilosaurus-${git_tag_name}-${git_revision}-linux64.AppImage"

mv "$imagename" "$imagenamenospace"
imagename="$imagenamenospace"

echo "File to upload: $imagename"
echo "URL ending: $imagenamenospace"

url=$(curl --upload-file "./$imagename" "https://transfer.sh/$imagenamenospace" --silent)

rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

wikiline="| Linux | $(date +'%m-%d-%Y %T') | [$git_revision](https\://github.com/martinrotter/textilosaurus/commit/$git_revision) | [transfer.sh]($url) |  "
wikifile="./build-wiki/Development-builds.md"
wikifilenew="./build-wiki/Development-builds.md.new"

echo "Line to add: $wikiline"
cat "$wikifile" | sed -e "s@| Linux | .\+$USE_WEBENGINE |  @$wikiline@g" > "$wikifilenew"

cat "$wikifilenew"
mv "$wikifilenew" "$wikifile"

cd ./build-wiki
git commit -a -m "New files."
git pull origin master
git push origin master
