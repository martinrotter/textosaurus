// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/miscellaneous/textapplicationsettings.h"

#include "common/miscellaneous/settings.h"
#include "saurus/external-tools/externaltools.h"
#include "saurus/gui/dialogs/formmain.h"
#include "saurus/gui/sidebars/outputsidebar.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/syntaxhighlighting.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/plugin-system/pluginfactory.h"

#include <QAction>
#include <QDir>
#include <QFileInfo>
#include <QFontDatabase>
#include <QRegularExpression>

TextApplicationSettings::TextApplicationSettings(TextApplication* parent)
  : QObject(parent), m_textApplication(parent), m_externalTools(new ExternalTools(parent)),
  m_syntaxHighlighting(new SyntaxHighlighting(this)), m_pluginFactory(new PluginFactory(this)) {}

bool TextApplicationSettings::contextAwareHighlightingEnabled() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::ContextAwareHighlights)).toBool();
}

bool TextApplicationSettings::codeFoldingEnabled() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::CodeFolding)).toBool();
}

bool TextApplicationSettings::restorePreviousSession() const {
  return qApp->settings()->value(GROUP(General), SETTING(General::RestoreSession)).toBool();
}

int TextApplicationSettings::restoredSessionTabIndex() const {
  return qApp->settings()->value(GROUP(General), SETTING(General::RestoreSessionIndex)).toInt();
}

bool TextApplicationSettings::autoIndentEnabled() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::AutoIndent)).toBool();
}

QStringList TextApplicationSettings::recentFiles() const {
  return qApp->settings()->value(GROUP(Editor), Editor::RecentFiles).toStringList();
}

void TextApplicationSettings::setRecentFiles(const QStringList& recent_files) {
  qApp->settings()->setValue(GROUP(Editor), Editor::RecentFiles, recent_files);
}

int TextApplicationSettings::eolMode() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::EolMode)).toInt();
}

QString TextApplicationSettings::eolString() const {
  switch (eolMode()) {
    case SC_EOL_CRLF:
      return QSL("\r\n");

    case SC_EOL_CR:
      return QSL("\r");

    case SC_EOL_LF:
    default:
      return QSL("\n");
  }
}

bool TextApplicationSettings::wordWrapEnabled() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::WordWrap)).toBool();
}

bool TextApplicationSettings::lineNumbersEnabled() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::LineNumbers)).toBool();
}

QString TextApplicationSettings::loadSaveDefaultDirectory() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::DefaultLoadSaveDirectory)).toString();
}

bool TextApplicationSettings::viewWhitespaces() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::ViewWhitespaces)).toBool();
}

bool TextApplicationSettings::viewEols() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::ViewEols)).toBool();
}

int TextApplicationSettings::lineSpacing() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::LineSpacing)).toInt();
}

QFont TextApplicationSettings::mainFont() const {
  QFont def_font = QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont);
  QFont fon;
  QString saved_font = qApp->settings()->value(GROUP(Editor),
                                               Editor::FontMain,
                                               def_font.toString()).toString();

  fon.fromString(saved_font);

  return fon;
}

bool TextApplicationSettings::reloadModifiedDocumentsAutomatically() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::ReloadModified)).toBool();
}

QString TextApplicationSettings::logTimestampFormat() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::LogTimestampFormat)).toString();
}

QString TextApplicationSettings::dateTimeTimestampFormat() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::DateTimeTimestampFormat)).toString();
}

int TextApplicationSettings::tabSize() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::TabSize)).toInt();
}

int TextApplicationSettings::indentSize() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::IndentSize)).toInt();
}

bool TextApplicationSettings::indentWithTabs() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::IndentWithTabs)).toBool();
}

bool TextApplicationSettings::edgeLineEnabled() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::EdgeLongLine)).toBool();
}

int TextApplicationSettings::edgeLineColumn() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::EdgeLongLineColumn)).toInt();
}

void TextApplicationSettings::increaseFontSize() {
  QFont font_old = mainFont();

  font_old.setPointSize(font_old.pointSize() + 1);
  setMainFont(font_old);
}

void TextApplicationSettings::decreaseFontSize() {
  QFont font_old = mainFont();

  font_old.setPointSize(font_old.pointSize() - 1);
  setMainFont(font_old);
}

void TextApplicationSettings::setContextAwareHighlightingEnabled(bool enabled) {
  qApp->settings()->setValue(GROUP(Editor), Editor::ContextAwareHighlights, enabled);
  emit settingsChanged(true, false);
}

void TextApplicationSettings::setCodeFoldingEnabled(bool enabled) {
  qApp->settings()->setValue(GROUP(Editor), Editor::CodeFolding, enabled);
  emit settingsChanged(true, false);
}

void TextApplicationSettings::setRestoredSessionTabIndex(int index) {
  qApp->settings()->setValue(GROUP(General), General::RestoreSessionIndex, index);
  emit settingsChanged(false, false);
}

void TextApplicationSettings::setRestorePreviousSession(bool restore) {
  qApp->settings()->setValue(GROUP(General), General::RestoreSession, restore);
  emit settingsChanged(false, false);
}

void TextApplicationSettings::setAutoIndentEnabled(bool enabled) {
  qApp->settings()->setValue(GROUP(Editor), Editor::AutoIndent, enabled);
  emit settingsChanged(false, false);
}

void TextApplicationSettings::setLogTimestampFormat(const QString& format) {
  qApp->settings()->setValue(GROUP(Editor), Editor::LogTimestampFormat, format);
  emit settingsChanged(false, false);
}

void TextApplicationSettings::setDateTimeTimestampFormat(const QString& format) {
  qApp->settings()->setValue(GROUP(Editor), Editor::DateTimeTimestampFormat, format);
  emit settingsChanged(false, false);
}

void TextApplicationSettings::setReloadModifiedDocumentsAutomatically(bool reload_automatically) {
  qApp->settings()->setValue(GROUP(Editor), Editor::ReloadModified, reload_automatically);
  emit settingsChanged(false, false);
}

void TextApplicationSettings::setTabSize(int size) {
  qApp->settings()->setValue(GROUP(Editor), Editor::TabSize, size);
  emit settingsChanged(true, false);
}

void TextApplicationSettings::setIndentSize(int size) {
  qApp->settings()->setValue(GROUP(Editor), Editor::IndentSize, size);
  emit settingsChanged(true, false);
}

void TextApplicationSettings::setIndentWithTabs(bool use_tabs) {
  qApp->settings()->setValue(GROUP(Editor), Editor::IndentWithTabs, use_tabs);
  emit settingsChanged(true, false);
}

void TextApplicationSettings::setLineSpacing(int spacing) {
  if (LINE_SPACING_MIN <= spacing && spacing <= LINE_SPACING_MAX) {
    qApp->settings()->setValue(GROUP(Editor), Editor::LineSpacing, spacing);
    emit settingsChanged(true, false);
  }
}

void TextApplicationSettings::setMainFont(const QFont& fon) {
  qApp->settings()->setValue(GROUP(Editor), Editor::FontMain, fon.toString());
  emit settingsChanged(true, false);
}

void TextApplicationSettings::setLoadSaveDefaultDirectory(const QString& file_path) {
  const QString normalized_file_path = QDir::toNativeSeparators(file_path);
  const QString normalized_folder = QDir::toNativeSeparators(QFileInfo(file_path).absolutePath());

  qApp->settings()->setValue(GROUP(Editor), Editor::DefaultLoadSaveDirectory, normalized_folder);

  // Also, we remember this for "Recently opened files" feature.
  QStringList recent_files = recentFiles();

  if (recent_files.contains(normalized_file_path)) {
    recent_files.removeOne(normalized_file_path);
  }

  // We move it to last position.
  recent_files.prepend(normalized_file_path);

  if (recent_files.size() > MAX_RECENT_FILES) {
    setRecentFiles(recent_files.mid(0, MAX_RECENT_FILES));
  }
  else {
    setRecentFiles(recent_files);
  }
}

void TextApplicationSettings::setWordWrapEnabled(bool enabled) {
  qApp->settings()->setValue(GROUP(Editor), Editor::WordWrap, enabled);
  emit settingsChanged(true, false);
}

void TextApplicationSettings::setLineNumbersEnabled(bool enabled) {
  qApp->settings()->setValue(GROUP(Editor), Editor::LineNumbers, enabled);
  emit settingsChanged(true, false);
}

void TextApplicationSettings::setEolMode(int mode) {
  qApp->settings()->setValue(GROUP(Editor), Editor::EolMode, mode);
  emit settingsChanged(false, false);
}

void TextApplicationSettings::setEdgeLineEnabled(bool enabled) {
  qApp->settings()->setValue(GROUP(Editor), Editor::EdgeLongLine, enabled);
  emit settingsChanged(true, false);
}

void TextApplicationSettings::setEdgeLineColumn(int column) {
  qApp->settings()->setValue(GROUP(Editor), Editor::EdgeLongLineColumn, column);
  emit settingsChanged(true, false);
}

SyntaxHighlighting* TextApplicationSettings::syntaxHighlighting() const {
  return m_syntaxHighlighting;
}

PluginFactory* TextApplicationSettings::pluginFactory() const {
  return m_pluginFactory;
}

void TextApplicationSettings::loadInitialSidebarGuiSettings(FormMain* main_form, const QList<BaseSidebar*>& dock_widgets) const {
  foreach (BaseSidebar* dock, dock_widgets) {
    dock->setParent(main_form);

    int size = dock->initialWidth();

    Qt::DockWidgetArea area = dock->initialArea();

    if (area == Qt::DockWidgetArea::NoDockWidgetArea) {
      area = Qt::DockWidgetArea::BottomDockWidgetArea;
    }

    main_form->addDockWidget(area, dock);
    main_form->resizeDocks(QList<QDockWidget*>() << dock,
                           QList<int>() << size,
                           (area == Qt::DockWidgetArea::LeftDockWidgetArea || area == Qt::DockWidgetArea::RightDockWidgetArea) ?
                           Qt::Horizontal : Qt::Vertical);

    dock->setVisible(dock->initiallyVisible());
  }
}

void TextApplicationSettings::increaseLineSpacing() {
  setLineSpacing(lineSpacing() + 1);
}

void TextApplicationSettings::decreaseLineSpacing() {
  setLineSpacing(lineSpacing() - 1);
}

ExternalTools* TextApplicationSettings::externalTools() const {
  return m_externalTools;
}

void TextApplicationSettings::setViewWhitespaces(bool view) {
  qApp->settings()->setValue(GROUP(Editor), Editor::ViewWhitespaces, view);
  emit settingsChanged(true, false);
}

void TextApplicationSettings::setViewEols(bool view) {
  qApp->settings()->setValue(GROUP(Editor), Editor::ViewEols, view);
  emit settingsChanged(true, false);
}
