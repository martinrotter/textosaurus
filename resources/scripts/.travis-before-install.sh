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
  brew install openssl@1.1
  brew link --force openssl@1.1
  
  ln -s /usr/local/opt/openssl/include/openssl /usr/local/include
  ln -s /usr/local/Cellar/openssl/*/include/openssl /usr/bin/openssl
  ln -s /usr/local/opt/openssl/lib/libssl.*.dylib /usr/local/lib/
else
  # Linux.
  sudo apt-key adv --recv-keys --keyserver keyserver.ubuntu.com DFA2F90D
  
  sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
  sudo add-apt-repository ppa:beineri/opt-qt-5.11.1-xenial -y
  sudo add-apt-repository ppa:carsten-uppenbrink-net/dovecot -y
  
  sudo apt-get update
  sudo apt-get -y install gcc-7 g++-7 qt511tools qt511base qt511svg
  sudo apt-get -y install openssl libssl-dev libgl1-mesa-dev 
  
  sudo update-alternatives --remove-all gcc 
  sudo update-alternatives --remove-all g++
  sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 50
  sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 50
  
  gcc -v
fi