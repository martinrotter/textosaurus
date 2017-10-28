// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/textapplication.h"

#include "gui/dialogs/formmain.h"
#include "gui/messagebox.h"
#include "gui/tabwidget.h"
#include "gui/texteditor.h"

#include <QFileDialog>

TextApplication::TextApplication(QObject* parent) : QObject(parent) {}

QList<TextEditor*> TextApplication::editors() const {
  QList<TextEditor*> editors;

  if (m_tabWidget != nullptr) {
    for (int i = 0; i < m_tabWidget->count(); i++) {
      if (m_tabWidget->tabBar()->tabType(i) == TabBar::TabType::TextEditor) {
        editors.append(qobject_cast<TextEditor*>(m_tabWidget->widget(i)));
      }
    }
  }

  return editors;
}

void TextApplication::loadTextEditorFromFile(const QString& file_path) {
  QFile file(file_path);

  if (!file.exists()) {
    QMessageBox::critical(m_mainForm, tr("Cannot open file"),
                          tr("File '%1' does not exist and cannot be opened.").arg(QDir::toNativeSeparators(file_path)));
    return;
  }

  if (file.size() >= MAX_TEXT_FILE_SIZE) {
    QMessageBox::critical(m_mainForm, tr("Cannot open file"),
                          tr("File '%1' too big. %2 can only open files smaller than %3 MB.").arg(QDir::toNativeSeparators(file_path),
                                                                                                  QSL(APP_NAME),
                                                                                                  QString::number(MAX_TEXT_FILE_SIZE /
                                                                                                                  1000000.0)));
    return;
  }

  int index = addEmptyTextEditor();
  TextEditor* new_editor = m_tabWidget->textEditorAt(index);

  if (new_editor != nullptr) {
    new_editor->loadFromFile(file);
  }
}

int TextApplication::addEmptyTextEditor() {
  return m_tabWidget->addTab(new TextEditor(m_tabWidget),
                             qApp->icons()->fromTheme(QSL("application-text")),
                             tr("Unsaved file"),
                             TabBar::TabType::TextEditor);
}

void TextApplication::setMainForm(FormMain* main_form) {
  m_mainForm = main_form;
  m_tabWidget = main_form->tabWidget();

  connect(m_tabWidget, &TabWidget::currentChanged, this, &TextApplication::changeCurrentEditor);
  connect(m_tabWidget->tabBar(), &TabBar::emptySpaceDoubleClicked, this, &TextApplication::addEmptyTextEditor);
  connect(m_mainForm->m_ui->m_actionFileNew, &QAction::triggered, this, &TextApplication::addEmptyTextEditor);
  connect(m_mainForm->m_ui->m_actionFileOpen, &QAction::triggered, this, &TextApplication::openTextFile);
}

void TextApplication::openTextFile() {
  QString file_path = QFileDialog::getOpenFileName(m_mainForm, tr("Open file"), qApp->documentsFolder(),
                                                   tr("Text files (*.txt);;All files (*)"));

  if (!file_path.isEmpty()) {
    loadTextEditorFromFile(file_path);
  }
}

void TextApplication::changeCurrentEditor(int index) {
  // TODO: je vybranej novej editor, načíst detaily do status baru a jinam.
}
