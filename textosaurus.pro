#################################################################
#
# For license of this file, see <project-root-folder>/LICENSE.md.
#
# This is Textosaurus compilation script for qmake.
#
# Usage:
#   cd ../build-dir
#   qmake ../textosaurus-dir/textosaurus.pro -r CONFIG+=release PREFIX=./usr
#   make
#   make install
#
# Variables:
#   PREFIX - specifies base folder to which files are copied during "make install"
#            step, defaults to "$$OUT_PWD/usr" on Linux and to "$$OUT_PWD/app" on Windows.
#   LRELEASE_EXECUTABLE - specifies the name/path of "lrelease" executable, defaults to "lrelease".
#
# Other information:
#   - supports Windows or Linux,
#   - supports static Qt,
#   - Qt 5.7.0 or higher is required,
#   - C++ 17 is required.
#
# Authors and contributors:
#   - Martin Rotter (project leader).
#
#################################################################

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

exists(.git) {
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

QT *= core gui widgets network printsupport svg

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

  msvc:static {
    message($$MSG_PREFIX: Setting up LTCG.)

    QMAKE_LFLAGS += /LTCG
  }

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

DISTFILES += resources/scripts/uncrustify/uncrustify.cfg

MOC_DIR = $$OUT_PWD/moc
RCC_DIR = $$OUT_PWD/rcc
UI_DIR = $$OUT_PWD/ui

mac {
  QT *= macextras
}

# Make needed tweaks for RC file getting generated on Windows.
win32 {
  RC_ICONS = resources/graphics/textosaurus.ico
  QMAKE_TARGET_COMPANY = $$APP_AUTHOR
  QMAKE_TARGET_DESCRIPTION = $$APP_NAME
  QMAKE_TARGET_COPYRIGHT = $$APP_COPYRIGHT
  QMAKE_TARGET_PRODUCT = $$APP_NAME
}

CONFIG += resources_small
RESOURCES += resources/textosaurus.qrc

mac|win32|android {
  RESOURCES += resources/icons.qrc

  message($$MSG_PREFIX: Adding resources for default icon theme.)
}

HEADERS +=  src/definitions/definitions.h \
            src/common/dynamic-shortcuts/dynamicshortcuts.h \
            src/common/dynamic-shortcuts/dynamicshortcutswidget.h \
            src/common/dynamic-shortcuts/shortcutcatcher.h \
            src/common/exceptions/applicationexception.h \
            src/common/exceptions/ioexception.h \
            src/saurus/external-tools/externaltool.h \
            src/saurus/external-tools/externaltools.h \
            src/saurus/external-tools/predefinedtools.h \
            src/common/gui/baselineedit.h \
            src/common/gui/basetoolbar.h \
            src/saurus/gui/dialogs/formabout.h \
            src/saurus/gui/dialogs/formfindreplace.h \
            src/saurus/gui/dialogs/formmain.h \
            src/saurus/gui/dialogs/formsettings.h \
            src/saurus/gui/dialogs/formupdate.h \
            src/common/gui/guiutilities.h \
            src/common/gui/labelwithstatus.h \
            src/common/gui/messagebox.h \
            src/common/gui/plaintoolbutton.h \
            src/saurus/gui/settings/settingsbrowsermail.h \
            src/saurus/gui/settings/settingseditor.h \
            src/saurus/gui/settings/settingsexternaltools.h \
            src/saurus/gui/settings/settingsgeneral.h \
            src/saurus/gui/settings/settingsgui.h \
            src/saurus/gui/settings/settingslocalization.h \
            src/saurus/gui/settings/settingspanel.h \
            src/saurus/gui/settings/settingsshortcuts.h \
            src/saurus/gui/statusbar.h \
            src/saurus/gui/tabbar.h \
            src/saurus/gui/tabwidget.h \
            src/saurus/gui/texteditor.h \
            src/saurus/gui/texteditorprinter.h \
            src/common/gui/toolbar.h \
            src/common/gui/toolbareditor.h \
            src/common/gui/widgetwithstatus.h \
            src/saurus/miscellaneous/application.h \
            src/common/miscellaneous/debugging.h \
            src/common/miscellaneous/iconfactory.h \
            src/common/miscellaneous/iofactory.h \
            src/common/miscellaneous/localization.h \
            src/common/miscellaneous/settings.h \
            src/common/miscellaneous/settingsproperties.h \
            src/saurus/miscellaneous/syntaxhighlighting.h \
            src/common/miscellaneous/systemfactory.h \
            src/saurus/miscellaneous/textapplication.h \
            src/saurus/miscellaneous/textapplicationsettings.h \
            src/common/miscellaneous/textfactory.h \
            src/common/network-web/basenetworkaccessmanager.h \
            src/common/network-web/downloader.h \
            src/common/network-web/networkfactory.h \
            src/common/network-web/silentnetworkaccessmanager.h \
            src/common/network-web/webfactory.h \
            src/saurus/gui/sidebars/outputsidebar.h \
            src/saurus/gui/sidebars/findresultssidebar.h \
            src/saurus/gui/sidebars/findresultsmodel.h \
            src/saurus/gui/sidebars/findresultsmodelitem.h \
            src/saurus/gui/sidebars/findresultsmodelitemeditor.h \
            src/saurus/gui/sidebars/findresultsmodelitemresult.h \
            src/saurus/plugin-system/markdown/markdownsidebar.h \
            src/saurus/plugin-system/pluginfactory.h \
            src/saurus/plugin-system/pluginbase.h \
            src/saurus/plugin-system/markdown/markdownplugin.h \
            src/saurus/plugin-system/filesystem/filesystemsidebar.h \
            src/saurus/plugin-system/filesystem/filesystemplugin.h \
            src/saurus/gui/sidebars/basesidebar.h \
            src/saurus/plugin-system/markdown/markdowntextbrowser.h \
            src/saurus/plugin-system/filesystem/filesystemview.h \
            src/saurus/plugin-system/filesystem/favoriteslistwidget.h \
            src/saurus/miscellaneous/syntaxcolortheme.h \
            src/saurus/plugin-system/filesystem/filesystemmodel.h \
            src/saurus/gui/settings/syntaxcolorthemeeditor.h \
            src/saurus/miscellaneous/macro.h \
            src/saurus/gui/sidebars/macrossidebar.h \
    src/saurus/gui/tab.h \
    src/saurus/gui/editortab.h \
    src/common/gui/systemtrayicon.h \
    src/common/gui/basetextedit.h

SOURCES +=  src/common/dynamic-shortcuts/dynamicshortcuts.cpp \
            src/common/dynamic-shortcuts/dynamicshortcutswidget.cpp \
            src/common/dynamic-shortcuts/shortcutcatcher.cpp \
            src/common/exceptions/applicationexception.cpp \
            src/common/exceptions/ioexception.cpp \
            src/saurus/external-tools/externaltool.cpp \
            src/saurus/external-tools/externaltools.cpp \
            src/saurus/external-tools/predefinedtools.cpp \
            src/common/gui/baselineedit.cpp \
            src/common/gui/basetoolbar.cpp \
            src/saurus/gui/dialogs/formabout.cpp \
            src/saurus/gui/dialogs/formfindreplace.cpp \
            src/saurus/gui/dialogs/formmain.cpp \
            src/saurus/gui/dialogs/formsettings.cpp \
            src/saurus/gui/dialogs/formupdate.cpp \
            src/common/gui/guiutilities.cpp \
            src/common/gui/labelwithstatus.cpp \
            src/common/gui/messagebox.cpp \
            src/common/gui/plaintoolbutton.cpp \
            src/saurus/gui/settings/settingsbrowsermail.cpp \
            src/saurus/gui/settings/settingseditor.cpp \
            src/saurus/gui/settings/settingsexternaltools.cpp \
            src/saurus/gui/settings/settingsgeneral.cpp \
            src/saurus/gui/settings/settingsgui.cpp \
            src/saurus/gui/settings/settingslocalization.cpp \
            src/saurus/gui/settings/settingspanel.cpp \
            src/saurus/gui/settings/settingsshortcuts.cpp \
            src/saurus/gui/statusbar.cpp \
            src/saurus/gui/tabbar.cpp \
            src/saurus/gui/tabwidget.cpp \
            src/saurus/gui/texteditor.cpp \
            src/saurus/gui/texteditorprinter.cpp \
            src/common/gui/toolbar.cpp \
            src/common/gui/toolbareditor.cpp \
            src/common/gui/widgetwithstatus.cpp \
            src/main.cpp \
            src/saurus/miscellaneous/application.cpp \
            src/common/miscellaneous/debugging.cpp \
            src/common/miscellaneous/iconfactory.cpp \
            src/common/miscellaneous/iofactory.cpp \
            src/common/miscellaneous/localization.cpp \
            src/common/miscellaneous/settings.cpp \
            src/saurus/miscellaneous/syntaxhighlighting.cpp \
            src/common/miscellaneous/systemfactory.cpp \
            src/saurus/miscellaneous/textapplication.cpp \
            src/saurus/miscellaneous/textapplicationsettings.cpp \
            src/common/miscellaneous/textfactory.cpp \
            src/common/network-web/basenetworkaccessmanager.cpp \
            src/common/network-web/downloader.cpp \
            src/common/network-web/networkfactory.cpp \
            src/common/network-web/silentnetworkaccessmanager.cpp \
            src/common/network-web/webfactory.cpp \
            src/saurus/gui/sidebars/outputsidebar.cpp \
            src/saurus/gui/sidebars/findresultssidebar.cpp \
            src/saurus/gui/sidebars/findresultsmodel.cpp \
            src/saurus/gui/sidebars/findresultsmodelitem.cpp \
            src/saurus/gui/sidebars/findresultsmodelitemeditor.cpp \
            src/saurus/gui/sidebars/findresultsmodelitemresult.cpp \
            src/saurus/plugin-system/markdown/markdownsidebar.cpp \
            src/saurus/plugin-system/pluginfactory.cpp \
            src/saurus/plugin-system/markdown/markdownplugin.cpp \
            src/saurus/plugin-system/filesystem/filesystemsidebar.cpp \
            src/saurus/plugin-system/filesystem/filesystemplugin.cpp \
            src/saurus/gui/sidebars/basesidebar.cpp \
            src/saurus/plugin-system/markdown/markdowntextbrowser.cpp \
            src/saurus/plugin-system/filesystem/filesystemview.cpp \
            src/saurus/plugin-system/filesystem/favoriteslistwidget.cpp \
            src/saurus/miscellaneous/syntaxcolortheme.cpp \
            src/saurus/plugin-system/filesystem/filesystemmodel.cpp \
            src/saurus/gui/settings/syntaxcolorthemeeditor.cpp \
            src/saurus/miscellaneous/macro.cpp \
            src/saurus/gui/sidebars/macrossidebar.cpp \
    src/saurus/gui/tab.cpp \
    src/saurus/gui/editortab.cpp \
    src/common/gui/systemtrayicon.cpp \
    src/common/gui/basetextedit.cpp

mac {
  OBJECTIVE_SOURCES += src/common/miscellaneous/disablewindowtabbing.mm
}

FORMS +=    src/saurus/gui/dialogs/formabout.ui \
            src/saurus/gui/dialogs/formmain.ui \
            src/saurus/gui/dialogs/formsettings.ui \
            src/saurus/gui/dialogs/formupdate.ui \
            src/saurus/gui/dialogs/formfindreplace.ui \
            src/saurus/gui/settings/settingsbrowsermail.ui \
            src/saurus/gui/settings/settingsgeneral.ui \
            src/saurus/gui/settings/settingsgui.ui \
            src/saurus/gui/settings/settingsexternaltools.ui \
            src/saurus/gui/settings/settingslocalization.ui \
            src/saurus/gui/settings/settingsshortcuts.ui \
            src/common/gui/toolbareditor.ui \
            src/saurus/gui/settings/settingseditor.ui \
            src/saurus/gui/settings/syntaxcolorthemeeditor.ui \
            src/saurus/gui/sidebars/macrossidebar.ui

# Add qtsingleapplication.
SOURCES += $$files(src/3rd-party/qtsingleapplication/*.cpp, false)
HEADERS  += $$files(src/3rd-party/qtsingleapplication/*.h, false)

# Add uchardet.
SOURCES += $$files(src/3rd-party/uchardet/*.cpp, false)
HEADERS  += $$files(src/3rd-party/uchardet/*.h, false)

# Add Scintilla.
SOURCES += \
    src/3rd-party/scintilla/qt/ScintillaEdit/ScintillaEdit.cpp \
    src/3rd-party/scintilla/qt/ScintillaEdit/ScintillaDocument.cpp \
    src/3rd-party/scintilla/qt/ScintillaEditBase/PlatQt.cpp \
    src/3rd-party/scintilla/qt/ScintillaEditBase/ScintillaQt.cpp \
    src/3rd-party/scintilla/qt/ScintillaEditBase/ScintillaEditBase.cpp \
    src/3rd-party/scintilla/src/XPM.cxx \
    src/3rd-party/scintilla/src/ViewStyle.cxx \
    src/3rd-party/scintilla/src/UniConversion.cxx \
    src/3rd-party/scintilla/src/Style.cxx \
    src/3rd-party/scintilla/src/Selection.cxx \
    src/3rd-party/scintilla/src/ScintillaBase.cxx \
    src/3rd-party/scintilla/src/RunStyles.cxx \
    src/3rd-party/scintilla/src/RESearch.cxx \
    src/3rd-party/scintilla/src/PositionCache.cxx \
    src/3rd-party/scintilla/src/PerLine.cxx \
    src/3rd-party/scintilla/src/MarginView.cxx \
    src/3rd-party/scintilla/src/LineMarker.cxx \
    src/3rd-party/scintilla/src/KeyMap.cxx \
    src/3rd-party/scintilla/src/Indicator.cxx \
    src/3rd-party/scintilla/src/ExternalLexer.cxx \
    src/3rd-party/scintilla/src/EditView.cxx \
    src/3rd-party/scintilla/src/Editor.cxx \
    src/3rd-party/scintilla/src/EditModel.cxx \
    src/3rd-party/scintilla/src/Document.cxx \
    src/3rd-party/scintilla/src/Decoration.cxx \
    src/3rd-party/scintilla/src/DBCS.cxx \
    src/3rd-party/scintilla/src/ContractionState.cxx \
    src/3rd-party/scintilla/src/CharClassify.cxx \
    src/3rd-party/scintilla/src/CellBuffer.cxx \
    src/3rd-party/scintilla/src/Catalogue.cxx \
    src/3rd-party/scintilla/src/CaseFolder.cxx \
    src/3rd-party/scintilla/src/CaseConvert.cxx \
    src/3rd-party/scintilla/src/CallTip.cxx \
    src/3rd-party/scintilla/src/AutoComplete.cxx \
    src/3rd-party/scintilla/lexlib/WordList.cxx \
    src/3rd-party/scintilla/lexlib/StyleContext.cxx \
    src/3rd-party/scintilla/lexlib/PropSetSimple.cxx \
    src/3rd-party/scintilla/lexlib/LexerSimple.cxx \
    src/3rd-party/scintilla/lexlib/LexerNoExceptions.cxx \
    src/3rd-party/scintilla/lexlib/LexerModule.cxx \
    src/3rd-party/scintilla/lexlib/LexerBase.cxx \
    src/3rd-party/scintilla/lexlib/DefaultLexer.cxx \
    src/3rd-party/scintilla/lexlib/CharacterSet.cxx \
    src/3rd-party/scintilla/lexlib/CharacterCategory.cxx \
    src/3rd-party/scintilla/lexlib/Accessor.cxx \
    $$files(src/3rd-party/scintilla/lexers/*.cxx, false)

HEADERS  += \
    src/3rd-party/scintilla/qt/ScintillaEdit/ScintillaEdit.h \
    src/3rd-party/scintilla/qt/ScintillaEdit/ScintillaDocument.h \
    src/3rd-party/scintilla/qt/ScintillaEditBase/ScintillaEditBase.h \
    src/3rd-party/scintilla/qt/ScintillaEditBase/ScintillaQt.h

INCLUDEPATH += src/3rd-party/scintilla/qt/ScintillaEditBase src/3rd-party/scintilla/include src/3rd-party/scintilla/src src/3rd-party/scintilla/lexlib

DEFINES *= SCINTILLA_QT=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1 EXPORT_IMPORT_API=

CONFIG(release, debug|release) {
  DEFINES *= NDEBUG=1
}

# Add hoextdown.
SOURCES += $$files(src/3rd-party/hoedown/*.c, false)
HEADERS  += $$files(src/3rd-party/hoedown/*.h, false)

INCLUDEPATH +=  $$PWD/. \
                $$PWD/src \
                $$PWD/src/common/gui \
                $$PWD/src/saurus/gui \
                $$PWD/src/saurus/gui/dialogs \
                $$PWD/src/saurus/gui/sidebars \
                $$PWD/src/saurus/gui/settings \
                $$PWD/src/common/dynamic-shortcuts \
                $$PWD/src/saurus/external-tools \
                $$PWD/src/saurus/plugin-system

TRANSLATIONS += $$PWD/localization/textosaurus_en_GB.ts \
                $$PWD/localization/textosaurus_en.ts

# Create new "make lupdate" target.
lupdate.target = lupdate
lupdate.commands = lupdate $$shell_path($$PWD/textosaurus.pro)

QMAKE_EXTRA_TARGETS += lupdate

# Make sure QM translations are nerated.
qtPrepareTool(LRELEASE, lrelease) {
  message($$MSG_PREFIX: Running: $$LRELEASE_EXECUTABLE -compress $$shell_quote($$shell_path($$PWD/textosaurus.pro)))
  system($$LRELEASE_EXECUTABLE -compress textosaurus.pro)
}

static {
  message($$MSG_PREFIX: Building static version of application.)

  QTPLUGIN.bearer = -
  QTPLUGIN.generic = -
  QTPLUGIN.sqldrivers = -
  QTPLUGIN.iconengines = qsvgicon
  QTPLUGIN.platforms = qwindows
  QTPLUGIN.imageformats = qgif qico qjpeg qsvg qwbmp
  QTPLUGIN.printsupport = windowsprintersupport
  QTPLUGIN.styles = qwindowsvistastyle
}
else {
  message($$MSG_PREFIX: Building shared version of application.)
}

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

# Install all files on Windows.
win32 {
  target.path = $$PREFIX

  INSTALLS += target
}

# Install all files on Linux.
unix:!mac:!android {
  target.path = $$PREFIX/bin

  desktop_file.files = resources/desktop/$${APP_REVERSE_NAME}.desktop
  desktop_file.path = $$quote($$PREFIX/share/applications/)

  appdata.files = resources/desktop/$${APP_REVERSE_NAME}.appdata.xml
  appdata.path = $$quote($$PREFIX/share/metainfo/)

  desktop_icon.files = resources/graphics/$${TARGET}.png
  desktop_icon.path = $$quote($$PREFIX/share/icons/hicolor/512x512/apps/)

  INSTALLS += target desktop_file desktop_icon appdata
}

android {
  target.path = $$PREFIX

  INSTALLS += target
}

mac {
  IDENTIFIER = org.$${TARGET}.textosaurus
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
