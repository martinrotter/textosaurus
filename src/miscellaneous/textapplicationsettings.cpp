// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/textapplicationsettings.h"

#include "miscellaneous/application.h"
#include "miscellaneous/settings.h"

#include <QAction>

TextApplicationSettings::TextApplicationSettings(QObject* parent) : QObject(parent) {}

QsciScintilla::EolMode TextApplicationSettings::eolMode() const {
  return static_cast<QsciScintilla::EolMode>(qApp->settings()->value(GROUP(Editor), SETTING(Editor::EolMode)).toInt());
}

bool TextApplicationSettings::wordWrapEnabled() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::WordWrap)).toBool();
}

void TextApplicationSettings::setWordWrapEnabled(bool enabled) {
  qApp->settings()->setValue(GROUP(Editor), Editor::WordWrap, enabled);
  emit settingsChanged(true, false);
}

void TextApplicationSettings::setEolModeFromAction(QAction* act) {
  QsciScintilla::EolMode new_mode = static_cast<QsciScintilla::EolMode>(act->data().toInt());

  qApp->settings()->setValue(GROUP(Editor), Editor::EolMode, int(new_mode));
  emit settingsChanged(false, false);
}
