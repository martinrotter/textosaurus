// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TEXTAPPLICATIONSETTINGS_H
#define TEXTAPPLICATIONSETTINGS_H

#include <QObject>

#include <Qsci/qsciscintilla.h>

#include "external-tools/externaltools.h"

class TextApplication;

class TextApplicationSettings : public QObject {
  Q_OBJECT

  public:
    explicit TextApplicationSettings(TextApplication* parent = nullptr);
    virtual ~TextApplicationSettings();

    QStringList recentFiles() const;
    QsciScintilla::EolMode eolMode() const;
    bool wordWrapEnabled() const;
    QString loadSaveDefaultDirectory() const;

    bool viewWhitespaces() const;
    bool viewEols() const;

    ExternalTools* externalTools() const;

  public slots:
    void setRecentFiles(const QStringList& recent_files);
    void setViewWhitespaces(bool view);
    void setViewEols(bool view);
    void setLoadSaveDefaultDirectory(const QString& file_path);
    void setWordWrapEnabled(bool enabled);
    void setEolModeFromAction(QAction* act);

  signals:

    // Emitted when there are settings changes which directly influence behavior of some editors.
    void settingsChanged(bool reload_visible_editor, bool reload_all_editors);

  private:
    TextApplication* m_textApplication;
    ExternalTools* m_externalTools;
};

#endif // TEXTAPPLICATIONSETTINGS_H
