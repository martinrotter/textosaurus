// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef EXTERNALTOOL_H
#define EXTERNALTOOL_H

#include <QObject>

#include "saurus/gui/texteditor.h"

#include <QProcess>

#include <functional>

class ExternalTool : public QObject {
  Q_OBJECT

  public:
    enum class ToolInput {
      // Currently selected text (or whole document's text) is given as tool input.
      // WARNING: This is very bad idea if current document/selection is big.
      SelectionDocument,

      // Text from highlighted line is given as tool input.
      CurrentLine,

      // Saved file path is given as tool input.
      SavedFile,

      // User is asked to enter textual input.
      AskForInput,

      // This tool does not take any sort of input.
      NoInput
    };

    enum class ToolOutput {
      // Current selection (or document if there is no selection) is replaced.
      // WARNING: This is very bad idea if current document/selection is big.
      ReplaceSelectionDocument,

      // Line containing care is replaced.
      ReplaceCurrentLine,

      // Tool output is inserted at cursor position.
      // WARNING: This is very bad idea if tool output is big.
      InsertAtCursorPosition,

      // Tool output is dumped into output toolbox window.
      // WARNING: This is very bad idea if tool output is big.
      DumpToOutputWindow,

      // Copies tool output to clipboard and displays in output window info.
      // WARNING: This is very bad idea if tool output is big.
      CopyToClipboard,

      // Output is saved to some file stored in TEMP
      // and is opened when tool finishes its work.
      NewSavedFile,

      // File opened in editor is reloaded.
      ReloadFile,

      // Tool has no output, therefore we do not have to wait for it to exit or handle it
      // in any way.
      NoOutput,

      // Output is inserted at current cursor position
      // and all selected text is removed before that.
      InsertAtCursorPositionAndReplaceSelection,
    };

    explicit ExternalTool(QObject* parent = nullptr);
    explicit ExternalTool(const ExternalTool& other, QObject* parent = nullptr);
    virtual ~ExternalTool() = default;

    virtual bool isRunning() const;
    virtual bool isPredefined() const;

    QString name() const;
    void setName(const QString& name);

    QString script() const;
    void setScript(const QString& script);

    ToolInput input() const;
    void setInput(const ToolInput& input);

    ToolOutput output() const;
    void setOutput(const ToolOutput& output);

    QString category() const;
    void setCategory(const QString& category);

    QString shortcut() const;
    void setShortcut(const QString& shortcut);

    QString interpreter() const;
    void setInterpreter(const QString& interpreter);

    QString prompt() const;
    void setPrompt(const QString& prompt);

    QAction* action() const;

    void setAction(QAction* action);

    void setActionObjectName(const QString& action_obj_name);
    QString actionObjectName() const;

    bool addToEditMenu() const;
    void setAddToEditMenu(bool add_to_edit_menu);

    QString promptValue() const;
    void setPromptValue(const QString& prompt_value);

  public slots:

    // Runs tool with given string data (which is depending on tool input mode
    // current line, current selection, current whole document text or saved
    // document filename.
    virtual void runTool(QPointer<TextEditor> editor, const QString& data);

  private slots:
    void onProcessError(QPointer<TextEditor> editor);
    void onProcessFinished(QPointer<TextEditor> editor, int exit_code, QProcess::ExitStatus exit_status);

  signals:
    void partialOutputObtained(QString output);
    void toolFinished(QPointer<TextEditor> editor, QString standard_output,
                      QString error_output, bool success);

  private:
    bool m_isRunning;
    bool m_addToEditMenu;
    QString m_actionObjectName;
    QAction* m_action;
    ToolInput m_input;
    ToolOutput m_output;
    QString m_prompt;
    QString m_promptValue;
    QString m_shortcut;
    QString m_category;
    QString m_name;
    QString m_interpreter;

    // If this tool runs complex script, then its contents
    // are here, for example
    // #!/bin/bash
    // base64
    QString m_script;
};

class PredefinedTool : public ExternalTool {
  Q_OBJECT

  public:
    explicit PredefinedTool(std::function<QString(const QString&, bool&)> functor, QObject* parent = nullptr);
    virtual ~PredefinedTool() = default;

    virtual bool isPredefined() const override;

  public slots:
    virtual void runTool(QPointer<TextEditor> editor, const QString& data) override;

  private:
    std::function<QString(const QString&, bool&)> m_functor;
};

#endif // EXTERNALTOOL_H
