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
APP_REVERSE_NAME              = "com.github.textosaurus"
APP_AUTHOR                    = "Martin Rotter"
APP_COPYRIGHT                 = "(C) 2018 $$APP_AUTHOR"
APP_VERSION                   = "2018.03.05"
APP_LONG_NAME                 = "$$APP_NAME $$APP_VERSION"
APP_EMAIL                     = "rotter.martinos@gmail.com"
APP_URL                       = "https://github.com/martinrotter/textosaurus"
APP_URL_ISSUES                = "https://github.com/martinrotter/textosaurus/issues"
APP_URL_ISSUES_NEW            = "https://github.com/martinrotter/textosaurus/issues/new"
APP_URL_WIKI                  = "https://github.com/martinrotter/textosaurus/wiki"
APP_USERAGENT                 = "Textosaurus/$$APP_VERSION (github.com/martinrotter/textosaurus)"
APP_DONATE_URL                = "https://liberapay.com/martinrotter"
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
  APP_REVISION = ""
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

    #QMAKE_CXXFLAGS += /O1QMAKE_CXXFLAGS +=  /GL
    QMAKE_LFLAGS += /LTCG
  }

  gcc:QMAKE_CXXFLAGS_RELEASE *= -O3
  clang:QMAKE_CXXFLAGS_RELEASE *= -O3
}
else {
  message($$MSG_PREFIX: Building in "debug" mode.)

  gcc:QMAKE_CXXFLAGS_DEBUG *= -Wall
  clang:QMAKE_CXXFLAGS_DEBUG *= -Wall
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
            src/dynamic-shortcuts/dynamicshortcuts.h \
            src/dynamic-shortcuts/dynamicshortcutswidget.h \
            src/dynamic-shortcuts/shortcutcatcher.h \
            src/exceptions/applicationexception.h \
            src/exceptions/ioexception.h \
            src/external-tools/externaltool.h \
            src/external-tools/externaltools.h \
            src/external-tools/predefinedtools.h \
            src/gui/baselineedit.h \
            src/gui/basetoolbar.h \
            src/gui/dialogs/formabout.h \
            src/gui/dialogs/formfindreplace.h \
            src/gui/dialogs/formmain.h \
            src/gui/dialogs/formsettings.h \
            src/gui/dialogs/formupdate.h \
            src/gui/guiutilities.h \
            src/gui/labelwithstatus.h \
            src/gui/messagebox.h \
            src/gui/plaintoolbutton.h \
            src/gui/settings/settingsbrowsermail.h \
            src/gui/settings/settingseditor.h \
            src/gui/settings/settingsexternaltools.h \
            src/gui/settings/settingsgeneral.h \
            src/gui/settings/settingsgui.h \
            src/gui/settings/settingslocalization.h \
            src/gui/settings/settingspanel.h \
            src/gui/settings/settingsshortcuts.h \
            src/gui/statusbar.h \
            src/gui/tabbar.h \
            src/gui/tabwidget.h \
            src/gui/texteditor.h \
            src/gui/texteditorprinter.h \
            src/gui/toolbar.h \
            src/gui/toolbareditor.h \
            src/gui/widgetwithstatus.h \
            src/miscellaneous/application.h \
            src/miscellaneous/debugging.h \
            src/miscellaneous/iconfactory.h \
            src/miscellaneous/iofactory.h \
            src/miscellaneous/localization.h \
            src/miscellaneous/settings.h \
            src/miscellaneous/settingsproperties.h \
            src/miscellaneous/syntaxhighlighting.h \
            src/miscellaneous/systemfactory.h \
            src/miscellaneous/textapplication.h \
            src/miscellaneous/textapplicationsettings.h \
            src/miscellaneous/textfactory.h \
            src/network-web/basenetworkaccessmanager.h \
            src/network-web/downloader.h \
            src/network-web/networkfactory.h \
            src/network-web/silentnetworkaccessmanager.h \
            src/network-web/webfactory.h \
            src/gui/sidebars/outputsidebar.h \
            src/gui/sidebars/findresultssidebar.h \
            src/gui/sidebars/findresultsmodel.h \
            src/gui/sidebars/findresultsmodelitem.h \
            src/gui/sidebars/findresultsmodelitemeditor.h \
            src/gui/sidebars/findresultsmodelitemresult.h \
            src/plugin-system/markdown/markdownsidebar.h \
            src/plugin-system/pluginfactory.h \
            src/plugin-system/pluginbase.h \
            src/plugin-system/markdown/markdownplugin.h \
            src/plugin-system/filesystem/filesystemsidebar.h \
            src/plugin-system/filesystem/filesystemplugin.h \
            src/gui/sidebars/basesidebar.h \
            src/plugin-system/markdown/markdowntextbrowser.h \
            src/plugin-system/filesystem/filesystemview.h \
            src/plugin-system/filesystem/favoriteslistwidget.h \
            src/miscellaneous/syntaxcolortheme.h \
            src/plugin-system/filesystem/filesystemmodel.h \
            src/gui/settings/syntaxcolorthemeeditor.h

SOURCES +=  src/dynamic-shortcuts/dynamicshortcuts.cpp \
            src/dynamic-shortcuts/dynamicshortcutswidget.cpp \
            src/dynamic-shortcuts/shortcutcatcher.cpp \
            src/exceptions/applicationexception.cpp \
            src/exceptions/ioexception.cpp \
            src/external-tools/externaltool.cpp \
            src/external-tools/externaltools.cpp \
            src/external-tools/predefinedtools.cpp \
            src/gui/baselineedit.cpp \
            src/gui/basetoolbar.cpp \
            src/gui/dialogs/formabout.cpp \
            src/gui/dialogs/formfindreplace.cpp \
            src/gui/dialogs/formmain.cpp \
            src/gui/dialogs/formsettings.cpp \
            src/gui/dialogs/formupdate.cpp \
            src/gui/guiutilities.cpp \
            src/gui/labelwithstatus.cpp \
            src/gui/messagebox.cpp \
            src/gui/plaintoolbutton.cpp \
            src/gui/settings/settingsbrowsermail.cpp \
            src/gui/settings/settingseditor.cpp \
            src/gui/settings/settingsexternaltools.cpp \
            src/gui/settings/settingsgeneral.cpp \
            src/gui/settings/settingsgui.cpp \
            src/gui/settings/settingslocalization.cpp \
            src/gui/settings/settingspanel.cpp \
            src/gui/settings/settingsshortcuts.cpp \
            src/gui/statusbar.cpp \
            src/gui/tabbar.cpp \
            src/gui/tabwidget.cpp \
            src/gui/texteditor.cpp \
            src/gui/texteditorprinter.cpp \
            src/gui/toolbar.cpp \
            src/gui/toolbareditor.cpp \
            src/gui/widgetwithstatus.cpp \
            src/main.cpp \
            src/miscellaneous/application.cpp \
            src/miscellaneous/debugging.cpp \
            src/miscellaneous/iconfactory.cpp \
            src/miscellaneous/iofactory.cpp \
            src/miscellaneous/localization.cpp \
            src/miscellaneous/settings.cpp \
            src/miscellaneous/syntaxhighlighting.cpp \
            src/miscellaneous/systemfactory.cpp \
            src/miscellaneous/textapplication.cpp \
            src/miscellaneous/textapplicationsettings.cpp \
            src/miscellaneous/textfactory.cpp \
            src/network-web/basenetworkaccessmanager.cpp \
            src/network-web/downloader.cpp \
            src/network-web/networkfactory.cpp \
            src/network-web/silentnetworkaccessmanager.cpp \
            src/network-web/webfactory.cpp \
            src/gui/sidebars/outputsidebar.cpp \
            src/gui/sidebars/findresultssidebar.cpp \
            src/gui/sidebars/findresultsmodel.cpp \
            src/gui/sidebars/findresultsmodelitem.cpp \
            src/gui/sidebars/findresultsmodelitemeditor.cpp \
            src/gui/sidebars/findresultsmodelitemresult.cpp \
            src/plugin-system/markdown/markdownsidebar.cpp \
            src/plugin-system/pluginfactory.cpp \
            src/plugin-system/markdown/markdownplugin.cpp \
            src/plugin-system/filesystem/filesystemsidebar.cpp \
            src/plugin-system/filesystem/filesystemplugin.cpp \
            src/gui/sidebars/basesidebar.cpp \
            src/plugin-system/markdown/markdowntextbrowser.cpp \
            src/plugin-system/filesystem/filesystemview.cpp \
            src/plugin-system/filesystem/favoriteslistwidget.cpp \
            src/miscellaneous/syntaxcolortheme.cpp \
            src/plugin-system/filesystem/filesystemmodel.cpp \
            src/gui/settings/syntaxcolorthemeeditor.cpp

mac {
  OBJECTIVE_SOURCES += src/miscellaneous/disablewindowtabbing.mm
}

FORMS +=    src/gui/dialogs/formabout.ui \
            src/gui/dialogs/formmain.ui \
            src/gui/dialogs/formsettings.ui \
            src/gui/dialogs/formupdate.ui \
            src/gui/dialogs/formfindreplace.ui \
            src/gui/settings/settingsbrowsermail.ui \
            src/gui/settings/settingsgeneral.ui \
            src/gui/settings/settingsgui.ui \
            src/gui/settings/settingsexternaltools.ui \
            src/gui/settings/settingslocalization.ui \
            src/gui/settings/settingsshortcuts.ui \
            src/gui/toolbareditor.ui \
            src/gui/settings/settingseditor.ui \
            src/gui/settings/syntaxcolorthemeeditor.ui

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

# Add hunspell.
unix {
  LIBS += -lhunspell
}

win32 {
  debug {
    static {
      LIBS += $$PWD/resources/binaries/hunspell-msvc/debug-static/libhunspell.lib
    }
    else {
      LIBS += $$PWD/resources/binaries/hunspell-msvc/debug-shared/libhunspell.lib
    }
  }
  else {
    static {
      LIBS += $$PWD/resources/binaries/hunspell-msvc/release-static/libhunspell.lib
    }
    else {
      LIBS += $$PWD/resources/binaries/hunspell-msvc/release-shared/libhunspell.lib
    }
  }
}

INCLUDEPATH +=  $$PWD/. \
                $$PWD/src \
                $$PWD/src/gui \
                $$PWD/src/gui/dialogs \
                $$PWD/src/gui/sidebars \
                $$PWD/src/gui/settings \
                $$PWD/src/dynamic-shortcuts \
                $$PWD/src/external-tools \
                $$PWD/src/plugin-system \
                $$PWD/src/3rd-party

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

# Install all Mingw64 files on Windows.
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
  desktop_icon.path = $$quote($$PREFIX/share/pixmaps/)

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
