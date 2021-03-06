TEMPLATE    = app
TARGET      = textosaurus

MSG_PREFIX  = "textosaurus"
APP_TYPE    = "executable"

include(../../pri/vars.pri)

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

include(../../pri/defs.pri)

message($$MSG_PREFIX: Shadow copy build directory \"$$OUT_PWD\".)
message($$MSG_PREFIX: $$APP_NAME version is: \"$$APP_VERSION\".)
message($$MSG_PREFIX: Detected Qt version: \"$$QT_VERSION\".)
message($$MSG_PREFIX: Build destination directory: \"$$DESTDIR\".)
message($$MSG_PREFIX: Prefix directory: \"$$PREFIX\".)
message($$MSG_PREFIX: Build revision: \"$$APP_REVISION\".)

QT *= core gui widgets network svg

include(../../pri/build_opts.pri)

DEFINES *= TEXTOSAURUS_DLLSPEC=Q_DECL_IMPORT EXPORT_IMPORT_API=Q_DECL_IMPORT
DEFINES -= MAKING_LIBRARY=1

CONFIG(FLATPAK_MODE) {
  message($$MSG_PREFIX: Enabling Flatpak-specific code.)
  DEFINES *= FLATPAK_MODE=1
}

os2 {
  SCINTILLA_DIR = $$PWD/../libtextosaurus/3rd-party/scintilla-lt
  SCINTILLA_DIR_NAME = scintilla-lt
}
else {
  SCINTILLA_DIR = $$PWD/../libtextosaurus/3rd-party/scintilla
  SCINTILLA_DIR_NAME = scintilla
}

DEFINES *= SCINTILLA_DIR=$${SCINTILLA_DIR}/
message($$MSG_PREFIX: Scintilla subdirectory: \"$$SCINTILLA_DIR\".)

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
    $$shell_path($$shell_quote($$PWD/../../resources/scripts/sed/sed.exe)) -e \"s|@APP_VERSION@|$$APP_VERSION|g; s|@APP_WIN_ARCH@|$$APP_WIN_ARCH|g; s|@APP_REVISION@|$$APP_REVISION|g; s|@APP_NAME@|$$APP_NAME|g; s|@APP_LOW_NAME@|$$APP_LOW_NAME|g; s|@EXE_NAME@|$${APP_LOW_NAME}.exe|g; s|@PWD@|$$replace(PWD, /, \\\\\\\\\\\\\\\\)|g; s|@OUT_PWD@|$$replace(OUT_PWD, /, \\\\\\\\\\\\\\\\)|g\" $$shell_quote($$shell_path($$PWD/../../resources/nsis/NSIS.definitions.nsh.in)) > $$shell_quote($$shell_path($$OUT_PWD/NSIS.definitions.nsh)) && \
    xcopy \"$$system_path($$PWD/../../resources/nsis/NSIS.template.in)\" \"$$system_path($$OUT_PWD/)\" /Y && \
    $$shell_path($$shell_quote($$PWD/../../resources/scripts/nsis/makensis.exe)) \"$$system_path($$OUT_PWD/NSIS.template.in)\"

  QMAKE_EXTRA_TARGETS += nsis
}

win32 {
  windows_all.target = windows_all
  windows_all.depends = seven_zip nsis
  windows_all.commands = echo "windows_all done..."

  QMAKE_EXTRA_TARGETS += windows_all
}

mac {
  frameworks.target = frameworks
  frameworks.depends = install
  frameworks.commands = macdeployqt $$shell_quote($$shell_path($$PREFIX))

  QMAKE_EXTRA_TARGETS += frameworks
}

include(../../pri/install.pri)
