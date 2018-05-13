TEMPLATE    = app
TARGET      = textosaurus

MSG_PREFIX                    = "textosaurus"
APP_NAME                      = "Textosaurus"
APP_LOW_NAME                  = "textosaurus"
APP_REVERSE_NAME              = "io.github.martinrotter.textosaurus"
APP_AUTHOR                    = "Martin Rotter"
APP_COPYRIGHT                 = "(C) 2018 $$APP_AUTHOR"
APP_VERSION                   = "0.9.5"
APP_LONG_NAME                 = "$$APP_NAME $$APP_VERSION"
APP_EMAIL                     = "rotter.martinos@gmail.com"
APP_URL                       = "https://github.com/martinrotter/textosaurus"
APP_URL_ISSUES                = "https://github.com/martinrotter/textosaurus/issues"
APP_URL_ISSUES_NEW            = "https://github.com/martinrotter/textosaurus/issues/new"
APP_URL_WIKI                  = "https://github.com/martinrotter/textosaurus/wiki"
APP_USERAGENT                 = "Textosaurus/$$APP_VERSION (io.github.martinrotter.textosaurus)"
APP_DONATE_URL                = "https://martinrotter.github.io/donate/"
APP_WIN_ARCH                  = "win64"

message($$MSG_PREFIX: Welcome to $$APP_NAME qmake script.)

lessThan(QT_MAJOR_VERSION, 5)|lessThan(QT_MINOR_VERSION, 7) {
  error($$MSG_PREFIX: At least Qt \"5.7.0\" is required!!!)
}

isEmpty(PREFIX) {
  message($$MSG_PREFIX: PREFIX variable is not set. This might indicate error.)

  win32 {
    PREFIX = $$OUT_PWD/app
  }

  mac {
    PREFIX = $$quote($$OUT_PWD/$${APP_NAME}.app)
  }

  unix:!mac:!android {
    PREFIX = $$OUT_PWD/AppDir/usr
  }
}

message($$MSG_PREFIX: Shadow copy build directory \"$$OUT_PWD\".)

isEmpty(LRELEASE_EXECUTABLE) {
  LRELEASE_EXECUTABLE = lrelease
  message($$MSG_PREFIX: LRELEASE_EXECUTABLE variable is not set.)
}

# Custom definitions.
DEFINES += APP_VERSION='"\\\"$$APP_VERSION\\\""'
DEFINES += APP_NAME='"\\\"$$APP_NAME\\\""'
DEFINES += APP_LOW_NAME='"\\\"$$APP_LOW_NAME\\\""'
DEFINES += APP_LONG_NAME='"\\\"$$APP_LONG_NAME\\\""'
DEFINES += APP_AUTHOR='"\\\"$$APP_AUTHOR\\\""'
DEFINES += APP_EMAIL='"\\\"$$APP_EMAIL\\\""'
DEFINES += APP_URL='"\\\"$$APP_URL\\\""'
DEFINES += APP_URL_ISSUES='"\\\"$$APP_URL_ISSUES\\\""'
DEFINES += APP_URL_ISSUES_NEW='"\\\"$$APP_URL_ISSUES_NEW\\\""'
DEFINES += APP_URL_WIKI='"\\\"$$APP_URL_WIKI\\\""'
DEFINES += APP_USERAGENT='"\\\"$$APP_USERAGENT\\\""'
DEFINES += APP_DONATE_URL='"\\\"$$APP_DONATE_URL\\\""'
DEFINES += APP_SYSTEM_NAME='"\\\"$$QMAKE_HOST.os\\\""'
DEFINES += APP_SYSTEM_VERSION='"\\\"$$QMAKE_HOST.arch\\\""'

CODECFORTR  = UTF-8
CODECFORSRC = UTF-8

exists(../../.git) {
  APP_REVISION = $$system(git rev-parse --short HEAD)
}

isEmpty(APP_REVISION) {
  APP_REVISION = "-"
}

DEFINES += APP_REVISION='"\\\"$$APP_REVISION\\\""'

message($$MSG_PREFIX: $$APP_NAME version is: \"$$APP_VERSION\".)
message($$MSG_PREFIX: Detected Qt version: \"$$QT_VERSION\".)
message($$MSG_PREFIX: Build destination directory: \"$$DESTDIR\".)
message($$MSG_PREFIX: Prefix directory: \"$$PREFIX\".)
message($$MSG_PREFIX: Build revision: \"$$APP_REVISION\".)
message($$MSG_PREFIX: lrelease executable name: \"$$LRELEASE_EXECUTABLE\".)

QT *= core gui widgets network svg

CONFIG *= c++1z warn_on
CONFIG -=  debug_and_release
DEFINES *= TEXTOSAURUS_DLLSPEC=Q_DECL_IMPORT QT_USE_QSTRINGBUILDER QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS UNICODE _UNICODE
VERSION = $$APP_VERSION

DEFINES -= MAKING_LIBRARY=1

msvc {
  QMAKE_CXXFLAGS += /std:c++latest
}

gcc|g++|clang* {
  QMAKE_CXXFLAGS += -std=c++17
}

# Setup specific compiler options.
CONFIG(release, debug|release) {
  message($$MSG_PREFIX: Building in "release" mode.)

  gcc:QMAKE_CXXFLAGS_RELEASE -= -O2
  clang:QMAKE_CXXFLAGS_RELEASE -= -O2
  gcc:QMAKE_CXXFLAGS_RELEASE *= -O3
  clang:QMAKE_CXXFLAGS_RELEASE *= -O3
}
else {
  message($$MSG_PREFIX: Building in "debug" mode.)

  DEFINES *= DEBUG=1
  gcc:QMAKE_CXXFLAGS_DEBUG *= -Wall
  clang:QMAKE_CXXFLAGS_DEBUG *= -Wall
}

CONFIG(FLATPAK_MODE) {
  message($$MSG_PREFIX: Enabling Flatpak-specific code.)
  DEFINES *= FLATPAK_MODE=1
}

DISTFILES += ../../resources/scripts/uncrustify/uncrustify.cfg

MOC_DIR = $$OUT_PWD/moc
RCC_DIR = $$OUT_PWD/rcc
UI_DIR = $$OUT_PWD/ui

mac {
  QT *= macextras
}

# Make needed tweaks for RC file getting generated on Windows.
win32 {
  RC_ICONS = ../../resources/graphics/textosaurus.ico
  QMAKE_TARGET_COMPANY = $$APP_AUTHOR
  QMAKE_TARGET_DESCRIPTION = $$APP_NAME (executable)
  QMAKE_TARGET_COPYRIGHT = $$APP_COPYRIGHT
  QMAKE_TARGET_PRODUCT = $$APP_NAME
}

CONFIG += resources_small

SOURCES += main.cpp
INCLUDEPATH +=  ../libtextosaurus \
                ../libtextosaurus/common/gui \
                $$OUT_PWD/../libtextosaurus \
                $$OUT_PWD/../libtextosaurus/ui

DEPENDPATH += $$PWD/../libtextosaurus

win32: LIBS += -L$$OUT_PWD/../libtextosaurus/ -llibtextosaurus
unix: LIBS += -L$$OUT_PWD/../libtextosaurus/ -ltextosaurus

# Create new "make 7zip" target and "make zip" target.
win32 {
  seven_zip.target = 7zip
  seven_zip.depends = install
  seven_zip.commands = $$shell_path($$shell_quote($$PWD/../../resources/scripts/7za/7za.exe)) a -t7z $$TARGET-$$APP_VERSION-$$APP_REVISION-$${APP_WIN_ARCH}.7z $$shell_path($$PREFIX/*)

  zip.target = zip
  zip.depends = install
  zip.commands = $$shell_path($$shell_quote($$PWD/../../resources/scripts/7za/7za.exe)) a -tzip $$TARGET-$$APP_VERSION-$$APP_REVISION-$${APP_WIN_ARCH}.zip $$shell_path($$PREFIX/*)

  QMAKE_EXTRA_TARGETS += seven_zip zip
}

# Create NSIS installer target on Windows.
win32 {
  nsis.target = nsis
  nsis.depends = install
  nsis.commands = \
    $$shell_path($$shell_quote($$PWD/resources/scripts/sed/sed.exe)) -e \"s|@APP_VERSION@|$$APP_VERSION|g; s|@APP_WIN_ARCH@|$$APP_WIN_ARCH|g; s|@APP_REVISION@|$$APP_REVISION|g; s|@APP_NAME@|$$APP_NAME|g; s|@APP_LOW_NAME@|$$APP_LOW_NAME|g; s|@EXE_NAME@|$${APP_LOW_NAME}.exe|g; s|@PWD@|$$replace(PWD, /, \\\\\\\\\\\\\\\\)|g; s|@OUT_PWD@|$$replace(OUT_PWD, /, \\\\\\\\\\\\\\\\)|g\" $$shell_quote($$shell_path($$PWD/resources/nsis/NSIS.definitions.nsh.in)) > $$shell_quote($$shell_path($$OUT_PWD/NSIS.definitions.nsh)) && \
    xcopy \"$$system_path($$PWD/resources/nsis/NSIS.template.in)\" \"$$system_path($$OUT_PWD/)\" /Y && \
    $$shell_path($$shell_quote($$PWD/resources/scripts/nsis/makensis.exe)) \"$$system_path($$OUT_PWD/NSIS.template.in)\"

  QMAKE_EXTRA_TARGETS += nsis
}

win32 {
  windows_all.target = windows_all
  windows_all.depends = seven_zip nsis
  windows_all.commands = echo "windows_all done..."

  QMAKE_EXTRA_TARGETS += windows_all
}

mac {
  dmg.target = dmg
  dmg.depends = install
  dmg.commands = macdeployqt $$shell_quote($$shell_path($$PREFIX)) -dmg

  QMAKE_EXTRA_TARGETS += dmg
}

# Install all files on Windows.
win32 {
  target.path = $$PREFIX

  lib.files = $$OUT_PWD/../libtextosaurus/*.dll
  lib.path = $$PREFIX

  INSTALLS += target lib
}

# Install all files on Linux.
unix:!mac:!android {
  target.path = $$PREFIX/bin

  desktop_file.files = ../../resources/desktop/$${APP_REVERSE_NAME}.desktop
  desktop_file.path = $$quote($$PREFIX/share/applications/)

  appdata.files = ../../resources/desktop/$${APP_REVERSE_NAME}.appdata.xml
  appdata.path = $$quote($$PREFIX/share/metainfo/)

  lib.files = $$OUT_PWD/../libtextosaurus/*.so
  lib.path = $$quote($$PREFIX/lib/)

  desktop_icon.files = ../../resources/graphics/$${TARGET}.png
  desktop_icon.path = $$quote($$PREFIX/share/icons/hicolor/512x512/apps/)

  INSTALLS += target desktop_file desktop_icon appdata lib
}

mac {
  IDENTIFIER = $$APP_REVERSE_NAME
  CONFIG -= app_bundle
  ICON = ../../resources/graphics/$${TARGET}.icns
  QMAKE_MAC_SDK = macosx10.12
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
  LIBS += -framework AppKit

  QMAKE_POST_LINK += $$system(install_name_tool -change "libtextosaurus.dylib" "@executable_path/libtextosaurus.dylib" $$PREFIX/Contents/MacOS/textosaurus)

  target.path = $$quote($$PREFIX/Contents/MacOS/)

  lib.files = $$OUT_PWD/../libtextosaurus/*.dylib
  lib.path = $$quote($$PREFIX/Contents/MacOS/)

  # Install app icon.
  icns_icon.files = ../../resources/graphics/$${TARGET}.icns
  icns_icon.path = $$quote($$PREFIX/Contents/Resources/)

  # Install Info.plist.
  info_plist.files = ../../resources/macosx/Info.plist.in
  info_plist.path  = $$quote($$PREFIX/Contents/)

  # Process the just installed Info.plist.
  info_plist2.extra = @sed -e "s,@EXECUTABLE@,$$TARGET,g" -e "s,@SHORT_VERSION@,$$APP_VERSION,g" -e "s,@APP_NAME@,\"$$APP_NAME\",g" -e "s,@ICON@,$$basename(ICON),g"  -e "s,@TYPEINFO@,"????",g" $$shell_quote($$PREFIX/Contents/Info.plist.in) > $$shell_quote($$PREFIX/Contents/Info.plist) && \
                      rm -f $$shell_quote($$PREFIX/Contents/Info.plist.in)
  info_plist2.path = $$quote($$PREFIX/Contents/)

  # Install PkgInfo
  pkginfo.extra = @printf "APPL????" > $$shell_quote($$PREFIX/Contents/PkgInfo)
  pkginfo.path = $$quote($$PREFIX/Contents/)

  INSTALLS += target lib icns_icon info_plist info_plist2 pkginfo
}
