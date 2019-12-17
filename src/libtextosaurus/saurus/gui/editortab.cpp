// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/editortab.h"

#include "common/miscellaneous/iconfactory.h"
#include "saurus/gui/tabwidget.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"

#include <QCloseEvent>
#include <QLayout>

EditorTab::EditorTab(TextApplication* text_app, TextEditor* editor)
  : Tab(text_app->tabWidget()), m_editor(editor), m_title(QString()), m_toolTip(QString()) {
  auto* lay = new QVBoxLayout(this);

  lay->setMargin(0);
  lay->setSpacing(0);
  lay->addWidget(m_editor, 1);

  updateTitleFromEditor();

  connect(m_editor, &TextEditor::visibilityRequested, this, &Tab::visibilityRequested);
  connect(m_editor, &TextEditor::readOnlyChanged, this, [this](bool read_only) {
    updateIcon(read_only);

    emit iconChanged(m_icon);
  });
  connect(m_editor, &TextEditor::savePointChanged, this, [this]() {
    updateIcon(m_editor->readOnly());
    updateTitleFromEditor();

    emit titleChanged(m_title, m_toolTip);
  });
}

EditorTab::EditorTab(TextApplication* text_app)
  : EditorTab(text_app, new TextEditor(text_app, this)) {}

QMenu* EditorTab::contextMenu() const {
  auto* menu = new QMenu();
  QAction* act_read_only = menu->addAction(qApp->icons()->fromTheme(QSL("lock")), tr("Read-Only Mode"), [this](bool toggle) {
    m_editor->setReadOnly(toggle);
  });

  act_read_only->setCheckable(true);
  act_read_only->setChecked(m_editor->readOnly());

  menu->addAction(qApp->icons()->fromTheme(QSL("document-save")), tr("Save"), [this]() {
    bool ok = false;
    m_editor->save(ok);
  });

  return menu;
}

TextEditor* EditorTab::primaryEditor() const {
  return m_editor;
}

QList<TextEditor*> EditorTab::allEditors() const {
  return QList<TextEditor*>() << primaryEditor();
}

int EditorTab::countOfEditors() const {
  return 1;
}

Tab::TabType EditorTab::tabType() const {
  return Tab::TabType::TextEditor;
}

void EditorTab::closeEvent(QCloseEvent* event) {
  bool ok = false;

  m_editor->closeEditor(ok);

  if (!ok) {
    event->ignore();
  }
  else {
    Tab::closeEvent(event);
  }
}

void EditorTab::updateIcon(bool read_only) {
  if (read_only) {
    m_icon = qApp->icons()->fromTheme(QSL("lock"));
  }
  else {
    m_icon = m_editor->modify() ?
             qApp->icons()->fromTheme(QSL("gtk-dialog-warning")) :
             QIcon();
  }
}

void EditorTab::updateTitleFromEditor() {
  m_title = m_editor ==
            nullptr ?
            QString() :
            (m_editor->filePath().isEmpty() ? tr("New text file") : QFileInfo(m_editor->filePath()).fileName());
  m_toolTip = m_editor == nullptr ? QString() : m_editor->filePath();
}

QString EditorTab::title() const {
  return m_title;
}

QString EditorTab::toolTip() const {
  return m_toolTip;
}

QIcon EditorTab::icon() const {
  return m_icon;
}
