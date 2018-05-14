CONFIG *= c++1z warn_on
CONFIG -=  debug_and_release
DEFINES *= QT_USE_QSTRINGBUILDER QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS UNICODE _UNICODE
VERSION = $$APP_VERSION

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
  QMAKE_TARGET_DESCRIPTION = $$APP_NAME ($$APP_TYPE)
  QMAKE_TARGET_COPYRIGHT = $$APP_COPYRIGHT
  QMAKE_TARGET_PRODUCT = $$APP_NAME
}

CONFIG += resources_small
