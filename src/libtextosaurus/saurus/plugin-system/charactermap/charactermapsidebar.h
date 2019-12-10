// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef CHARACTERMAPSIDEBAR_H
#define CHARACTERMAPSIDEBAR_H

#include "saurus/gui/sidebars/basesidebar.h"

class CharacterMapPlugin;

class CharacterMapSidebar : public BaseSidebar {
  Q_OBJECT

  public:
    CharacterMapSidebar(CharacterMapPlugin* plugin, QWidget* parent = nullptr);

  public:
    virtual Qt::DockWidgetArea initialArea() const override;
    virtual bool initiallyVisible() const override;
    virtual int initialWidth() const override;

  protected:
    virtual void load() override;
};

#endif // CHARACTERMAPSIDEBAR_H
