// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef EXTERNALTOOLS_H
#define EXTERNALTOOLS_H

#include <QObject>

class TextApplicationSettings;
class TextEditor;
class QMenu;

// Top-level management class for external tools.
class ExternalTools : public QObject {
  Q_OBJECT

  public:
    explicit ExternalTools(QObject* parent = nullptr);

  public slots:

    // Gets empty menu, then adds submenus/actions for various tools.
    void prepareMenu(QMenu* menu);

  private:

};

#endif // EXTERNALTOOLS_H
