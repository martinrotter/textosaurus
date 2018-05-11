TEMPLATE    = lib
TARGET      = libtextosaurus

MSG_PREFIX                    = "libtextosaurus"
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
    PREFIX = $$OUT_PWD/AppDir/usr/lib
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

QT *= core gui widgets network printsupport svg

CONFIG *= c++1z warn_on
CONFIG -=  debug_and_release
DEFINES *= QT_USE_QSTRINGBUILDER QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS UNICODE _UNICODE
VERSION = $$APP_VERSION

win32 {
  LIBS *= Shell32.lib
}

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
  QMAKE_TARGET_COMPANY = $$APP_AUTHOR
  QMAKE_TARGET_DESCRIPTION = $$APP_NAME (core library)
  QMAKE_TARGET_COPYRIGHT = $$APP_COPYRIGHT
  QMAKE_TARGET_PRODUCT = $$APP_NAME
}

CONFIG += resources_small
RESOURCES += ../../resources/textosaurus.qrc

mac|win32 {
  RESOURCES += ../../resources/icons.qrc

  message($$MSG_PREFIX: Adding resources for default icon theme.)
}

HEADERS +=  common/dynamic-shortcuts/dynamicshortcuts.h \
            common/dynamic-shortcuts/dynamicshortcutswidget.h \
            common/dynamic-shortcuts/shortcutcatcher.h \
            common/exceptions/applicationexception.h \
            common/exceptions/ioexception.h \
            common/gui/baselineedit.h \
            common/gui/basetextedit.h \
            common/gui/basetoolbar.h \
            common/gui/guiutilities.h \
            common/gui/labelwithstatus.h \
            common/gui/messagebox.h \
            common/gui/plaintoolbutton.h \
            common/gui/systemtrayicon.h \
            common/gui/toolbar.h \
            common/gui/toolbareditor.h \
            common/gui/widgetwithstatus.h \
            common/miscellaneous/debugging.h \
            common/miscellaneous/iconfactory.h \
            common/miscellaneous/iofactory.h \
            common/miscellaneous/localization.h \
            common/miscellaneous/settings.h \
            common/miscellaneous/settingsproperties.h \
            common/miscellaneous/systemfactory.h \
            common/miscellaneous/textfactory.h \
            common/network-web/basenetworkaccessmanager.h \
            common/network-web/downloader.h \
            common/network-web/networkfactory.h \
            common/network-web/silentnetworkaccessmanager.h \
            common/network-web/webfactory.h \
            definitions/definitions.h \
            saurus/external-tools/externaltool.h \
            saurus/external-tools/externaltools.h \
            saurus/external-tools/predefinedtools.h \
            saurus/gui/dialogs/formabout.h \
            saurus/gui/dialogs/formfindreplace.h \
            saurus/gui/dialogs/formmain.h \
            saurus/gui/dialogs/formsettings.h \
            saurus/gui/dialogs/formupdate.h \
            saurus/gui/editortab.h \
            saurus/gui/settings/settingsbrowsermail.h \
            saurus/gui/settings/settingseditor.h \
            saurus/gui/settings/settingsexternaltools.h \
            saurus/gui/settings/settingsgeneral.h \
            saurus/gui/settings/settingsgui.h \
            saurus/gui/settings/settingslocalization.h \
            saurus/gui/settings/settingspanel.h \
            saurus/gui/settings/settingsshortcuts.h \
            saurus/gui/settings/syntaxcolorthemeeditor.h \
            saurus/gui/sidebars/basesidebar.h \
            saurus/gui/sidebars/findresultsmodel.h \
            saurus/gui/sidebars/findresultsmodelitem.h \
            saurus/gui/sidebars/findresultsmodelitemeditor.h \
            saurus/gui/sidebars/findresultsmodelitemresult.h \
            saurus/gui/sidebars/findresultssidebar.h \
            saurus/plugin-system/macros/macrossidebar.h \
            saurus/gui/sidebars/outputsidebar.h \
            saurus/gui/statusbar.h \
            saurus/gui/tab.h \
            saurus/gui/tabbar.h \
            saurus/gui/tabwidget.h \
            saurus/gui/texteditor.h \
            saurus/gui/texteditorprinter.h \
            saurus/miscellaneous/application.h \
            saurus/plugin-system/macros/macro.h \
            saurus/miscellaneous/syntaxcolortheme.h \
            saurus/miscellaneous/syntaxhighlighting.h \
            saurus/miscellaneous/textapplication.h \
            saurus/miscellaneous/textapplicationsettings.h \
            saurus/plugin-system/filesystem/favoriteslistwidget.h \
            saurus/plugin-system/filesystem/filesystemmodel.h \
            saurus/plugin-system/filesystem/filesystemplugin.h \
            saurus/plugin-system/filesystem/filesystemsidebar.h \
            saurus/plugin-system/filesystem/filesystemview.h \
            saurus/plugin-system/markdown/markdownplugin.h \
            saurus/plugin-system/markdown/markdownsidebar.h \
            saurus/plugin-system/markdown/markdowntextbrowser.h \
            saurus/plugin-system/pluginbase.h \
            saurus/plugin-system/pluginfactory.h \
            saurus/plugin-system/macros/macroswidget.h \
            saurus/plugin-system/macros/macros.h \
            saurus/plugin-system/macros/macrosplugin.h

SOURCES +=  common/dynamic-shortcuts/dynamicshortcuts.cpp \
            common/dynamic-shortcuts/dynamicshortcutswidget.cpp \
            common/dynamic-shortcuts/shortcutcatcher.cpp \
            common/exceptions/applicationexception.cpp \
            common/exceptions/ioexception.cpp \
            common/gui/baselineedit.cpp \
            common/gui/basetextedit.cpp \
            common/gui/basetoolbar.cpp \
            common/gui/guiutilities.cpp \
            common/gui/labelwithstatus.cpp \
            common/gui/messagebox.cpp \
            common/gui/plaintoolbutton.cpp \
            common/gui/systemtrayicon.cpp \
            common/gui/toolbar.cpp \
            common/gui/toolbareditor.cpp \
            common/gui/widgetwithstatus.cpp \
            common/miscellaneous/debugging.cpp \
            common/miscellaneous/iconfactory.cpp \
            common/miscellaneous/iofactory.cpp \
            common/miscellaneous/localization.cpp \
            common/miscellaneous/settings.cpp \
            common/miscellaneous/systemfactory.cpp \
            common/miscellaneous/textfactory.cpp \
            common/network-web/basenetworkaccessmanager.cpp \
            common/network-web/downloader.cpp \
            common/network-web/networkfactory.cpp \
            common/network-web/silentnetworkaccessmanager.cpp \
            common/network-web/webfactory.cpp \
            saurus/external-tools/externaltool.cpp \
            saurus/external-tools/externaltools.cpp \
            saurus/external-tools/predefinedtools.cpp \
            saurus/gui/dialogs/formabout.cpp \
            saurus/gui/dialogs/formfindreplace.cpp \
            saurus/gui/dialogs/formmain.cpp \
            saurus/gui/dialogs/formsettings.cpp \
            saurus/gui/dialogs/formupdate.cpp \
            saurus/gui/editortab.cpp \
            saurus/gui/settings/settingsbrowsermail.cpp \
            saurus/gui/settings/settingseditor.cpp \
            saurus/gui/settings/settingsexternaltools.cpp \
            saurus/gui/settings/settingsgeneral.cpp \
            saurus/gui/settings/settingsgui.cpp \
            saurus/gui/settings/settingslocalization.cpp \
            saurus/gui/settings/settingspanel.cpp \
            saurus/gui/settings/settingsshortcuts.cpp \
            saurus/gui/settings/syntaxcolorthemeeditor.cpp \
            saurus/gui/sidebars/basesidebar.cpp \
            saurus/gui/sidebars/findresultsmodel.cpp \
            saurus/gui/sidebars/findresultsmodelitem.cpp \
            saurus/gui/sidebars/findresultsmodelitemeditor.cpp \
            saurus/gui/sidebars/findresultsmodelitemresult.cpp \
            saurus/gui/sidebars/findresultssidebar.cpp \
            saurus/plugin-system/macros/macrossidebar.cpp \
            saurus/gui/sidebars/outputsidebar.cpp \
            saurus/gui/statusbar.cpp \
            saurus/gui/tab.cpp \
            saurus/gui/tabbar.cpp \
            saurus/gui/tabwidget.cpp \
            saurus/gui/texteditor.cpp \
            saurus/gui/texteditorprinter.cpp \
            saurus/miscellaneous/application.cpp \
            saurus/plugin-system/macros/macro.cpp \
            saurus/miscellaneous/syntaxcolortheme.cpp \
            saurus/miscellaneous/syntaxhighlighting.cpp \
            saurus/miscellaneous/textapplication.cpp \
            saurus/miscellaneous/textapplicationsettings.cpp \
            saurus/plugin-system/filesystem/favoriteslistwidget.cpp \
            saurus/plugin-system/filesystem/filesystemmodel.cpp \
            saurus/plugin-system/filesystem/filesystemplugin.cpp \
            saurus/plugin-system/filesystem/filesystemsidebar.cpp \
            saurus/plugin-system/filesystem/filesystemview.cpp \
            saurus/plugin-system/markdown/markdownplugin.cpp \
            saurus/plugin-system/markdown/markdownsidebar.cpp \
            saurus/plugin-system/markdown/markdowntextbrowser.cpp \
            saurus/plugin-system/pluginfactory.cpp \
            saurus/plugin-system/macros/macroswidget.cpp \
            saurus/plugin-system/macros/macros.cpp \
            saurus/plugin-system/macros/macrosplugin.cpp

mac {
  OBJECTIVE_SOURCES += common/miscellaneous/disablewindowtabbing.mm
}

FORMS +=  common/gui/toolbareditor.ui \
          saurus/gui/dialogs/formabout.ui \
          saurus/gui/dialogs/formfindreplace.ui \
          saurus/gui/dialogs/formmain.ui \
          saurus/gui/dialogs/formsettings.ui \
          saurus/gui/dialogs/formupdate.ui \
          saurus/gui/settings/settingsbrowsermail.ui \
          saurus/gui/settings/settingseditor.ui \
          saurus/gui/settings/settingsexternaltools.ui \
          saurus/gui/settings/settingsgeneral.ui \
          saurus/gui/settings/settingsgui.ui \
          saurus/gui/settings/settingslocalization.ui \
          saurus/gui/settings/settingsshortcuts.ui \
          saurus/gui/settings/syntaxcolorthemeeditor.ui \
          saurus/plugin-system/macros/macroswidget.ui

# Add qtsingleapplication.
SOURCES += $$files(3rd-party/qtsingleapplication/*.cpp, false)
HEADERS  += $$files(3rd-party/qtsingleapplication/*.h, false)

# Add uchardet.
SOURCES += $$files(3rd-party/uchardet/*.cpp, false)
HEADERS  += $$files(3rd-party/uchardet/*.h, false)

# Add Scintilla.
SOURCES += \
    3rd-party/scintilla/qt/ScintillaEdit/ScintillaEdit.cpp \
    3rd-party/scintilla/qt/ScintillaEdit/ScintillaDocument.cpp \
    3rd-party/scintilla/qt/ScintillaEditBase/PlatQt.cpp \
    3rd-party/scintilla/qt/ScintillaEditBase/ScintillaQt.cpp \
    3rd-party/scintilla/qt/ScintillaEditBase/ScintillaEditBase.cpp \
    3rd-party/scintilla/src/XPM.cxx \
    3rd-party/scintilla/src/ViewStyle.cxx \
    3rd-party/scintilla/src/UniConversion.cxx \
    3rd-party/scintilla/src/Style.cxx \
    3rd-party/scintilla/src/Selection.cxx \
    3rd-party/scintilla/src/ScintillaBase.cxx \
    3rd-party/scintilla/src/RunStyles.cxx \
    3rd-party/scintilla/src/RESearch.cxx \
    3rd-party/scintilla/src/PositionCache.cxx \
    3rd-party/scintilla/src/PerLine.cxx \
    3rd-party/scintilla/src/MarginView.cxx \
    3rd-party/scintilla/src/LineMarker.cxx \
    3rd-party/scintilla/src/KeyMap.cxx \
    3rd-party/scintilla/src/Indicator.cxx \
    3rd-party/scintilla/src/ExternalLexer.cxx \
    3rd-party/scintilla/src/EditView.cxx \
    3rd-party/scintilla/src/Editor.cxx \
    3rd-party/scintilla/src/EditModel.cxx \
    3rd-party/scintilla/src/Document.cxx \
    3rd-party/scintilla/src/Decoration.cxx \
    3rd-party/scintilla/src/DBCS.cxx \
    3rd-party/scintilla/src/ContractionState.cxx \
    3rd-party/scintilla/src/CharClassify.cxx \
    3rd-party/scintilla/src/CellBuffer.cxx \
    3rd-party/scintilla/src/Catalogue.cxx \
    3rd-party/scintilla/src/CaseFolder.cxx \
    3rd-party/scintilla/src/CaseConvert.cxx \
    3rd-party/scintilla/src/CallTip.cxx \
    3rd-party/scintilla/src/AutoComplete.cxx \
    3rd-party/scintilla/lexlib/WordList.cxx \
    3rd-party/scintilla/lexlib/StyleContext.cxx \
    3rd-party/scintilla/lexlib/PropSetSimple.cxx \
    3rd-party/scintilla/lexlib/LexerSimple.cxx \
    3rd-party/scintilla/lexlib/LexerNoExceptions.cxx \
    3rd-party/scintilla/lexlib/LexerModule.cxx \
    3rd-party/scintilla/lexlib/LexerBase.cxx \
    3rd-party/scintilla/lexlib/DefaultLexer.cxx \
    3rd-party/scintilla/lexlib/CharacterSet.cxx \
    3rd-party/scintilla/lexlib/CharacterCategory.cxx \
    3rd-party/scintilla/lexlib/Accessor.cxx \
    $$files(3rd-party/scintilla/lexers/*.cxx, false)

HEADERS  += \
    3rd-party/scintilla/qt/ScintillaEdit/ScintillaEdit.h \
    3rd-party/scintilla/qt/ScintillaEdit/ScintillaDocument.h \
    3rd-party/scintilla/qt/ScintillaEditBase/ScintillaEditBase.h \
    3rd-party/scintilla/qt/ScintillaEditBase/ScintillaQt.h

INCLUDEPATH +=  3rd-party/scintilla/qt/ScintillaEditBase \
                3rd-party/scintilla/include \
                3rd-party/scintilla/src \
                3rd-party/scintilla/lexlib

DEFINES *= SCINTILLA_QT=1 MAKING_LIBRARY=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1

CONFIG(release, debug|release) {
  DEFINES *= NDEBUG=1
}

# Add hoextdown.
SOURCES += $$files(3rd-party/hoedown/*.c, false)
HEADERS  += $$files(3rd-party/hoedown/*.h, false)

INCLUDEPATH +=  $$PWD/. \
                $$PWD/common/gui \
                $$PWD/saurus/gui \
                $$PWD/saurus/gui/dialogs \
                $$PWD/saurus/gui/sidebars \
                $$PWD/saurus/gui/settings \
                $$PWD/common/dynamic-shortcuts \
                $$PWD/saurus/external-tools \
                $$PWD/saurus/plugin-system

TRANSLATIONS += $$PWD/../../localization/textosaurus_en_GB.ts \
                $$PWD/../../localization/textosaurus_en.ts

# Create new "make lupdate" target.
lupdate.target = lupdate
lupdate.commands = lupdate $$shell_path($$PWD/libtextosaurus.pro)

QMAKE_EXTRA_TARGETS += lupdate

# Make sure QM translations are nerated.
qtPrepareTool(LRELEASE, lrelease) {
  message($$MSG_PREFIX: Running: $$LRELEASE_EXECUTABLE -compress $$shell_quote($$shell_path($$PWD/libtextosaurus.pro)))
  system($$LRELEASE_EXECUTABLE -compress libtextosaurus.pro)
}

static {
  message($$MSG_PREFIX: Building static version of library.)
}
else {
  message($$MSG_PREFIX: Building shared version of library.)
}

win32 {
  target.path = $$PREFIX
  INSTALLS += target
}

unix:!mac:!android {
  target.path = $$PREFIX/lib
  INSTALLS += target
}

mac {
  IDENTIFIER = $$APP_REVERSE_NAME
  CONFIG -= app_bundle
  QMAKE_MAC_SDK = macosx10.12
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
  LIBS += -framework AppKit
}
