// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef EDITORTAB_H
#define EDITORTAB_H

#include "saurus/gui/tab.h"
#include "saurus/gui/texteditor.h"

#include <QIcon>

class TextApplication;

class EditorTab : public Tab {
  Q_OBJECT

  public:
    explicit EditorTab(TextApplication* text_app, TextEditor* editor);
    explicit EditorTab(TextApplication* text_app);

    virtual QMenu* contextMenu() const override;
    virtual TextEditor* primaryEditor() const override;
    virtual QList<TextEditor*> allEditors() const override;
    virtual int countOfEditors() const override;
    virtual TabType tabType() const override;
    virtual QIcon icon() const override;
    virtual QString title() const override;
    virtual QString toolTip() const override;

  protected:
    virtual void closeEvent(QCloseEvent* event) override;

  private slots:
    void updateIcon(bool read_only);
    void updateTitleFromEditor();

  private:
    TextEditor* m_editor;
    QIcon m_icon;
    QString m_title;
    QString m_toolTip;
};

#endif // EDITORTAB_H
