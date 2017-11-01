// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef EXTERNALTOOL_H
#define EXTERNALTOOL_H

#include <QObject>

#include <functional>

enum class ToolInput {
  SelectionDocument,
  SavedFile
};

enum class ToolOutput {
  ReplaceSelectionDocument,
  DumpToOutputWindow
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

  public slots:
    virtual void runTool(const QString& application_path, const QString& file_path, const QString& selected_text);

  signals:
    void toolFinished(QString output_file, QString output_text, bool reload_current_file);

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
};

class PredefinedTool : public QObject {
  Q_OBJECT

  public:
    explicit PredefinedTool(std::function<QString(const QString&, const QString&, const QString&)> functor,
                            QObject* parent = nullptr);

  public slots:
    virtual void runTool(const QString& application_path, const QString& file_path, const QString& selected_text);

    bool isPredefined() const;

  private:
    std::function<QString(const QString&, const QString&, const QString&)> m_functor;
};

#endif // EXTERNALTOOL_H
