// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/textapplicationsettings.h"

#include "external-tools/externaltools.h"
#include "gui/dialogs/formmain.h"
#include "gui/docks/dockwidget.h"
#include "miscellaneous/application.h"
#include "miscellaneous/settings.h"
#include "miscellaneous/syntaxhighlighting.h"
#include "miscellaneous/textapplication.h"

#include <QAction>
#include <QDir>
#include <QFileInfo>
#include <QFontDatabase>
#include <QRegularExpression>

TextApplicationSettings::TextApplicationSettings(TextApplication* parent)
  : QObject(parent), m_textApplication(parent), m_externalTools(new ExternalTools(parent)),
  m_syntaxHighlighting(new SyntaxHighlighting(this)) {}

TextApplicationSettings::~TextApplicationSettings() {
  qDebug("Destroying TextApplicationSettings");
}

QStringList TextApplicationSettings::recentFiles() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::RecentFiles)).toStringList();
}

void TextApplicationSettings::setRecentFiles(const QStringList& recent_files) {
  qApp->settings()->setValue(GROUP(Editor), Editor::RecentFiles, recent_files);
}

int TextApplicationSettings::eolMode() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::EolMode)).toInt();
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

SyntaxHighlighting* TextApplicationSettings::syntaxHighlighting() const {
  return m_syntaxHighlighting;
}

void TextApplicationSettings::loadDocksStates(FormMain* main_form, const QList<DockWidget*>& dock_widgets) const {
  foreach (DockWidget* dock, dock_widgets) {
    int size = qApp->settings()->value(GROUP(General), dock->objectName() + QSL("_width"), dock->initialWidth()).toInt();
    bool visible = qApp->settings()->value(GROUP(General), dock->objectName() + QSL("_visible"), dock->initiallyVisible()).toBool();

    Qt::DockWidgetArea area = qApp->settings()->value(GROUP(General),
                                                      dock->objectName() + QSL("_area"),
                                                      dock->initialArea()).value<Qt::DockWidgetArea>();

    if (area == Qt::DockWidgetArea::NoDockWidgetArea) {
      area = Qt::DockWidgetArea::BottomDockWidgetArea;
    }

    main_form->removeDockWidget(dock);
    main_form->addDockWidget(area, dock);
    main_form->resizeDocks(QList<QDockWidget*>() << dock,
                           QList<int>() << size,
                           (area == Qt::DockWidgetArea::LeftDockWidgetArea || area == Qt::DockWidgetArea::RightDockWidgetArea) ?
                           Qt::Horizontal : Qt::Vertical);

    dock->setVisible(visible);
  }
}

void TextApplicationSettings::saveDocksStates(FormMain* main_form, const QList<DockWidget*>& dock_widgets) const {
  foreach (DockWidget* dock, dock_widgets) {
    Qt::DockWidgetArea area = main_form->dockWidgetArea(dock);

    int size = qApp->settings()->value(GROUP(General),
                                       dock->objectName() + QSL("_width"),
                                       dock->width()).toInt();
    bool visible = qApp->settings()->value(GROUP(General), dock->objectName() + QSL("_visible"), dock->isVisible()).toBool();

    if (area == Qt::DockWidgetArea::NoDockWidgetArea) {
      area = dock->initialArea();
    }

    main_form->resizeDocks(QList<QDockWidget*>() << dock,
                           QList<int>() << size,
                           (area == Qt::DockWidgetArea::LeftDockWidgetArea || area == Qt::DockWidgetArea::RightDockWidgetArea) ?
                           Qt::Horizontal : Qt::Vertical);

    dock->setVisible(visible);
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
