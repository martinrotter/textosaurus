// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TEXTAPPLICATION_H
#define TEXTAPPLICATION_H

#include <QObject>

class TabWidget;
class FormMain;

class TextApplication : public QObject {
  Q_OBJECT

  public:
    explicit TextApplication(QObject* parent = nullptr);

    void setMainForm(FormMain* main_form);

  private:
    TabWidget* m_tabWidget;
    FormMain* m_mainForm;
};

#endif // TEXTAPPLICATION_H
