// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/externaltools.h"

#include "external-tools/externaltool.h"
#include "external-tools/predefinedtools.h"
#include "gui/toolbox.h"
#include "miscellaneous/application.h"
#include "miscellaneous/settings.h"
#include "miscellaneous/textapplication.h"
#include "network-web/networkfactory.h"

#include <functional>
#include <QDir>

#include <QAction>
#include <QDateTime>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QPointer>
#include <QRegularExpression>
#include <QtConcurrent/QtConcurrentRun>

ExternalTools::ExternalTools(QObject* parent) : QObject(parent), m_tools(QList<ExternalTool*>()), m_sampleToolsAdded(false) {}

ExternalTools::~ExternalTools() {
  qDeleteAll(m_tools);
  qDebug("Destroying ExternalTools.");
}

QList<QAction*> ExternalTools::generateActions(QWidget* parent, TextApplication* app) const {
  QList<QAction*> actions;
  QMap<QString, QMenu*> categories;

  foreach (ExternalTool* tool, m_tools) {
    QAction* act = new QAction(tool->name(), parent);

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

    connect(act, &QAction::triggered, app, &TextApplication::runSelectedExternalTool);
    connect(tool, &ExternalTool::toolFinished, this, &ExternalTools::onToolFinished);
  }

  return actions;
}

const QList<ExternalTool*> ExternalTools::tools() const {
  return m_tools;
}

void ExternalTools::saveExternalTools(const QList<ExternalTool*>& ext_tools) {
  // We persistently save custom tools.
  QSettings sett_ext_tools(qApp->settings()->pathName() + QDir::separator() + EXT_TOOLS_CONFIG, QSettings::Format::IniFormat);

  sett_ext_tools.clear();
  int i = 0;

  foreach (const ExternalTool* tool, ext_tools) {
    if (tool->isPredefined()) {
      continue;
    }

    sett_ext_tools.beginGroup(QString::number(i++));

    sett_ext_tools.setValue(QSL("interpreter"), tool->interpreter());
    sett_ext_tools.setValue(QSL("name"), tool->name());
    sett_ext_tools.setValue(QSL("script"), tool->script().toUtf8());
    sett_ext_tools.setValue(QSL("input"), int(tool->input()));
    sett_ext_tools.setValue(QSL("output"), int(tool->output()));
    sett_ext_tools.setValue(QSL("category"), tool->category());
    sett_ext_tools.setValue(QSL("shortcut"), tool->shortcut());

    sett_ext_tools.endGroup();
  }
}

void ExternalTools::loadPredefinedTools() {
  PredefinedTool* insert_date_time = new PredefinedTool(&PredefinedTools::currentDateTime, this);

  insert_date_time->setCategory(tr("Insert..."));
  insert_date_time->setName(tr("Date/time"));
  insert_date_time->setInput(ToolInput::NoInput);
  insert_date_time->setOutput(ToolOutput::InsertAtCursorPosition);

  m_tools.append(insert_date_time);

  PredefinedTool* tobase64 = new PredefinedTool(&PredefinedTools::toBase64, this);

  tobase64->setCategory(tr("MIME tools"));
  tobase64->setName(tr("Text → Base64"));
  tobase64->setInput(ToolInput::SelectionDocument);
  tobase64->setOutput(ToolOutput::ReplaceSelectionDocument);

  m_tools.append(tobase64);

  PredefinedTool* tobase64url = new PredefinedTool(&PredefinedTools::toBase64Url, this);

  tobase64url->setCategory(tr("MIME tools"));
  tobase64url->setName(tr("Text → Base64Url"));
  tobase64url->setInput(ToolInput::SelectionDocument);
  tobase64url->setOutput(ToolOutput::ReplaceSelectionDocument);

  m_tools.append(tobase64url);

  PredefinedTool* tohtmlencoded = new PredefinedTool(&PredefinedTools::toHtmlEscaped, this);

  tohtmlencoded->setCategory(tr("MIME tools"));
  tohtmlencoded->setName(tr("Text → HTML escaped"));
  tohtmlencoded->setInput(ToolInput::SelectionDocument);
  tohtmlencoded->setOutput(ToolOutput::ReplaceSelectionDocument);

  m_tools.append(tohtmlencoded);

  PredefinedTool* tourlencoded = new PredefinedTool(&PredefinedTools::toUrlEncoded, this);

  tourlencoded->setCategory(tr("MIME tools"));
  tourlencoded->setName(tr("Text → URL encoded"));
  tourlencoded->setInput(ToolInput::SelectionDocument);
  tourlencoded->setOutput(ToolOutput::ReplaceSelectionDocument);

  m_tools.append(tourlencoded);

  PredefinedTool* tolower = new PredefinedTool(&PredefinedTools::toLower, this);

  tolower->setCategory(tr("Text conversion"));
  tolower->setName(tr("to lower case"));
  tolower->setInput(ToolInput::SelectionDocument);
  tolower->setOutput(ToolOutput::ReplaceSelectionDocument);

  m_tools.append(tolower);

  PredefinedTool* toupper = new PredefinedTool(&PredefinedTools::toUpper, this);

  toupper->setCategory(tr("Text conversion"));
  toupper->setName(tr("TO UPPER CASE"));
  toupper->setInput(ToolInput::SelectionDocument);
  toupper->setOutput(ToolOutput::ReplaceSelectionDocument);

  m_tools.append(toupper);

  PredefinedTool* frombase64 = new PredefinedTool(&PredefinedTools::fromBase64, this);

  frombase64->setCategory(tr("MIME tools"));
  frombase64->setName(tr("Base64 → text"));
  frombase64->setInput(ToolInput::SelectionDocument);
  frombase64->setOutput(ToolOutput::ReplaceSelectionDocument);

  m_tools.append(frombase64);

  PredefinedTool* frombase64url = new PredefinedTool(&PredefinedTools::fromBase64Url, this);

  frombase64url->setCategory(tr("MIME tools"));
  frombase64url->setName(tr("Base64Url → text"));
  frombase64url->setInput(ToolInput::SelectionDocument);
  frombase64url->setOutput(ToolOutput::ReplaceSelectionDocument);

  m_tools.append(frombase64url);

  PredefinedTool* fromurlencoded = new PredefinedTool(&PredefinedTools::fromUrlEncoded, this);

  fromurlencoded->setCategory(tr("MIME tools"));
  fromurlencoded->setName(tr("URL encoded → text"));
  fromurlencoded->setInput(ToolInput::SelectionDocument);
  fromurlencoded->setOutput(ToolOutput::ReplaceSelectionDocument);

  m_tools.append(fromurlencoded);

  PredefinedTool* send_to_clbin = new PredefinedTool(&PredefinedTools::sendToClbin, this);

  send_to_clbin->setCategory(tr("Upload to..."));
  send_to_clbin->setName(tr("clbin.com"));
  send_to_clbin->setInput(ToolInput::SelectionDocument);
  send_to_clbin->setOutput(ToolOutput::DumpToOutputWindow);

  m_tools.append(send_to_clbin);

  PredefinedTool* send_to_github = new PredefinedTool(&PredefinedTools::sendToGithub, this);

  send_to_github->setCategory(tr("Upload to..."));
  send_to_github->setName(tr("github.com"));
  send_to_github->setInput(ToolInput::SelectionDocument);
  send_to_github->setOutput(ToolOutput::DumpToOutputWindow);

  m_tools.append(send_to_github);

  PredefinedTool* send_to_ixio = new PredefinedTool(&PredefinedTools::sendToIxio, this);

  send_to_ixio->setCategory(tr("Upload to..."));
  send_to_ixio->setName(tr("ix.io"));
  send_to_ixio->setInput(ToolInput::SelectionDocument);
  send_to_ixio->setOutput(ToolOutput::DumpToOutputWindow);

  m_tools.append(send_to_ixio);

  PredefinedTool* send_to_sprunge = new PredefinedTool(&PredefinedTools::sendToSprunge, this);

  send_to_sprunge->setCategory(tr("Upload to..."));
  send_to_sprunge->setName(tr("sprunge.us"));
  send_to_sprunge->setInput(ToolInput::SelectionDocument);
  send_to_sprunge->setOutput(ToolOutput::DumpToOutputWindow);

  m_tools.append(send_to_sprunge);
}

void ExternalTools::loadCustomTools() {
  QSettings sett_ext_tools(qApp->settings()->pathName() + QDir::separator() + EXT_TOOLS_CONFIG, QSettings::Format::IniFormat);
  QStringList sections = sett_ext_tools.childGroups();

  foreach (const QString& section, sections) {
    sett_ext_tools.beginGroup(section);

    ExternalTool* tool = new ExternalTool(this);

    tool->setInterpreter(sett_ext_tools.value(QSL("interpreter"), QSL(EXT_TOOL_INTERPRETER)).toString());
    tool->setName(sett_ext_tools.value(QSL("name")).toString());
    tool->setScript(sett_ext_tools.value(QSL("script")).toString());
    tool->setInput(ToolInput(sett_ext_tools.value(QSL("input"), int(ToolInput::SelectionDocument)).toInt()));
    tool->setOutput(ToolOutput(sett_ext_tools.value(QSL("output"), int(ToolOutput::ReplaceSelectionDocument)).toInt()));
    tool->setCategory(sett_ext_tools.value(QSL("category")).toString());
    tool->setShortcut(sett_ext_tools.value(QSL("shortcut")).toString());

    m_tools.append(tool);

    sett_ext_tools.endGroup();
  }

  // We add extra tools if this is the first time when app runs.
  if (qApp->isFirstRun() && !m_sampleToolsAdded) {
    m_sampleToolsAdded = true;

    ExternalTool* ext_bash_xml = new ExternalTool(this);

    ext_bash_xml->setScript("xmllint --format -");
    ext_bash_xml->setCategory(tr("Bash (external tool examples)"));
    ext_bash_xml->setInput(ToolInput::SelectionDocument);
    ext_bash_xml->setOutput(ToolOutput::ReplaceSelectionDocument);
    ext_bash_xml->setName("XML - beautify");

    m_tools.append(ext_bash_xml);

    ExternalTool* ext_bash_json = new ExternalTool(this);

    ext_bash_json->setScript("import sys, json;\n\ndata = json.load(sys.stdin)\nprint(json.dumps(data, indent=2))");
    ext_bash_json->setCategory(tr("Python (external tool examples)"));
    ext_bash_json->setInterpreter(QSL("python2.7"));
    ext_bash_json->setInput(ToolInput::SelectionDocument);
    ext_bash_json->setOutput(ToolOutput::ReplaceSelectionDocument);
    ext_bash_json->setName("JSON - beautify");

    m_tools.append(ext_bash_json);

    saveExternalTools(m_tools);
  }
}

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
      data = !ptr_editor->selectionEmpty() ? ptr_editor->getSelText() : ptr_editor->getText(ptr_editor->length() + 1);
      break;

    case ToolInput::CurrentLine:
      data = ptr_editor->getCurLine(-1);
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

  QFuture<QPair<QString, bool>> future = QtConcurrent::run([tool_to_run, ptr_editor, data]() {
    QPair<QString, bool> result = tool_to_run->runTool(ptr_editor, data);
    return result;
  });
  QFutureWatcher<QPair<QString, bool>>* watched = new QFutureWatcher<QPair<QString, bool>>();

  watched->setFuture(future);
  connect(watched, &QFutureWatcher<QPair<QString, bool>>::finished, this, [this, tool_to_run, editor]() {
    QFutureWatcher<QPair<QString, bool>>* sndr = static_cast<QFutureWatcher<QPair<QString, bool>>*>(sender());
    QPair<QString, bool> result = sndr->result();

    emit toolFinished(tool_to_run, editor, result.first, result.second);
    sndr->deleteLater();
  });
}

void ExternalTools::onToolFinished(const QPointer<TextEditor>& editor, const QString& output_text, bool success) {
  ExternalTool* tool = qobject_cast<ExternalTool*>(sender());
  emit toolFinished(tool, editor, output_text, success);
}
