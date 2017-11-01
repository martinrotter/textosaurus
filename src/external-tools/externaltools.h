// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef EXTERNALTOOLS_H
#define EXTERNALTOOLS_H

#include <QObject>

class TextApplicationSettings;
class TextEditor;
class ExternalTool;
class QAction;

// Top-level management class for external tools.
class ExternalTools : public QObject {
  Q_OBJECT

  public:
    explicit ExternalTools(QObject* parent = nullptr);
    virtual ~ExternalTools();

    QList<QAction*> generateActions() const;

    const QList<ExternalTool*> tools() const;

  signals:

    // Emitted when some of external tools is added/removed/edited.
    void externalToolsChanged();

  private:
    QList<ExternalTool*> m_tools;
};

#endif // EXTERNALTOOLS_H
