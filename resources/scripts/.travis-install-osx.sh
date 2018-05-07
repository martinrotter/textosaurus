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