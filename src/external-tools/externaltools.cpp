// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/externaltools.h"

#include "external-tools/externaltool.h"
#include "gui/toolbox.h"
#include "miscellaneous/textapplication.h"

#include <functional>

#include <QAction>
#include <QDateTime>
#include <QMenu>
#include <QPointer>

ExternalTools::ExternalTools(QObject* parent) : QObject(parent), m_tools(QList<ExternalTool*>()) {}

ExternalTools::~ExternalTools() {
  qDeleteAll(m_tools);
  qDebug("Destroying ExternalTools.");
}

QList<QAction*> ExternalTools::generateActions(QWidget* parent, TextApplication* app) const {
  QList<QAction*> actions;
  QMap<QString, QMenu*> categories;

  foreach (ExternalTool* tool, m_tools) {
    QAction* act = new QAction(tool->name());

    if (!tool->category().isEmpty()) {
      if (!categories.contains(tool->category())) {
        QMenu* category_menu = new QMenu(parent);

        category_menu->setTitle(tool->category());

        actions.append(category_menu->menuAction());
        categories.insert(tool->category(), category_menu);
      }

      categories[tool->category()]->addAction(act);
    }
    else {
      actions.append(act);
    }

    act->setData(QVariant::fromValue(tool));
    act->setShortcut(QKeySequence::fromString(tool->shortcut(), QKeySequence::SequenceFormat::PortableText));
    act->setShortcutContext(Qt::ApplicationShortcut);

    connect(act, &QAction::triggered, app, &TextApplication::runTool);
    connect(tool, &ExternalTool::toolFinished, this, &ExternalTools::onToolFinished);
  }

  return actions;
}

const QList<ExternalTool*> ExternalTools::tools() const {
  return m_tools;
}

void ExternalTools::loadPredefinedTools() {
  std::function<QString(const QString&)> get_date = [](const QString& data) {
                                                      return QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate);
                                                    };

  PredefinedTool* predefined_inset_date_time = new PredefinedTool(get_date, this);

  predefined_inset_date_time->setShortcut(QSL("CTRL+SHIFT+K"));
  predefined_inset_date_time->setCategory(tr("Insert..."));
  predefined_inset_date_time->setName(tr("Insert date/time"));
  predefined_inset_date_time->setInput(ToolInput::NoInput);
  predefined_inset_date_time->setOutput(ToolOutput::InsertAtCursorPosition);

  m_tools.append(predefined_inset_date_time);
}

void ExternalTools::loadCustomTools() {}

void ExternalTools::reloadTools() {
  qDeleteAll(m_tools);
  m_tools.clear();

  loadPredefinedTools();
  loadCustomTools();

  emit externalToolsChanged();
}

void ExternalTools::runTool(ExternalTool* tool_to_run, TextEditor* editor) {
  QPointer<TextEditor> ptr_editor = editor;
  QString data;

  switch (tool_to_run->input()) {
    case ToolInput::SelectionDocument:
      data = ptr_editor->hasSelectedText() ? ptr_editor->selectedText() : ptr_editor->text();
      break;

    case ToolInput::CurrentLine:
      int line;
      int index;

      ptr_editor->getCursorPosition(&line, &index);
      data = ptr_editor->text(line);
      break;

    case ToolInput::SavedFile:
      bool ok;

      ptr_editor->save(&ok);
      data = ptr_editor->filePath();
      break;

    case ToolInput::NoInput:
    default:
      break;
  }

  tool_to_run->runTool(ptr_editor, data);
}

void ExternalTools::onToolFinished(QPointer<TextEditor> editor, const QString& output_text) {
  ExternalTool* tool = qobject_cast<ExternalTool*>(sender());
  emit toolFinished(tool, editor, output_text);
}
