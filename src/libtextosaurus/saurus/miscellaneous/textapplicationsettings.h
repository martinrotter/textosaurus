// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef TEXTAPPLICATIONSETTINGS_H
#define TEXTAPPLICATIONSETTINGS_H

#include <QObject>

class TextApplication;
class SyntaxHighlighting;
class ExternalTools;
class FormMain;
class QAction;
class PluginFactory;
class BaseSidebar;

class TEXTOSAURUS_DLLSPEC TextApplicationSettings : public QObject {
  Q_OBJECT

  public:
    explicit TextApplicationSettings(TextApplication* parent = nullptr);

    bool contextAwareHighlightingEnabled() const;
    bool codeFoldingEnabled() const;
    bool restorePreviousSession() const;
    int restoredSessionTabIndex() const;
    bool autoIndentEnabled() const;
    QStringList recentFiles() const;
    int eolMode() const;
    QString eolString() const;
    bool wordWrapEnabled() const;
    bool lineNumbersEnabled() const;
    QString loadSaveDefaultDirectory() const;
    bool viewWhitespaces() const;
    bool viewEols() const;
    int lineSpacing() const;
    QFont mainFont() const;
    bool reloadModifiedDocumentsAutomatically() const;
    QString logTimestampFormat() const;
    QString dateTimeTimestampFormat() const;

    int tabSize() const;
    int indentSize() const;
    bool indentWithTabs() const;

    bool edgeLineEnabled() const;
    int edgeLineColumn() const;

    ExternalTools* externalTools() const;
    SyntaxHighlighting* syntaxHighlighting() const;
    PluginFactory* pluginFactory() const;

  public slots:
    void loadInitialSidebarGuiSettings(FormMain* main_form, const QList<BaseSidebar*>& dock_widgets) const;

    void increaseLineSpacing();
    void decreaseLineSpacing();

    void increaseFontSize();
    void decreaseFontSize();

    void setContextAwareHighlightingEnabled(bool enabled);
    void setCodeFoldingEnabled(bool enabled);
    void setRestoredSessionTabIndex(int index);
    void setRestorePreviousSession(bool restore);
    void setAutoIndentEnabled(bool enabled);
    void setLogTimestampFormat(const QString& format);
    void setDateTimeTimestampFormat(const QString& format);
    void setReloadModifiedDocumentsAutomatically(bool reload_automatically);
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
    void setEdgeLineEnabled(bool enabled);
    void setEdgeLineColumn(int column);

  signals:

    // Emitted when there are settings changes which directly influence behavior of some editors.
    void settingsChanged(bool reload_visible_editor, bool reload_all_editors);

  private:
    TextApplication* m_textApplication;
    ExternalTools* m_externalTools;
    SyntaxHighlighting* m_syntaxHighlighting;
    PluginFactory* m_pluginFactory;
};

#endif // TEXTAPPLICATIONSETTINGS_H
