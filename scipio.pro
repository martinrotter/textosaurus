#################################################################
#
# For license of this file, see <project-root-folder>/LICENSE.md.
#
#
# This is Scipio compilation script for qmake.
#
# Usage:
#   a) DEBUG build for testing. (out of source build type)
#     cd ../build-dir
#     qmake ../scipio-dir/scipio.pro -r CONFIG+=debug PREFIX=./usr
#     make
#     make install
#
#   b) RELEASE build for production use. (out of source build type)
#     cd ../build-dir
#     qmake ../scipio-dir/scipio.pro -r CONFIG+=release PREFIX=./usr
#     make
#     make install
#
# Variables:
#   PREFIX - specifies base folder to which files are copied during "make install"
#            step, defaults to "$$OUT_PWD/usr" on Linux and to "$$OUT_PWD/app" on Windows.
#   LRELEASE_EXECUTABLE - specifies the name/path of "lrelease" executable, defaults to "lrelease".
#
#
# Other information:
#   - supports Windows, Linux,
#   - Qt 5.5.0 or higher is required,
#   - C++ 11 is required.
#
# Authors and contributors:
#   - Martin Rotter (project leader),
#
#################################################################

TEMPLATE    = app
TARGET      = scipio
DEFINES	    *= QT_USE_QSTRINGBUILDER

message(scipio: Welcome Scipio qmake script.)

lessThan(QT_MAJOR_VERSION, 5)|lessThan(QT_MINOR_VERSION, 5) {
  error(scipio: At least Qt \"5.5.0\" is required!!!)
}

APP_NAME                      = "Scipio"
APP_LOW_NAME                  = "scipio"
APP_REVERSE_NAME              = "com.github.scipio"
APP_LOW_H_NAME                = ".scipio"
APP_AUTHOR                    = "Martin Rotter"
APP_COPYRIGHT                 = "(C) 2017 $$APP_AUTHOR"
APP_VERSION                   = "0.0.1"
APP_LONG_NAME                 = "$$APP_NAME $$APP_VERSION"
APP_EMAIL                     = "rotter.martinos@gmail.com"
APP_URL                       = "https://github.com/martinrotter/scipio"
APP_URL_ISSUES                = "https://github.com/martinrotter/scipio/issues"
APP_URL_ISSUES_NEW            = "https://github.com/martinrotter/scipio/issues/new"
APP_URL_WIKI                  = "https://github.com/martinrotter/scipio/wiki"
APP_USERAGENT                 = "Scipio/$$APP_VERSION (github.com/martinrotter/scipio)"
APP_DONATE_URL                = "https://www.patreon.com/martinrotter"
APP_WIN_ARCH                  = "win64"

isEmpty(PREFIX) {
  message(scipio: PREFIX variable is not set. This might indicate error.)

  win32 {
    PREFIX = $$OUT_PWD/app
  }

  android {
    PREFIX = $$OUT_PWD/app
  }

  mac {
    PREFIX = $$quote($$OUT_PWD/$${APP_NAME}.app)
  }

  unix:!mac:!android {
    PREFIX = $$OUT_PWD/AppDir/usr
  }
}

message(scipio: Shadow copy build directory \"$$OUT_PWD\".)

isEmpty(LRELEASE_EXECUTABLE) {
  LRELEASE_EXECUTABLE = lrelease
  message(scipio: LRELEASE_EXECUTABLE variable is not set.)
}

# Custom definitions.
DEFINES += APP_VERSION='"\\\"$$APP_VERSION\\\""'
DEFINES += APP_NAME='"\\\"$$APP_NAME\\\""'
DEFINES += APP_LOW_NAME='"\\\"$$APP_LOW_NAME\\\""'
DEFINES += APP_LOW_H_NAME='"\\\"$$APP_LOW_H_NAME\\\""'
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

exists(.git) {
  APP_REVISION = $$system(git rev-parse --short HEAD)
}

isEmpty(APP_REVISION) {
  APP_REVISION = ""
}

DEFINES += APP_REVISION='"\\\"$$APP_REVISION\\\""'

message(scipio: Scipio version is: \"$$APP_VERSION\".)
message(scipio: Detected Qt version: \"$$QT_VERSION\".)
message(scipio: Build destination directory: \"$$DESTDIR\".)
message(scipio: Prefix directory: \"$$PREFIX\".)
message(scipio: Build revision: \"$$APP_REVISION\".)
message(scipio: lrelease executable name: \"$$LRELEASE_EXECUTABLE\".)

QT *= core gui widgets network xml printsupport

CONFIG *= c++11 warn_on
DEFINES *= QT_USE_QSTRINGBUILDER QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS UNICODE _UNICODE
VERSION = $$APP_VERSION

# Add QScintilla stuff.
DEFINES += QSCINTILLA_DLL

win32 {
  # Add QScintilla from fixed path.
  LIBS += -L$$PWD/resources/binaries/qscintilla/ -lqscintilla2_qt5

  INCLUDEPATH += $$PWD/resources/binaries/qscintilla
  DEPENDPATH += $$PWD/resources/binaries/qscintilla
}
else {
  # Use system-wide QScintilla.
  LIBS += -lqscintilla2_qt5
}

win32 {
  # Makes sure we use correct subsystem on Windows.
  !contains(QMAKE_TARGET.arch, x86_64) {
    message(scipio: Compilling x86 variant.)
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
  } else {
    message(scipio: Compilling x86_64 variant.)
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.02
  }
}

DISTFILES += resources/scripts/uncrustify/uncrustify.cfg

MOC_DIR = $$OUT_PWD/moc
RCC_DIR = $$OUT_PWD/rcc
UI_DIR = $$OUT_PWD/ui

mac {
  QT *= macextras
}

# Make needed tweaks for RC file getting generated on Windows.
win32 {
  RC_ICONS = resources/graphics/scipio.ico
  QMAKE_TARGET_COMPANY = $$APP_AUTHOR
  QMAKE_TARGET_DESCRIPTION = $$APP_NAME
  QMAKE_TARGET_COPYRIGHT = $$APP_COPYRIGHT
  QMAKE_TARGET_PRODUCT = $$APP_NAME
}

CONFIG *= resources_big
RESOURCES += resources/icons.qrc \
             resources/scipio.qrc

HEADERS +=  src/definitions/definitions.h \
            src/dynamic-shortcuts/dynamicshortcuts.h \
            src/dynamic-shortcuts/dynamicshortcutswidget.h \
            src/dynamic-shortcuts/shortcutbutton.h \
            src/dynamic-shortcuts/shortcutcatcher.h \
            src/exceptions/applicationexception.h \
            src/exceptions/ioexception.h \
            src/gui/baselineedit.h \
            src/gui/basetoolbar.h \
            src/gui/colorlabel.h \
            src/gui/comboboxwithstatus.h \
            src/gui/dialogs/formabout.h \
            src/gui/dialogs/formmain.h \
            src/gui/dialogs/formsettings.h \
            src/gui/dialogs/formupdate.h \
            src/gui/edittableview.h \
            src/gui/guiutilities.h \
            src/gui/labelwithstatus.h \
            src/gui/lineeditwithstatus.h \
            src/gui/messagebox.h \
            src/gui/plaintoolbutton.h \
            src/gui/settings/settingsbrowsermail.h \
            src/gui/settings/settingsdownloads.h \
            src/gui/settings/settingsgeneral.h \
            src/gui/settings/settingsgui.h \
            src/gui/settings/settingslocalization.h \
            src/gui/settings/settingspanel.h \
            src/gui/settings/settingsshortcuts.h \
            src/gui/squeezelabel.h \
            src/gui/statusbar.h \
            src/gui/styleditemdelegatewithoutfocus.h \
            src/gui/tabbar.h \
            src/gui/tabcontent.h \
            src/gui/tabwidget.h \
            src/gui/timespinbox.h \
            src/gui/toolbareditor.h \
            src/gui/widgetwithstatus.h \
            src/miscellaneous/application.h \
            src/miscellaneous/autosaver.h \
            src/miscellaneous/debugging.h \
            src/miscellaneous/iconfactory.h \
            src/miscellaneous/iofactory.h \
            src/miscellaneous/localization.h \
            src/miscellaneous/mutex.h \
            src/miscellaneous/settings.h \
            src/miscellaneous/simplecrypt/simplecrypt.h \
            src/miscellaneous/systemfactory.h \
            src/miscellaneous/textfactory.h \
            src/miscellaneous/settingsproperties.h \
            src/network-web/basenetworkaccessmanager.h \
            src/network-web/downloader.h \
            src/network-web/downloadmanager.h \
            src/network-web/networkfactory.h \
            src/network-web/silentnetworkaccessmanager.h \
            src/network-web/webfactory.h \
            src/qtsingleapplication/qtlocalpeer.h \
            src/qtsingleapplication/qtlockedfile.h \
            src/qtsingleapplication/qtsingleapplication.h \
            src/qtsingleapplication/qtsinglecoreapplication.h \
            src/gui/texteditor.h \
    src/miscellaneous/textapplication.h

SOURCES +=  src/dynamic-shortcuts/dynamicshortcuts.cpp \
            src/dynamic-shortcuts/dynamicshortcutswidget.cpp \
            src/dynamic-shortcuts/shortcutbutton.cpp \
            src/dynamic-shortcuts/shortcutcatcher.cpp \
            src/exceptions/applicationexception.cpp \
            src/exceptions/ioexception.cpp \
            src/gui/baselineedit.cpp \
            src/gui/basetoolbar.cpp \
            src/gui/colorlabel.cpp \
            src/gui/comboboxwithstatus.cpp \
            src/gui/dialogs/formabout.cpp \
            src/gui/dialogs/formmain.cpp \
            src/gui/dialogs/formsettings.cpp \
            src/gui/dialogs/formupdate.cpp \
            src/gui/edittableview.cpp \
            src/gui/guiutilities.cpp \
            src/gui/labelwithstatus.cpp \
            src/gui/lineeditwithstatus.cpp \
            src/gui/messagebox.cpp \
            src/gui/plaintoolbutton.cpp \
            src/gui/settings/settingsbrowsermail.cpp \
            src/gui/settings/settingsdownloads.cpp \
            src/gui/settings/settingsgeneral.cpp \
            src/gui/settings/settingsgui.cpp \
            src/gui/settings/settingslocalization.cpp \
            src/gui/settings/settingspanel.cpp \
            src/gui/settings/settingsshortcuts.cpp \
            src/gui/squeezelabel.cpp \
            src/gui/statusbar.cpp \
            src/gui/styleditemdelegatewithoutfocus.cpp \
            src/gui/tabbar.cpp \
            src/gui/tabcontent.cpp \
            src/gui/tabwidget.cpp \
            src/gui/timespinbox.cpp \
            src/gui/toolbareditor.cpp \
            src/gui/widgetwithstatus.cpp \
            src/main.cpp \
            src/miscellaneous/application.cpp \
            src/miscellaneous/autosaver.cpp \
            src/miscellaneous/debugging.cpp \
            src/miscellaneous/iconfactory.cpp \
            src/miscellaneous/iofactory.cpp \
            src/miscellaneous/localization.cpp \
            src/miscellaneous/mutex.cpp \
            src/miscellaneous/settings.cpp \
            src/miscellaneous/simplecrypt/simplecrypt.cpp \
            src/miscellaneous/systemfactory.cpp \
            src/miscellaneous/textfactory.cpp \
            src/network-web/basenetworkaccessmanager.cpp \
            src/network-web/downloader.cpp \
            src/network-web/downloadmanager.cpp \
            src/network-web/networkfactory.cpp \
            src/network-web/silentnetworkaccessmanager.cpp \
            src/network-web/webfactory.cpp \
            src/qtsingleapplication/qtlocalpeer.cpp \
            src/qtsingleapplication/qtlockedfile.cpp \
            src/qtsingleapplication/qtsingleapplication.cpp \
            src/qtsingleapplication/qtsinglecoreapplication.cpp \
            src/gui/texteditor.cpp \
    src/miscellaneous/textapplication.cpp

mac {
  OBJECTIVE_SOURCES += src/miscellaneous/disablewindowtabbing.mm
}

FORMS +=    src/gui/dialogs/formabout.ui \
            src/gui/dialogs/formmain.ui \
            src/gui/dialogs/formsettings.ui \
            src/gui/dialogs/formupdate.ui \
            src/gui/settings/settingsbrowsermail.ui \
            src/gui/settings/settingsdownloads.ui \
            src/gui/settings/settingsgeneral.ui \
            src/gui/settings/settingsgui.ui \
            src/gui/settings/settingslocalization.ui \
            src/gui/settings/settingsshortcuts.ui \
            src/gui/toolbareditor.ui \
            src/network-web/downloaditem.ui \
            src/network-web/downloadmanager.ui

TRANSLATIONS += $$PWD/localization/scipio_en_GB.ts \
                $$PWD/localization/scipio_en.ts

INCLUDEPATH +=  $$PWD/. \
                $$PWD/src \
                $$PWD/src/gui \
                $$PWD/src/gui/dialogs \
                $$PWD/src/dynamic-shortcuts

# Make sure QM translations are gnerated.
lrelease.input = TRANSLATIONS
lrelease.output = $$PWD/resources/localizations/${QMAKE_FILE_BASE}.qm
lrelease.commands = $$LRELEASE_EXECUTABLE -compress ${QMAKE_FILE_IN} -qm $$PWD/resources/localizations/${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link target_predeps

# Create new "make lupdate" target.
lupdate.target = lupdate
lupdate.commands = lupdate $$shell_path($$PWD/scipio.pro)

QMAKE_EXTRA_TARGETS += lupdate
QMAKE_EXTRA_COMPILERS += lrelease

# Create new "make 7zip" target and "make zip" target.
win32 {
  seven_zip.target = 7zip
  seven_zip.depends = install
  seven_zip.commands = $$shell_path($$shell_quote($$PWD/resources/scripts/7za/7za.exe)) a -t7z $$TARGET-$$APP_VERSION-$$APP_REVISION-$${APP_WIN_ARCH}.7z $$shell_path($$PREFIX/*)

  zip.target = zip
  zip.depends = install
  zip.commands = $$shell_path($$shell_quote($$PWD/resources/scripts/7za/7za.exe)) a -tzip $$TARGET-$$APP_VERSION-$$APP_REVISION-$${APP_WIN_ARCH}.zip $$shell_path($$PREFIX/*)

  QMAKE_EXTRA_TARGETS += seven_zip zip
}

mac {
  dmg.target = dmg
  dmg.depends = install
  dmg.commands = macdeployqt $$shell_quote($$shell_path($$PREFIX)) -dmg

  QMAKE_EXTRA_TARGETS += dmg
}

# Create NSIS installer target on Windows.
win32 {
  nsis.target = nsis
  nsis.depends = install
  nsis.commands = \
    $$shell_path($$shell_quote($$PWD/resources/scripts/sed/sed.exe)) -e \"s|@APP_VERSION@|$$APP_VERSION|g; s|@APP_WIN_ARCH@|$$APP_WIN_ARCH|g; s|@APP_REVISION@|$$APP_REVISION|g; s|@APP_NAME@|$$APP_NAME|g; s|@APP_LOW_NAME@|$$APP_LOW_NAME|g; s|@EXE_NAME@|$${APP_LOW_NAME}.exe|g; s|@PWD@|$$replace(PWD, /, \\\\)|g; s|@OUT_PWD@|$$replace(OUT_PWD, /, \\\\)|g\" $$shell_path($$shell_quote($$PWD/resources/nsis/NSIS.definitions.nsh.in)) > $$shell_path($$shell_quote($$OUT_PWD/NSIS.definitions.nsh)) && \
    xcopy /Y $$shell_path($$shell_quote($$PWD/resources/nsis/NSIS.template.in)) $$shell_path($$shell_quote($$OUT_PWD/)) && \
    $$shell_path($$shell_quote($$PWD/resources/scripts/nsis/makensis.exe)) $$shell_path($$shell_quote($$OUT_PWD/NSIS.template.in))

  QMAKE_EXTRA_TARGETS += nsis
}

win32 {
  windows_all.target = windows_all
  windows_all.depends = seven_zip nsis
  windows_all.commands = echo "windows_all done..."

  QMAKE_EXTRA_TARGETS += windows_all
}

# Install all files on Windows.
win32 {
  target.path = $$PREFIX

  qt_dlls_root.files = resources/binaries/windows/qt5-msvc2015/*.*
  qt_dlls_root.path = $$quote($$PREFIX/)

  qt_dlls_plugins.files = resources/binaries/windows/qt5-msvc2015/*
  qt_dlls_plugins.path = $$quote($$PREFIX/)

  INSTALLS += target qt_dlls_root qt_dlls_plugins
}

# Install all files on Linux.
unix:!mac:!android {
  target.path = $$PREFIX/bin

  desktop_file.files = resources/desktop/$${APP_REVERSE_NAME}.desktop
  desktop_file.path = $$quote($$PREFIX/share/applications/)

  appdata.files = resources/desktop/$${APP_REVERSE_NAME}.appdata.xml
  appdata.path = $$quote($$PREFIX/share/metainfo/)

  desktop_icon.files = resources/graphics/$${TARGET}.png
  desktop_icon.path = $$quote($$PREFIX/share/pixmaps/)

  INSTALLS += target desktop_file desktop_icon appdata
}

android {
  target.path = $$PREFIX

  INSTALLS += target
}

mac {
  IDENTIFIER = org.$${TARGET}.scipio
  CONFIG -= app_bundle
  ICON = resources/macosx/$${TARGET}.icns
  QMAKE_MAC_SDK = macosx10.12
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
  LIBS += -framework AppKit

  target.path = $$quote($$PREFIX/Contents/MacOS/)

  # Install app icon.
  icns_icon.files = resources/macosx/$${TARGET}.icns
  icns_icon.path = $$quote($$PREFIX/Contents/Resources/)

  # Install Info.plist.
  info_plist.files = resources/macosx/Info.plist.in
  info_plist.path  = $$quote($$PREFIX/Contents/)

  # Process the just installed Info.plist.
  info_plist2.extra = @sed -e "s,@EXECUTABLE@,$$TARGET,g" -e "s,@SHORT_VERSION@,$$APP_VERSION,g" -e "s,@APP_NAME@,\"$$APP_NAME\",g" -e "s,@ICON@,$$basename(ICON),g"  -e "s,@TYPEINFO@,"????",g" $$shell_quote($$PREFIX/Contents/Info.plist.in) > $$shell_quote($$PREFIX/Contents/Info.plist) && \
                      rm -f $$shell_quote($$PREFIX/Contents/Info.plist.in)
  info_plist2.path = $$quote($$PREFIX/Contents/)

  # Install PkgInfo
  pkginfo.extra = @printf "APPL????" > $$shell_quote($$PREFIX/Contents/PkgInfo)
  pkginfo.path = $$quote($$PREFIX/Contents/)

  INSTALLS += target icns_icon info_plist info_plist2 pkginfo
}
