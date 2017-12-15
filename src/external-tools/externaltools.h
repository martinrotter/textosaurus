// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef EXTERNALTOOLS_H
#define EXTERNALTOOLS_H

#include <QObject>

class TextApplicationSettings;
class TextEditor;
class TextApplication;
class ExternalTool;
class PredefinedTool;
class QAction;

// Top-level management class for external tools.
class ExternalTools : public QObject {
  Q_OBJECT

  public:
    explicit ExternalTools(TextApplication* parent = nullptr);
    virtual ~ExternalTools();

    QList<QAction*> generateActions(QWidget* parent) const;

    QList<ExternalTool*> tools() const;
    QList<QAction*> predefinedToolsActions() const;

    void saveExternalTools(const QList<ExternalTool*>& ext_tools);

  public slots:
    void reloadTools();

  private slots:
    void runSelectedExternalTool();
    void runTool(ExternalTool* tool_to_run, TextEditor* editor);
    void onToolPartialOutputObtained(const QString& output);
    void onToolFinished(const QPointer<TextEditor>& editor, const QString& output_text, const QString& error_text, bool success);

  signals:

    // Emitted when some of external tools is added/removed/edited.
    void externalToolsChanged();

  private:
    void loadPredefinedTools();
    void loadCustomTools();

  private:
    TextApplication* m_application;

    QList<PredefinedTool*> m_predefinedTools;
    QList<ExternalTool*> m_tools;
};

#endif // EXTERNALTOOLS_H
