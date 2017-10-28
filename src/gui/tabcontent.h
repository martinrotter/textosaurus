// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TABCONTENT_H
#define TABCONTENT_H

#include <QWidget>

// Base class for all widgets which are placed inside tabs of TabWidget
class TabContent {
  public:

    // Contructors.
    explicit TabContent();
    virtual ~TabContent();
};

#endif // TABCONTENT_H
