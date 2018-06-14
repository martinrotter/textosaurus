#!/bin/sh

if test "$TRAVIS_OS_NAME" = "osx"; then
  # Mac OS X.
  brew update
  brew install p7zip
  brew link --force p7zip
  brew install qt5
  brew link --force qt5
  brew install curl
  brew link --force curl
else
  # Linux.
  sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
  sudo add-apt-repository ppa:beineri/opt-qt594-trusty -y
  sudo apt-get update
  sudo apt-get -y install gcc-7 g++-7 qt59tools qt59base qt59svg
  
  sudo update-alternatives --remove-all gcc 
  sudo update-alternatives --remove-all g++
  sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 50
  sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 50
  
  gcc -v
fi