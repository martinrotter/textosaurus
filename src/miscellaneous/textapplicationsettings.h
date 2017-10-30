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
    QString loadSaveDefaultDirectory() const;

  public slots:
    void setLoadSaveDefaultDirectory(const QString& directory);
    void setWordWrapEnabled(bool enabled);
    void setEolModeFromAction(QAction* act);

  signals:

    // Emitted when there are settings changes which directly influence behavior of some editors.
    void settingsChanged(bool reload_visible_editor, bool reload_all_editors);

  private:
};

#endif // TEXTAPPLICATIONSETTINGS_H
