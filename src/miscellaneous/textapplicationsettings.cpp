// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/textapplicationsettings.h"

#include "miscellaneous/application.h"
#include "miscellaneous/settings.h"
#include "miscellaneous/textapplication.h"

#include <QAction>
#include <QDir>
#include <QFileInfo>

TextApplicationSettings::TextApplicationSettings(TextApplication* parent) : QObject(parent), m_textApplication(parent) {}

QsciScintilla::EolMode TextApplicationSettings::eolMode() const {
  return static_cast<QsciScintilla::EolMode>(qApp->settings()->value(GROUP(Editor), SETTING(Editor::EolMode)).toInt());
}

bool TextApplicationSettings::wordWrapEnabled() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::WordWrap)).toBool();
}

QString TextApplicationSettings::loadSaveDefaultDirectory() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::DefaultLoadSaveDirectory)).toString();
}

void TextApplicationSettings::setLoadSaveDefaultDirectory(const QString& directory) {
  qApp->settings()->setValue(GROUP(Editor), Editor::DefaultLoadSaveDirectory,
                             QDir::toNativeSeparators(QFileInfo(directory).absolutePath()));
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

ExternalTools* TextApplicationSettings::externalTools() const
{
  return m_externalTools;
}
