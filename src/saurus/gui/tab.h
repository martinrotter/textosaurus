// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TAB_H
#define TAB_H

#include <QWidget>

#include <QList>

class TextEditor;
class TabWidget;

enum class TabTyp {
  TextEditor = 1
};

class Tab : public QWidget {
  Q_OBJECT

  public:
    explicit Tab(TabWidget* tab_widget, QWidget* parent = nullptr);

    // Returns primary editor.
    virtual TextEditor* primaryEditor() const = 0;

    // Returns list of all editors, sorted from
    // most significant to less significant.
    virtual QList<TextEditor*> allEditors() const = 0;
    virtual TabTyp tabType() const = 0;

    // Return count of editors.
    virtual int countOfEditors() const = 0;

    // Returns index of this tab in tab widget.
    // NOTE: Returns value < 0 if this tab is not in tab widget.
    int index() const;

  private:
    TabWidget* m_tabWidget;
};

#endif // TAB_H
