#!/bin/bash

# This script will pull Flathub/Textosaurus repo
# and push new appdata/flatpak-json to new branch.
echo "flathub"

git clone https://martinrotter:${GH_TOKEN}@github.com/flathub/io.github.martinrotter.textosaurus.git ./flathub
cd flathub
cat *.*