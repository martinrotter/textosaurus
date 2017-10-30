// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TEXTAPPLICATIONSETTINGS_H
#define TEXTAPPLICATIONSETTINGS_H

#include <QObject>

#include <Qsci/qsciscintilla.h>

class TextApplicationSettings : public QObject {
  Q_OBJECT

  public:
    explicit TextApplicationSettings(QObject* parent = nullptr);

    QsciScintilla::EolMode eolMode() const;
    bool wordWrapEnabled() const;

  public slots:
    void setWordWrapEnabled(bool enabled);
    void setEolModeFromAction(QAction* act);

  signals:
    void settingsChanged(bool reload_visible_editor, bool reload_all_editors);

  private:

    //QList<
};

#endif // TEXTAPPLICATIONSETTINGS_H
