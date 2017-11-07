// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/textapplicationsettings.h"

#include "miscellaneous/application.h"
#include "miscellaneous/settings.h"
#include "miscellaneous/textapplication.h"

#include <QAction>
#include <QDir>
#include <QFileInfo>

TextApplicationSettings::TextApplicationSettings(TextApplication* parent)
  : QObject(parent), m_textApplication(parent), m_externalTools(new ExternalTools(parent)) {}

TextApplicationSettings::~TextApplicationSettings() {
  qDebug("Destroying TextApplicationSettings");
}

QStringList TextApplicationSettings::recentFiles() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::RecentFiles)).toStringList();
}

void TextApplicationSettings::setRecentFiles(const QStringList& recent_files) {
  qApp->settings()->setValue(GROUP(Editor), Editor::RecentFiles, recent_files);
}

QsciScintilla::EolMode TextApplicationSettings::eolMode() const {
  return static_cast<QsciScintilla::EolMode>(qApp->settings()->value(GROUP(Editor), SETTING(Editor::EolMode)).toInt());
}

bool TextApplicationSettings::wordWrapEnabled() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::WordWrap)).toBool();
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
  recent_files.append(normalized_file_path);
  setRecentFiles(recent_files);
}

void TextApplicationSettings::setWordWrapEnabled(bool enabled) {
  qApp->settings()->setValue(GROUP(Editor), Editor::WordWrap, enabled);
  emit settingsChanged(true, false);
}

void TextApplicationSettings::setEolModeFromAction(QAction* act) {
  QsciScintilla::EolMode new_mode = static_cast<QsciScintilla::EolMode>(act->data().toInt());

  qApp->settings()->setValue(GROUP(Editor), Editor::EolMode, int(new_mode));
  emit settingsChanged(true, false);
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
