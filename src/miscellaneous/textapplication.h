// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TEXTAPPLICATION_H
#define TEXTAPPLICATION_H

#include <QObject>

class TextEditor;
class TabWidget;
class FormMain;

class TextApplication : public QObject {
  Q_OBJECT

  public:
    explicit TextApplication(QObject* parent = nullptr);

    QList<TextEditor*> editors()  const;

    void setMainForm(FormMain* main_form);

  public slots:
    void openTextFile();
    void loadTextEditorFromFile(const QString& file_path);
    int addEmptyTextEditor();

  private slots:
    void changeCurrentEditor(int index);

  private:
    TabWidget* m_tabWidget;
    FormMain* m_mainForm;
};

#endif // TEXTAPPLICATION_H
