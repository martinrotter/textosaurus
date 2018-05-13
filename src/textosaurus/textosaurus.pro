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
