#################################################################
#
# For license of this file, see <project-root-folder>/LICENSE.md.
#
#
# This is Textilosaurus compilation script for qmake.
#
# Usage:
#   a) DEBUG build for testing. (out of source build type)
#     cd ../build-dir
#     qmake ../textilosaurus-dir/textilosaurus.pro -r CONFIG+=debug PREFIX=./usr
#     make
#     make install
#
#   b) RELEASE build for production use. (out of source build type)
#     cd ../build-dir
#     qmake ../textilosaurus-dir/textilosaurus.pro -r CONFIG+=release PREFIX=./usr
#     make
#     make install
#
# Variables:
#   PREFIX - specifies base folder to which files are copied during "make install"
#            step, defaults to "$$OUT_PWD/usr" on Linux and to "$$OUT_PWD/app" on Windows.
#   LRELEASE_EXECUTABLE - specifies the name/path of "lrelease" executable, defaults to "lrelease".
#
# Other information:
#   - supports Windows, Linux,
#   - Qt 5.6.0 or higher is required,
#   - C++ 14 is required.
#
# Authors and contributors:
#   - Martin Rotter (project leader).
#
#################################################################

TEMPLATE    = app
TARGET      = textilosaurus

message(textilosaurus: Welcome Textilosaurus qmake script.)

lessThan(QT_MAJOR_VERSION, 5)|lessThan(QT_MINOR_VERSION, 6) {
  error(textilosaurus: At least Qt \"5.6.0\" is required!!!)
}

APP_NAME                      = "Textilosaurus"
APP_LOW_NAME                  = "textilosaurus"
APP_REVERSE_NAME              = "com.github.textilosaurus"
APP_AUTHOR                    = "Martin Rotter"
APP_COPYRIGHT                 = "(C) 2017 $$APP_AUTHOR"
APP_VERSION                   = "2017.12.20"
APP_LONG_NAME                 = "$$APP_NAME $$APP_VERSION"
APP_EMAIL                     = "rotter.martinos@gmail.com"
APP_URL                       = "https://github.com/martinrotter/textilosaurus"
APP_URL_ISSUES                = "https://github.com/martinrotter/textilosaurus/issues"
APP_URL_ISSUES_NEW            = "https://github.com/martinrotter/textilosaurus/issues/new"
APP_URL_WIKI                  = "https://github.com/martinrotter/textilosaurus/wiki"
APP_USERAGENT                 = "Textilosaurus/$$APP_VERSION (github.com/martinrotter/textilosaurus)"
APP_DONATE_URL                = "https://liberapay.com/martinrotter"
APP_WIN_ARCH                  = "win64"

isEmpty(PREFIX) {
  message(textilosaurus: PREFIX variable is not set. This might indicate error.)

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

message(textilosaurus: Shadow copy build directory \"$$OUT_PWD\".)

isEmpty(LRELEASE_EXECUTABLE) {
  LRELEASE_EXECUTABLE = lrelease
  message(textilosaurus: LRELEASE_EXECUTABLE variable is not set.)
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

message(textilosaurus: Textilosaurus version is: \"$$APP_VERSION\".)
message(textilosaurus: Detected Qt version: \"$$QT_VERSION\".)
message(textilosaurus: Build destination directory: \"$$DESTDIR\".)
message(textilosaurus: Prefix directory: \"$$PREFIX\".)
message(textilosaurus: Build revision: \"$$APP_REVISION\".)
message(textilosaurus: lrelease executable name: \"$$LRELEASE_EXECUTABLE\".)

QT *= core gui widgets network printsupport svg

CONFIG *= c++14 warn_on
DEFINES *= QT_USE_QSTRINGBUILDER QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS UNICODE _UNICODE
VERSION = $$APP_VERSION

win32 {
  # Makes sure we use correct subsystem on Windows.
  !contains(QMAKE_TARGET.arch, x86_64) {
    message(textilosaurus: Compilling x86 variant.)
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
  } else {
    message(textilosaurus: Compilling x86_64 variant.)
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.02
  }
}

# Setup specific compiler options.
CONFIG(release, debug|release) {
  gcc:QMAKE_CXXFLAGS_RELEASE += -O3
  clang:QMAKE_CXXFLAGS_RELEASE += -O3
}
else {
  gcc:QMAKE_CXXFLAGS_DEBUG += -Wall
  clang:QMAKE_CXXFLAGS_DEBUG += -Wall
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
  RC_ICONS = resources/graphics/textilosaurus.ico
  QMAKE_TARGET_COMPANY = $$APP_AUTHOR
  QMAKE_TARGET_DESCRIPTION = $$APP_NAME
  QMAKE_TARGET_COPYRIGHT = $$APP_COPYRIGHT
  QMAKE_TARGET_PRODUCT = $$APP_NAME
}

CONFIG *= resources_big
RESOURCES += resources/textilosaurus.qrc

mac|win32|android {
  RESOURCES += resources/icons.qrc

  message(textilosaurus: Adding resources for default icon theme.)
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
            src/gui/docks/outputwindow.h \
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
            src/qtsingleapplication/qtlocalpeer.h \
            src/qtsingleapplication/qtlockedfile.h \
            src/qtsingleapplication/qtsingleapplication.h \
            src/qtsingleapplication/qtsinglecoreapplication.h \
            src/uchardet/CharDistribution.h \
            src/uchardet/JpCntx.h \
            src/uchardet/nsBig5Prober.h \
            src/uchardet/nsCodingStateMachine.h \
            src/uchardet/nscore.h \
            src/uchardet/nsEscCharsetProber.h \
            src/uchardet/nsEUCJPProber.h \
            src/uchardet/nsEUCKRProber.h \
            src/uchardet/nsEUCTWProber.h \
            src/uchardet/nsGB2312Prober.h \
            src/uchardet/nsHebrewProber.h \
            src/uchardet/nsCharSetProber.h \
            src/uchardet/nsLatin1Prober.h \
            src/uchardet/nsMBCSGroupProber.h \
            src/uchardet/nsPkgInt.h \
            src/uchardet/nsSBCSGroupProber.h \
            src/uchardet/nsSBCharSetProber.h \
            src/uchardet/nsSJISProber.h \
            src/uchardet/nsUniversalDetector.h \
            src/uchardet/nsUTF8Prober.h \
            src/uchardet/prmem.h \
            src/uchardet/uchardet.h \
    src/gui/docks/dockwidget.h \
    src/gui/docks/filesystemsidebar.h

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
            src/gui/docks/outputwindow.cpp \
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
            src/qtsingleapplication/qtlocalpeer.cpp \
            src/qtsingleapplication/qtlockedfile.cpp \
            src/qtsingleapplication/qtsingleapplication.cpp \
            src/qtsingleapplication/qtsinglecoreapplication.cpp \
            src/uchardet/CharDistribution.cpp \
            src/uchardet/JpCntx.cpp \
            src/uchardet/LangArabicModel.cpp \
            src/uchardet/LangBulgarianModel.cpp \
            src/uchardet/LangCroatianModel.cpp \
            src/uchardet/LangCzechModel.cpp \
            src/uchardet/LangDanishModel.cpp \
            src/uchardet/LangEsperantoModel.cpp \
            src/uchardet/LangEstonianModel.cpp \
            src/uchardet/LangFinnishModel.cpp \
            src/uchardet/LangFrenchModel.cpp \
            src/uchardet/LangGermanModel.cpp \
            src/uchardet/LangGreekModel.cpp \
            src/uchardet/LangHebrewModel.cpp \
            src/uchardet/LangHungarianModel.cpp \
            src/uchardet/LangIrishModel.cpp \
            src/uchardet/LangItalianModel.cpp \
            src/uchardet/LangLatvianModel.cpp \
            src/uchardet/LangLithuanianModel.cpp \
            src/uchardet/LangMalteseModel.cpp \
            src/uchardet/LangPolishModel.cpp \
            src/uchardet/LangPortugueseModel.cpp \
            src/uchardet/LangRomanianModel.cpp \
            src/uchardet/LangRussianModel.cpp \
            src/uchardet/LangSlovakModel.cpp \
            src/uchardet/LangSloveneModel.cpp \
            src/uchardet/LangSpanishModel.cpp \
            src/uchardet/LangSwedishModel.cpp \
            src/uchardet/LangThaiModel.cpp \
            src/uchardet/LangTurkishModel.cpp \
            src/uchardet/LangVietnameseModel.cpp \
            src/uchardet/nsBig5Prober.cpp \
            src/uchardet/nsEscCharsetProber.cpp \
            src/uchardet/nsEscSM.cpp \
            src/uchardet/nsEUCJPProber.cpp \
            src/uchardet/nsEUCKRProber.cpp \
            src/uchardet/nsEUCTWProber.cpp \
            src/uchardet/nsGB2312Prober.cpp \
            src/uchardet/nsHebrewProber.cpp \
            src/uchardet/nsCharSetProber.cpp \
            src/uchardet/nsLatin1Prober.cpp \
            src/uchardet/nsMBCSGroupProber.cpp \
            src/uchardet/nsMBCSSM.cpp \
            src/uchardet/nsSBCSGroupProber.cpp \
            src/uchardet/nsSBCharSetProber.cpp \
            src/uchardet/nsSJISProber.cpp \
            src/uchardet/nsUniversalDetector.cpp \
            src/uchardet/nsUTF8Prober.cpp \
            src/uchardet/uchardet.cpp \
    src/gui/docks/dockwidget.cpp \
    src/gui/docks/filesystemsidebar.cpp

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
            src/gui/settings/settingseditor.ui

# Add Scintilla.
win32 {
  QMAKE_CXXFLAGS += -std:c++latest
}

SOURCES += \
    src/scintilla/qt/ScintillaEdit/ScintillaEdit.cpp \
    src/scintilla/qt/ScintillaEdit/ScintillaDocument.cpp \
    src/scintilla/qt/ScintillaEditBase/PlatQt.cpp \
    src/scintilla/qt/ScintillaEditBase/ScintillaQt.cpp \
    src/scintilla/qt/ScintillaEditBase/ScintillaEditBase.cpp \
    src/scintilla/src/XPM.cxx \
    src/scintilla/src/ViewStyle.cxx \
    src/scintilla/src/UniConversion.cxx \
    src/scintilla/src/Style.cxx \
    src/scintilla/src/Selection.cxx \
    src/scintilla/src/ScintillaBase.cxx \
    src/scintilla/src/RunStyles.cxx \
    src/scintilla/src/RESearch.cxx \
    src/scintilla/src/PositionCache.cxx \
    src/scintilla/src/PerLine.cxx \
    src/scintilla/src/MarginView.cxx \
    src/scintilla/src/LineMarker.cxx \
    src/scintilla/src/KeyMap.cxx \
    src/scintilla/src/Indicator.cxx \
    src/scintilla/src/ExternalLexer.cxx \
    src/scintilla/src/EditView.cxx \
    src/scintilla/src/Editor.cxx \
    src/scintilla/src/EditModel.cxx \
    src/scintilla/src/Document.cxx \
    src/scintilla/src/Decoration.cxx \
    src/scintilla/src/DBCS.cxx \
    src/scintilla/src/ContractionState.cxx \
    src/scintilla/src/CharClassify.cxx \
    src/scintilla/src/CellBuffer.cxx \
    src/scintilla/src/Catalogue.cxx \
    src/scintilla/src/CaseFolder.cxx \
    src/scintilla/src/CaseConvert.cxx \
    src/scintilla/src/CallTip.cxx \
    src/scintilla/src/AutoComplete.cxx \
    src/scintilla/lexlib/WordList.cxx \
    src/scintilla/lexlib/StyleContext.cxx \
    src/scintilla/lexlib/PropSetSimple.cxx \
    src/scintilla/lexlib/LexerSimple.cxx \
    src/scintilla/lexlib/LexerNoExceptions.cxx \
    src/scintilla/lexlib/LexerModule.cxx \
    src/scintilla/lexlib/LexerBase.cxx \
    src/scintilla/lexlib/DefaultLexer.cxx \
    src/scintilla/lexlib/CharacterSet.cxx \
    src/scintilla/lexlib/CharacterCategory.cxx \
    src/scintilla/lexlib/Accessor.cxx \
    $$files(src/scintilla/lexers/*.cxx, false)

HEADERS  += \
    src/scintilla/qt/ScintillaEdit/ScintillaEdit.h \
    src/scintilla/qt/ScintillaEdit/ScintillaDocument.h \
    src/scintilla/qt/ScintillaEditBase/ScintillaEditBase.h \
    src/scintilla/qt/ScintillaEditBase/ScintillaQt.h

INCLUDEPATH += src/scintilla/qt/ScintillaEditBase src/scintilla/include src/scintilla/src src/scintilla/lexlib

DEFINES *= SCINTILLA_QT=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1 EXPORT_IMPORT_API=

CONFIG(release, debug|release) {
  DEFINES *= NDEBUG=1
}

INCLUDEPATH +=  $$PWD/. \
                $$PWD/src \
                $$PWD/src/gui \
                $$PWD/src/gui/dialogs \
                $$PWD/src/gui/docks \
                $$PWD/src/dynamic-shortcuts \
                $$PWD/src/external-tools

TRANSLATIONS += $$PWD/localization/textilosaurus_en_GB.ts \
                $$PWD/localization/textilosaurus_en.ts

# Create new "make lupdate" target.
lupdate.target = lupdate
lupdate.commands = lupdate $$shell_path($$PWD/textilosaurus.pro)

QMAKE_EXTRA_TARGETS += lupdate

# Make sure QM translations are nerated.
qtPrepareTool(LRELEASE, lrelease) {
  system($$LRELEASE_EXECUTABLE -compress $$shell_quote($$shell_path($$PWD/textilosaurus.pro)))
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

  qt_dlls_root.files =  resources/binaries/qt/windows/qt5-msvc2015/libeay32.dll \
                        resources/binaries/qt/windows/qt5-msvc2015/msvcp140.dll \
                        resources/binaries/qt/windows/qt5-msvc2015/qt.conf \
                        resources/binaries/qt/windows/qt5-msvc2015/Qt5Core.dll \
                        resources/binaries/qt/windows/qt5-msvc2015/Qt5Gui.dll \
                        resources/binaries/qt/windows/qt5-msvc2015/Qt5Widgets.dll \
                        resources/binaries/qt/windows/qt5-msvc2015/Qt5Network.dll \
                        resources/binaries/qt/windows/qt5-msvc2015/Qt5Svg.dll \
                        resources/binaries/qt/windows/qt5-msvc2015/ssleay32.dll \
                        resources/binaries/qt/windows/qt5-msvc2015/vcruntime140.dll \
                        resources/binaries/qt/windows/qt5-msvc2015-webengine/Qt5PrintSupport.dll \
                        resources/binaries/qt/windows/qt5-msvc2015-webengine/printsupport
  qt_dlls_root.path = $$quote($$PREFIX/)

  qt_dlls_plugins.files =   resources/binaries/qt/windows/qt5-msvc2015/bearer \
                            resources/binaries/qt/windows/qt5-msvc2015/iconengines \
                            resources/binaries/qt/windows/qt5-msvc2015/imageformats \
                            resources/binaries/qt/windows/qt5-msvc2015/platforms
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
  IDENTIFIER = org.$${TARGET}.textilosaurus
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
