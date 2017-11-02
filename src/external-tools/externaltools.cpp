// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/externaltools.h"

#include "external-tools/externaltool.h"
#include "gui/toolbox.h"
#include "miscellaneous/application.h"
#include "miscellaneous/settings.h"
#include "miscellaneous/textapplication.h"
#include "network-web/networkfactory.h"

#include <functional>

#include <QAction>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QPointer>
#include <QRegularExpression>

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
  // 1. Insert date/time.
  std::function<QString(const QString&)> get_date =
    [](const QString& data) {
      Q_UNUSED(data)
      return QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate);
    };

  PredefinedTool* insert_date_time = new PredefinedTool(get_date, this);

  insert_date_time->setCategory(tr("Insert..."));
  insert_date_time->setName(tr("Date/time"));
  insert_date_time->setInput(ToolInput::NoInput);
  insert_date_time->setOutput(ToolOutput::InsertAtCursorPosition);

  m_tools.append(insert_date_time);

  // 2. Send to clbin.
  std::function<QString(const QString&)> send_to_clbin_lambda =
    [](const QString& data) {
      QByteArray output;
      QString content = QString("clbin=%1").arg(data);
      NetworkResult result = NetworkFactory::performNetworkOperation(QSL(PASTEBIN_CLBIN),
                                                                     DOWNLOAD_TIMEOUT,
                                                                     content.toUtf8(),
                                                                     output,
                                                                     QNetworkAccessManager::Operation::PostOperation);

      if (result.first == QNetworkReply::NetworkError::NoError) {
        return QString(output).remove(QRegularExpression(QSL("\\s")));
      }
      else {
        return QString();
      }
    };

  PredefinedTool* send_to_clbin = new PredefinedTool(send_to_clbin_lambda, this);

  send_to_clbin->setCategory(tr("Upload to..."));
  send_to_clbin->setName(tr("Upload to clbin.com"));
  send_to_clbin->setInput(ToolInput::SelectionDocument);
  send_to_clbin->setOutput(ToolOutput::DumpToOutputWindow);

  m_tools.append(send_to_clbin);

  // 3. Send to ix.io.
  std::function<QString(const QString&)> send_to_ixio_lambda =
    [](const QString& data) {
      QByteArray output;
      QString content = QString("f:1=%1").arg(data);
      NetworkResult result = NetworkFactory::performNetworkOperation(QSL(PASTEBIN_IXIO),
                                                                     DOWNLOAD_TIMEOUT,
                                                                     content.toUtf8(),
                                                                     output,
                                                                     QNetworkAccessManager::Operation::PostOperation);

      if (result.first == QNetworkReply::NetworkError::NoError) {
        return QString(output).remove(QRegularExpression(QSL("\\s")));
      }
      else {
        return QString();
      }
    };

  PredefinedTool* send_to_ixio = new PredefinedTool(send_to_ixio_lambda, this);

  send_to_ixio->setCategory(tr("Upload to..."));
  send_to_ixio->setName(tr("Upload to ix.io"));
  send_to_ixio->setInput(ToolInput::SelectionDocument);
  send_to_ixio->setOutput(ToolOutput::DumpToOutputWindow);

  m_tools.append(send_to_ixio);

  // 4. Upload to sprunge.us.
  std::function<QString(const QString&)> send_to_sprunge_lambda =
    [](const QString& data) {
      QByteArray output;
      QString content = QString("sprunge=%1").arg(data);
      NetworkResult result = NetworkFactory::performNetworkOperation(QSL("http://sprunge.us/"),
                                                                     DOWNLOAD_TIMEOUT,
                                                                     content.toUtf8(),
                                                                     output,
                                                                     QNetworkAccessManager::Operation::PostOperation);

      if (result.first == QNetworkReply::NetworkError::NoError) {
        return QString(output).remove(QRegularExpression(QSL("\\s")));
      }
      else {
        return QString();
      }
    };

  PredefinedTool* send_to_sprunge = new PredefinedTool(send_to_sprunge_lambda, this);

  send_to_sprunge->setCategory(tr("Upload to..."));
  send_to_sprunge->setName(tr("Upload to sprunge.us"));
  send_to_sprunge->setInput(ToolInput::SelectionDocument);
  send_to_sprunge->setOutput(ToolOutput::DumpToOutputWindow);

  m_tools.append(send_to_sprunge);
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
