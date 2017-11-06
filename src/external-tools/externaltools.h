// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef EXTERNALTOOLS_H
#define EXTERNALTOOLS_H

#include <QObject>

class TextApplicationSettings;
class TextEditor;
class TextApplication;
class ExternalTool;
class QAction;

// Top-level management class for external tools.
class ExternalTools : public QObject {
  Q_OBJECT

  public:
    explicit ExternalTools(QObject* parent = nullptr);
    virtual ~ExternalTools();

    QList<QAction*> generateActions(QWidget* parent, TextApplication* app) const;

    const QList<ExternalTool*> tools() const;

  public slots:
    void runTool(ExternalTool* tool_to_run, TextEditor* editor);
    void reloadTools();

  private slots:
    void onToolFinished(const QPointer<TextEditor>& editor, const QString& output_text, bool success);

  signals:
    void toolFinished(ExternalTool* tool, QPointer<TextEditor> editor, QString output_text, bool success);

    // Emitted when some of external tools is added/removed/edited.
    void externalToolsChanged();

  private:
    void loadPredefinedTools();
    void loadCustomTools();

  private:
    QList<ExternalTool*> m_tools;
};

#endif // EXTERNALTOOLS_H
