#################################################################
#
# This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.
#
# This is Textosaurus compilation script for qmake.
#
# Usage:
#   cd ../build-dir
#   qmake ../build.pro -r "CONFIG+=release" "PREFIX=./usr"
#   make
#   make install
#
# Variables:
#   PREFIX - specifies base folder to which files are copied during "make install"
#            step, defaults to "$$OUT_PWD/AppDir/usr" on Linux and to "$$OUT_PWD/app" on Windows.
#            Also, note that INSTALL_ROOT variable during "make install" step is not needed, when
#            PREFIX variable in "qmake" call is used.
#
#   LRELEASE_EXECUTABLE - specifies the name/path of "lrelease" executable, defaults to "lrelease".
#
# Other information:
#   - supports Windows or Linux,
#   - Qt 5.7.0 or higher is required,
#   - C++ 17 is required.
#
# Authors and contributors:
#   - Martin Rotter (project leader).
#
#################################################################

TEMPLATE = subdirs

CONFIG += ordered
SUBDIRS = libtextosaurus textosaurus

libtextosaurus.subdir  = src/libtextosaurus

textosaurus.subdir  = src/textosaurus
textosaurus.depends = libtextosaurus
