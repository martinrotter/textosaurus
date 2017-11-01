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
    explicit ExternalTools(TextApplication* parent = nullptr);
    virtual ~ExternalTools();

    QList<QAction*> generateActions() const;

    const QList<ExternalTool*> tools() const;

  public slots:
    void reloadTools();

  private slots:
    void runTool();
    void onToolFinished(QPointer<TextEditor> editor, const QString& output_text);

  signals:

    // Emitted when some of external tools is added/removed/edited.
    void externalToolsChanged(QList<QAction*> actions);

  private:
    TextApplication* m_textApplication;

    QList<ExternalTool*> m_tools;
};

#endif // EXTERNALTOOLS_H
