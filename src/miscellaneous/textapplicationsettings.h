// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TEXTAPPLICATIONSETTINGS_H
#define TEXTAPPLICATIONSETTINGS_H

#include <QObject>

class TextApplication;
class SyntaxHighlighting;
class ExternalTools;
class FormMain;
class QAction;
class DockWidget;

class TextApplicationSettings : public QObject {
  Q_OBJECT

  public:
    explicit TextApplicationSettings(TextApplication* parent = nullptr);
    virtual ~TextApplicationSettings();

    QStringList recentFiles() const;
    int eolMode() const;
    bool wordWrapEnabled() const;
    bool lineNumbersEnabled() const;
    QString loadSaveDefaultDirectory() const;
    bool viewWhitespaces() const;
    bool viewEols() const;
    int lineSpacing() const;
    QFont mainFont() const;

    int tabSize() const;
    int indentSize() const;
    bool indentWithTabs() const;

    ExternalTools* externalTools() const;
    SyntaxHighlighting* syntaxHighlighting() const;

  public slots:
    void loadDocksStates(FormMain* main_form, const QList<DockWidget*>& dock_widgets) const;
    void saveDocksStates(FormMain* main_form, const QList<DockWidget*>& dock_widgets) const;

    void increaseLineSpacing();
    void decreaseLineSpacing();

    void increaseFontSize();
    void decreaseFontSize();

    void setTabSize(int size);
    void setIndentSize(int size);
    void setIndentWithTabs(bool use_tabs);
    void setMainFont(const QFont& fon);
    void setLineSpacing(int spacing);
    void setRecentFiles(const QStringList& recent_files);
    void setViewWhitespaces(bool view);
    void setViewEols(bool view);
    void setLoadSaveDefaultDirectory(const QString& file_path);
    void setWordWrapEnabled(bool enabled);
    void setLineNumbersEnabled(bool enabled);
    void setEolMode(int mode);

  signals:

    // Emitted when there are settings changes which directly influence behavior of some editors.
    void settingsChanged(bool reload_visible_editor, bool reload_all_editors);

  private:
    TextApplication* m_textApplication;
    ExternalTools* m_externalTools;
    SyntaxHighlighting* m_syntaxHighlighting;
};

#endif // TEXTAPPLICATIONSETTINGS_H
