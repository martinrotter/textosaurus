// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef EXTERNALTOOL_H
#define EXTERNALTOOL_H

#include <QObject>

#include "gui/texteditor.h"

#include <functional>

enum class ToolInput {
  // Currently selected text (or whole document's text) is given as tool input.
  // WARNING: This is very bad idea if current document/selection is big.
  SelectionDocument,

  // Text from highlighted line is given as tool input.
  CurrentLine,

  // Saved file path is given as tool input.
  SavedFile
};

enum class ToolOutput {
  // Current selection (or document if there is no selection) is replaced.
  // WARNING: This is very bad idea if current document/selection is big.
  ReplaceSelectionDocument,

  // Tool output is inserted at cursor position.
  // WARNING: This is very bad idea if tool output is big.
  InsertAtCursorPosition,

  // Tool output is dumped into output toolbox window.
  DumpToOutputWindow,

  // Output is saved to some file stored in TEMP
  // and is opened when tool finishes its work.
  NewSavedFile
};

class ExternalTool : public QObject {
  Q_OBJECT

  public:
    explicit ExternalTool(QObject* parent = nullptr);

    virtual bool isPredefined() const;

    QString name() const;
    void setName(const QString& name);

    QString id() const;
    void setId(const QString& id);

    QString command() const;
    void setCommand(const QString& command);

    QString script() const;
    void setScript(const QString& script);

    ToolInput input() const;
    void setInput(const ToolInput& input);

    ToolOutput output() const;
    void setOutput(const ToolOutput& output);

  public slots:
    virtual void runTool(const QPointer<TextEditor>& editor);

  signals:
    void toolFinished(QPointer<TextEditor> editor, QString output_text);

  private:
    QString m_name;
    QString m_id;

    // If this tool runs simple command, for example
    // "base64" then it is here.
    QString m_command;

    // If this tool runs complex script, then its contents
    // are here, for example
    // #!/bin/bash
    //
    QString m_script;
    ToolInput m_input;
    ToolOutput m_output;
};

class PredefinedTool : public ExternalTool {
  Q_OBJECT

  public:
    explicit PredefinedTool(std::function<QString(const QString&)> functor, QObject* parent = nullptr);

  public slots:
    virtual void runTool(const QPointer<TextEditor>& editor);

    bool isPredefined() const;

  private:

    // This is reference to operation:
    // QString operation(const QString& input).
    std::function<QString(const QString&)> m_functor;
};

#endif // EXTERNALTOOL_H
