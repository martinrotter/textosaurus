#!/bin/sh

# Build application.
git_revision=$(git rev-parse --short HEAD)
git_tag_name=$TRAVIS_BRANCH

mkdir textosaurus-build && cd textosaurus-build
qmake ../textosaurus.pro 
make
make install

# Make DMG image.
make dmg
otool -L "Textosaurus.app/Contents/MacOS/textosaurus"

set -- *.dmg

rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

dmgname="$1"
dmgnamenospace="textosaurus-${git_tag_name}-${git_revision}-mac64.dmg"

mv $dmgname $dmgnamenospace

echo "File to upload: $dmgnamenospace"

# DMG is finished, upload it.
git config --global user.email "rotter.martinos@gmail.com"
git config --global user.name "martinrotter"
git clone https://martinrotter:${GH_TOKEN}@github.com/martinrotter/textosaurus.wiki.git ./build-wiki

url=$(curl --upload-file "./$dmgnamenospace" "https://transfer.sh/$dmgnamenospace" --silent)

rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

wikiline="| Mac OS X | $(date +'%m-%d-%Y %T') | [$git_revision](https\://github.com/martinrotter/textosaurus/commit/$git_revision) | [transfer.sh]($url) |  "
wikifile="./build-wiki/Development-builds.md"
wikifilenew="./build-wiki/Development-builds.md.new"

echo "Line to add: $wikiline"
cat "$wikifile" | sed -e "s@| Mac .*|  @$wikiline@g" > "$wikifilenew"

cat "$wikifilenew"
mv "$wikifilenew" "$wikifile"

cd ./build-wiki
git commit -a -m "New files."
git pull origin master
git push origin master