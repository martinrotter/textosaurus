// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef EDITORTAB_H
#define EDITORTAB_H

#include "saurus/gui/tab.h"
#include "saurus/gui/texteditor.h"

class TextApplication;

class EditorTab : public Tab {
  public:
    explicit EditorTab(TextApplication* text_app, QWidget* parent = nullptr);

    virtual TextEditor* primaryEditor() const override;
    virtual QList<TextEditor*> allEditors() const override;
    virtual int countOfEditors() const override;
    virtual TabTyp tabType() const override;

  private:
    TextEditor* m_editor;
};

#endif // EDITORTAB_H
