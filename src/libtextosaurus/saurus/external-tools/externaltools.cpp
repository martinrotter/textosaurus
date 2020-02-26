// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/external-tools/externaltools.h"

#include "common/miscellaneous/settings.h"
#include "common/network-web/networkfactory.h"
#include "saurus/external-tools/externaltool.h"
#include "saurus/external-tools/predefinedtools.h"
#include "saurus/gui/sidebars/outputsidebar.h"
#include "saurus/gui/tabwidget.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"

#include <functional>
#include <QDir>
#include <QInputDialog>

#include <QAction>
#include <QClipboard>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QPointer>
#include <QRegularExpression>

ExternalTools::ExternalTools(TextApplication* parent)
  : QObject(parent), m_application(parent) {
  loadPredefinedTools();
}

ExternalTools::~ExternalTools() {
  qDeleteAll(m_customTools);
  qDeleteAll(m_predefinedTools);
}

QList<QAction*> ExternalTools::generateToolsMenuTools(QWidget* parent) const {
  QList<QAction*> actions;
  QMap<QString, QMenu*> categories;
  QList<ExternalTool*> tools; tools << m_customTools << m_predefinedTools;

  std::sort(tools.begin(), tools.end(), [](ExternalTool* lhs, ExternalTool* rhs) {
    return QString::localeAwareCompare(lhs->name(), rhs->name()) < 0;
  });

  foreach (ExternalTool* tool, tools) {
    if (tool->addToEditMenu()) {
      continue;
    }

    if (!tool->category().isEmpty()) {
      if (!categories.contains(tool->category())) {
        auto* category_menu = new QMenu(parent);

        category_menu->setTitle(tool->category());

        actions.append(category_menu->menuAction());
        categories.insert(tool->category(), category_menu);
      }

      categories[tool->category()]->addAction(tool->action());
    }
    else {
      actions.append(tool->action());
    }
  }

  std::sort(actions.begin(), actions.end(), [](QAction* lhs, QAction* rhs) {
    if ((rhs->menu() != nullptr) == (lhs->menu() != nullptr)) {
      return QString::localeAwareCompare(lhs->text(), rhs->text()) < 0;
    }
    else if (lhs->menu() != nullptr) {
      return true;
    }
    else {
      return false;
    }
  });

  return actions;
}

QList<QAction*> ExternalTools::generateEditMenuTools(QWidget* parent) const {
  QList<QAction*> actions;
  QMap<QString, QMenu*> categories;
  QList<ExternalTool*> tools; tools << m_customTools << m_predefinedTools;

  std::sort(tools.begin(), tools.end(), [](ExternalTool* lhs, ExternalTool* rhs) {
    return QString::localeAwareCompare(lhs->name(), rhs->name()) < 0;
  });

  foreach (ExternalTool* tool, tools) {
    if (!tool->addToEditMenu()) {
      continue;
    }

    if (!tool->category().isEmpty()) {
      if (!categories.contains(tool->category())) {
        auto* category_menu = new QMenu(parent);

        category_menu->setTitle(tool->category());
        actions.append(category_menu->menuAction());
        categories.insert(tool->category(), category_menu);
      }

      categories[tool->category()]->addAction(tool->action());
    }
    else {
      actions.append(tool->action());
    }
  }

  std::sort(actions.begin(), actions.end(), [](QAction* lhs, QAction* rhs) {
    if ((rhs->menu() != nullptr) == (lhs->menu() != nullptr)) {
      return QString::localeAwareCompare(lhs->text(), rhs->text()) < 0;
    }
    else if (lhs->menu() != nullptr) {
      return true;
    }
    else {
      return false;
    }
  });

  return actions;
}

QList<ExternalTool*> ExternalTools::customTools() const {
  return m_customTools;
}

QList<QAction*> ExternalTools::predefinedToolsActions() const {
  QList<QAction*> act;

  foreach (ExternalTool* tool, m_predefinedTools) {
    act.append(tool->action());
  }

  return act;
}

void ExternalTools::saveExternalTools(const QList<ExternalTool*>& ext_tools) {
  // We persistently save custom tools.
  QSettings sett_ext_tools(qApp->settings()->baseFolder() + QDir::separator() + EXT_TOOLS_CONFIG, QSettings::Format::IniFormat);

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
    sett_ext_tools.setValue(QSL("prompt"), tool->prompt());

    sett_ext_tools.endGroup();
  }
}

void ExternalTools::runSelectedExternalTool() {
  TextEditor* editor = m_application->tabWidget()->currentEditor();

  if (editor != nullptr) {
    auto* tool_to_run = qobject_cast<QAction*>(sender())->data().value<ExternalTool*>();

    connect(tool_to_run, &ExternalTool::toolFinished, this, &ExternalTools::onToolFinished,
            Qt::ConnectionType(Qt::ConnectionType::AutoConnection | Qt::ConnectionType::UniqueConnection));
    connect(tool_to_run, &ExternalTool::partialOutputObtained, this, &ExternalTools::onToolPartialOutputObtained,
            Qt::ConnectionType(Qt::ConnectionType::AutoConnection | Qt::ConnectionType::UniqueConnection));

    // We run the tool.
    runTool(tool_to_run, editor);
  }
}

void ExternalTools::loadPredefinedTools() {
  if (!m_predefinedTools.isEmpty()) {
    return;
  }

  PredefinedTool* insert_date_time = new PredefinedTool(&PredefinedTools::currentDateTime, this);

  insert_date_time->setActionObjectName(QSL("m_actionPredefCurrDateTime"));
  insert_date_time->setCategory(tr("&DateTime"));
  insert_date_time->setName(tr("Insert &DateTime"));
  insert_date_time->setInput(ExternalTool::ToolInput::NoInput);
  insert_date_time->setOutput(ExternalTool::ToolOutput::InsertAtCursorPositionAndReplaceSelection);

  m_predefinedTools.append(insert_date_time);

  PredefinedTool* insert_date = new PredefinedTool(&PredefinedTools::currentDate, this);

  insert_date->setActionObjectName(QSL("m_actionPredefCurrDate"));
  insert_date->setCategory(tr("&DateTime"));
  insert_date->setName(tr("Insert &Date"));
  insert_date->setInput(ExternalTool::ToolInput::NoInput);
  insert_date->setOutput(ExternalTool::ToolOutput::InsertAtCursorPositionAndReplaceSelection);

  m_predefinedTools.append(insert_date);

  PredefinedTool* insert_time = new PredefinedTool(&PredefinedTools::currentTime, this);

  insert_time->setActionObjectName(QSL("m_actionPredefCurrTime"));
  insert_time->setCategory(tr("&DateTime"));
  insert_time->setName(tr("Insert &Time"));
  insert_time->setInput(ExternalTool::ToolInput::NoInput);
  insert_time->setOutput(ExternalTool::ToolOutput::InsertAtCursorPositionAndReplaceSelection);

  m_predefinedTools.append(insert_time);

  PredefinedTool* insert_formatted_datetime = new PredefinedTool(&PredefinedTools::formattedDateTime, this);

  insert_formatted_datetime->setActionObjectName(QSL("m_actionPredefCurrFormatDateTime"));
  insert_formatted_datetime->setCategory(tr("&DateTime"));
  insert_formatted_datetime->setName(tr("Insert &DateTime (Custom Format)"));
  insert_formatted_datetime->setInput(ExternalTool::ToolInput::NoInput);
  insert_formatted_datetime->setOutput(ExternalTool::ToolOutput::InsertAtCursorPositionAndReplaceSelection);

  m_predefinedTools.append(insert_formatted_datetime);

  PredefinedTool* json_beautify = new PredefinedTool(&PredefinedTools::jsonBeautify, this);

  json_beautify->setActionObjectName(QSL("m_actionPredefJsonBeautify"));
  json_beautify->setCategory(tr("&JSON"));
  json_beautify->setName(tr("JSON &Beautify"));
  json_beautify->setInput(ExternalTool::ToolInput::SelectionDocument);
  json_beautify->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);

  m_predefinedTools.append(json_beautify);

  PredefinedTool* json_minify = new PredefinedTool(&PredefinedTools::jsonMinify, this);

  json_minify->setActionObjectName(QSL("m_actionPredefMinify"));
  json_minify->setCategory(tr("&JSON"));
  json_minify->setName(tr("JSON &Minify"));
  json_minify->setInput(ExternalTool::ToolInput::SelectionDocument);
  json_minify->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);

  m_predefinedTools.append(json_minify);

  PredefinedTool* xml_check = new PredefinedTool(&PredefinedTools::xmlCheck, this);

  xml_check->setActionObjectName(QSL("m_actionPredefXmlCheck"));
  xml_check->setCategory(tr("&XML"));
  xml_check->setName(tr("&Check XML syntax"));
  xml_check->setInput(ExternalTool::ToolInput::SelectionDocument);
  xml_check->setOutput(ExternalTool::ToolOutput::DumpToOutputWindow);

  m_predefinedTools.append(xml_check);

  PredefinedTool* xml_beautify = new PredefinedTool(&PredefinedTools::xmlBeautify, this);

  xml_beautify->setActionObjectName(QSL("m_actionPredefXmlBeautify"));
  xml_beautify->setCategory(tr("&XML"));
  xml_beautify->setName(tr("XML &Beautify (Selected String)"));
  xml_beautify->setInput(ExternalTool::ToolInput::SelectionDocument);
  xml_beautify->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);

  m_predefinedTools.append(xml_beautify);

  PredefinedTool* xml_beautify_file = new PredefinedTool(&PredefinedTools::xmlBeautifyFile, this);

  xml_beautify_file->setActionObjectName(QSL("m_actionPredefXmlBeautifyFile"));
  xml_beautify_file->setCategory(tr("&XML"));
  xml_beautify_file->setName(tr("XML &Beautify (File)"));
  xml_beautify_file->setInput(ExternalTool::ToolInput::SavedFile);
  xml_beautify_file->setOutput(ExternalTool::ToolOutput::ReloadFile);

  m_predefinedTools.append(xml_beautify_file);

  PredefinedTool* xml_linearize = new PredefinedTool(&PredefinedTools::xmlLinearize, this);

  xml_linearize->setActionObjectName(QSL("m_actionPredefMinify"));
  xml_linearize->setCategory(tr("&XML"));
  xml_linearize->setName(tr("XML &Linearize/Minify"));
  xml_linearize->setInput(ExternalTool::ToolInput::SelectionDocument);
  xml_linearize->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);

  m_predefinedTools.append(xml_linearize);

  PredefinedTool* tobase64 = new PredefinedTool(&PredefinedTools::toBase64, this);

  tobase64->setActionObjectName(QSL("m_actionPredefToBase64"));
  tobase64->setCategory(tr("&MIME Tools"));
  tobase64->setName(tr("Text → &Base64"));
  tobase64->setInput(ExternalTool::ToolInput::SelectionDocument);
  tobase64->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);

  m_predefinedTools.append(tobase64);

  PredefinedTool* tobase64url = new PredefinedTool(&PredefinedTools::toBase64Url, this);

  tobase64url->setActionObjectName(QSL("m_actionPredefToBase64Url"));
  tobase64url->setCategory(tr("&MIME Tools"));
  tobase64url->setName(tr("Text → &Base64Url"));
  tobase64url->setInput(ExternalTool::ToolInput::SelectionDocument);
  tobase64url->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);

  m_predefinedTools.append(tobase64url);

  PredefinedTool* tohtmlencoded = new PredefinedTool(&PredefinedTools::toHtmlEscaped, this);

  tohtmlencoded->setActionObjectName(QSL("m_actionPredefToHtmlEscaped"));
  tohtmlencoded->setCategory(tr("&MIME Tools"));
  tohtmlencoded->setName(tr("Text → &HTML escaped"));
  tohtmlencoded->setInput(ExternalTool::ToolInput::SelectionDocument);
  tohtmlencoded->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);

  m_predefinedTools.append(tohtmlencoded);

  PredefinedTool* tourlencoded = new PredefinedTool(&PredefinedTools::toUrlEncoded, this);

  tourlencoded->setActionObjectName(QSL("m_actionPredefToUrlEncoded"));
  tourlencoded->setCategory(tr("&MIME Tools"));
  tourlencoded->setName(tr("Text → &URL encoded"));
  tourlencoded->setInput(ExternalTool::ToolInput::SelectionDocument);
  tourlencoded->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);

  m_predefinedTools.append(tourlencoded);

  PredefinedTool* tolower = new PredefinedTool(&PredefinedTools::toLower, this);

  tolower->setActionObjectName(QSL("m_actionPredefToLower"));
  tolower->setCategory(tr("&Text Case Conversion"));
  tolower->setName(tr("to &lower case"));
  tolower->setInput(ExternalTool::ToolInput::SelectionDocument);
  tolower->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);
  tolower->setAddToEditMenu(true);

  m_predefinedTools.append(tolower);

  PredefinedTool* toupper = new PredefinedTool(&PredefinedTools::toUpper, this);

  toupper->setActionObjectName(QSL("m_actionPredefToUpper"));
  toupper->setCategory(tr("&Text Case Conversion"));
  toupper->setName(tr("TO &UPPER CASE"));
  toupper->setInput(ExternalTool::ToolInput::SelectionDocument);
  toupper->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);
  toupper->setAddToEditMenu(true);

  m_predefinedTools.append(toupper);

  PredefinedTool* tosentence = new PredefinedTool(&PredefinedTools::toSentenceCase, this);

  tosentence->setActionObjectName(QSL("m_actionPredefToSentence"));
  tosentence->setCategory(tr("&Text Case Conversion"));
  tosentence->setName(tr("To &sentence case"));
  tosentence->setInput(ExternalTool::ToolInput::SelectionDocument);
  tosentence->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);
  tosentence->setAddToEditMenu(true);

  m_predefinedTools.append(tosentence);

  PredefinedTool* totitle = new PredefinedTool(&PredefinedTools::toTitleCase, this);

  totitle->setActionObjectName(QSL("m_actionPredefToTitle"));
  totitle->setCategory(tr("&Text Case Conversion"));
  totitle->setName(tr("To &Title Case"));
  totitle->setInput(ExternalTool::ToolInput::SelectionDocument);
  totitle->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);
  totitle->setAddToEditMenu(true);

  m_predefinedTools.append(totitle);

  PredefinedTool* toinvert = new PredefinedTool(&PredefinedTools::invertCase, this);

  toinvert->setActionObjectName(QSL("m_actionPredefInvertCase"));
  toinvert->setCategory(tr("&Text Case Conversion"));
  toinvert->setName(tr("&Invert case"));
  toinvert->setInput(ExternalTool::ToolInput::SelectionDocument);
  toinvert->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);
  toinvert->setAddToEditMenu(true);

  m_predefinedTools.append(toinvert);

  PredefinedTool* reverse = new PredefinedTool(&PredefinedTools::reverse, this);

  reverse->setActionObjectName(QSL("m_actionPredefReverseString"));
  reverse->setName(tr("&Invert Text"));
  reverse->setInput(ExternalTool::ToolInput::SelectionDocument);
  reverse->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);
  reverse->setAddToEditMenu(true);

  m_predefinedTools.append(reverse);

  PredefinedTool* frombase64 = new PredefinedTool(&PredefinedTools::fromBase64, this);

  frombase64->setActionObjectName(QSL("m_actionPredefFromBase64"));
  frombase64->setCategory(tr("&MIME Tools"));
  frombase64->setName(tr("&Base64 → text"));
  frombase64->setInput(ExternalTool::ToolInput::SelectionDocument);
  frombase64->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);

  m_predefinedTools.append(frombase64);

  PredefinedTool* frombase64url = new PredefinedTool(&PredefinedTools::fromBase64Url, this);

  frombase64url->setActionObjectName(QSL("m_actionPredefFromBase64Url"));
  frombase64url->setCategory(tr("&MIME Tools"));
  frombase64url->setName(tr("&Base64Url → text"));
  frombase64url->setInput(ExternalTool::ToolInput::SelectionDocument);
  frombase64url->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);

  m_predefinedTools.append(frombase64url);

  PredefinedTool* fromurlencoded = new PredefinedTool(&PredefinedTools::fromUrlEncoded, this);

  fromurlencoded->setActionObjectName(QSL("m_actionPredefFromUrlEncoded"));
  fromurlencoded->setCategory(tr("&MIME Tools"));
  fromurlencoded->setName(tr("&URL encoded → text"));
  fromurlencoded->setInput(ExternalTool::ToolInput::SelectionDocument);
  fromurlencoded->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);

  m_predefinedTools.append(fromurlencoded);

  PredefinedTool* send_to_clbin = new PredefinedTool(&PredefinedTools::sendToClbin, this);

  send_to_clbin->setActionObjectName(QSL("m_actionPredefSendClbin"));
  send_to_clbin->setCategory(tr("&Upload to..."));
  send_to_clbin->setName(tr("Upload to &clbin.com"));
  send_to_clbin->setInput(ExternalTool::ToolInput::SelectionDocument);
  send_to_clbin->setOutput(ExternalTool::ToolOutput::DumpToOutputWindow);

  m_predefinedTools.append(send_to_clbin);

  PredefinedTool* send_to_haste = new PredefinedTool(&PredefinedTools::sendToHastebin, this);

  send_to_haste->setActionObjectName(QSL("m_actionPredefSendHastebin"));
  send_to_haste->setCategory(tr("&Upload to..."));
  send_to_haste->setName(tr("Upload to &hastebin.com"));
  send_to_haste->setInput(ExternalTool::ToolInput::SelectionDocument);
  send_to_haste->setOutput(ExternalTool::ToolOutput::DumpToOutputWindow);

  m_predefinedTools.append(send_to_haste);

  PredefinedTool* send_to_ixio = new PredefinedTool(&PredefinedTools::sendToIxio, this);

  send_to_ixio->setActionObjectName(QSL("m_actionPredefSendIxio"));
  send_to_ixio->setCategory(tr("&Upload to..."));
  send_to_ixio->setName(tr("Upload to &ix.io"));
  send_to_ixio->setInput(ExternalTool::ToolInput::SelectionDocument);
  send_to_ixio->setOutput(ExternalTool::ToolOutput::DumpToOutputWindow);

  m_predefinedTools.append(send_to_ixio);

  // We pre-generate actions for built-in tools.
  foreach (ExternalTool* tool, m_predefinedTools) {
    QAction* act = new QAction(tool->name(), tool);

    act->setObjectName(tool->actionObjectName());
    act->setData(QVariant::fromValue(tool));
    act->setShortcut(QKeySequence::fromString(tool->shortcut(), QKeySequence::SequenceFormat::PortableText));
    act->setShortcutContext(Qt::ApplicationShortcut);

    tool->setAction(act);

    connect(act, &QAction::triggered, this, &ExternalTools::runSelectedExternalTool);
  }
}

void ExternalTools::loadCustomTools() {
  qDeleteAll(m_customTools);
  m_customTools.clear();

  QSettings sett_ext_tools(qApp->settings()->baseFolder() + QDir::separator() + EXT_TOOLS_CONFIG, QSettings::Format::IniFormat);
  QStringList sections = sett_ext_tools.childGroups();

  foreach (const QString& section, sections) {
    sett_ext_tools.beginGroup(section);

    auto* tool = new ExternalTool(this);

    tool->setInterpreter(sett_ext_tools.value(QSL("interpreter"), EXT_TOOL_INTERPRETER).toString());
    tool->setName(sett_ext_tools.value(QSL("name")).toString());
    tool->setScript(sett_ext_tools.value(QSL("script")).toString());
    tool->setPrompt(sett_ext_tools.value(QSL("prompt")).toString());
    tool->setInput(ExternalTool::ToolInput(sett_ext_tools.value(QSL("input"),
                                                                int(ExternalTool::ToolInput::SelectionDocument)).toInt()));
    tool->setOutput(ExternalTool::ToolOutput(sett_ext_tools.value(QSL("output"),
                                                                  int(ExternalTool::ToolOutput::ReplaceSelectionDocument)).toInt()));
    tool->setCategory(sett_ext_tools.value(QSL("category")).toString());
    tool->setShortcut(sett_ext_tools.value(QSL("shortcut")).toString());

    m_customTools.append(tool);

    sett_ext_tools.endGroup();
  }

  // We add extra tools if this is the first time when app runs.
  if (m_customTools.isEmpty()) {
    auto* ext_bash_xml = new ExternalTool(this);

    ext_bash_xml->setScript("IFS=''"
                            "read -r fil"
                            "xmllint --format \"$fil\" > \"${fil}.out\" 2> /dev/null"
                            "mv \"${fil}.out\" \"$fil\"");
    ext_bash_xml->setCategory(tr("Bash (external tool examples)"));
    ext_bash_xml->setInput(ExternalTool::ToolInput::SavedFile);
    ext_bash_xml->setOutput(ExternalTool::ToolOutput::ReloadFile);
    ext_bash_xml->setName("XML - beautify");

    m_customTools.append(ext_bash_xml);

    auto* ext_bash_json = new ExternalTool(this);

    ext_bash_json->setScript("import sys, json;\n\ndata = json.load(sys.stdin)\nprint(json.dumps(data, indent=2))");
    ext_bash_json->setCategory(tr("Python (external tool examples)"));
    ext_bash_json->setInterpreter(QSL("python3.6"));
    ext_bash_json->setInput(ExternalTool::ToolInput::SelectionDocument);
    ext_bash_json->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);
    ext_bash_json->setName("JSON - beautify");

    m_customTools.append(ext_bash_json);

    auto* ext_bash_sha256 = new ExternalTool(this);

    ext_bash_sha256->setScript("sha256sum | head -c 64");
    ext_bash_sha256->setCategory(tr("Bash (external tool examples)"));
    ext_bash_sha256->setInput(ExternalTool::ToolInput::SelectionDocument);
    ext_bash_sha256->setOutput(ExternalTool::ToolOutput::ReplaceSelectionDocument);
    ext_bash_sha256->setName(tr("SHA256 sum of selected/all text"));

    m_customTools.append(ext_bash_sha256);

    auto* ext_python_reverse = new ExternalTool(this);

    ext_python_reverse->setScript("print raw_input().lower()[::-1]");
    ext_python_reverse->setInterpreter(QSL("python3.6"));
    ext_python_reverse->setCategory(tr("Python (external tool examples)"));
    ext_python_reverse->setInput(ExternalTool::ToolInput::CurrentLine);
    ext_python_reverse->setOutput(ExternalTool::ToolOutput::ReplaceCurrentLine);
    ext_python_reverse->setName(tr("Reverse current line"));

    m_customTools.append(ext_python_reverse);

    auto* ext_bash_seq = new ExternalTool(this);

    ext_bash_seq->setScript("IFS=' '\nread -r a b\nunset IFS\nfor i in $(seq $a $b); do printf \"$i \"; done");
    ext_bash_seq->setCategory(tr("Bash (external tool examples)"));
    ext_bash_seq->setPrompt(tr("Enter sequence bounds (for example \"0 10\"):"));
    ext_bash_seq->setInput(ExternalTool::ToolInput::AskForInput);
    ext_bash_seq->setOutput(ExternalTool::ToolOutput::InsertAtCursorPosition);
    ext_bash_seq->setName(tr("Generate sequence"));

    m_customTools.append(ext_bash_seq);

    auto* ext_python_eval = new ExternalTool(this);

    ext_python_eval->setScript("import sys\nimport math\n\nprint(eval(sys.stdin.read()))");
    ext_python_eval->setCategory(tr("Python (external tool examples)"));
    ext_python_eval->setInterpreter(QSL("python3.6"));
    ext_python_eval->setPrompt(tr("Enter Python code:"));
    ext_python_eval->setInput(ExternalTool::ToolInput::AskForInput);
    ext_python_eval->setOutput(ExternalTool::ToolOutput::InsertAtCursorPosition);
    ext_python_eval->setName(tr("Run Python code"));

    m_customTools.append(ext_python_eval);

    auto* ext_bash_garbage = new ExternalTool(this);

    ext_bash_garbage->setScript("read -r count\n\n"
                                "tr -dc a-z1-4 </dev/urandom | tr 1-2 ' \n' | awk 'length==0 || length>50' | tr 3-4 ' ' | sed 's/^ *//' | cat -s | sed 's/ / /g' | fmt | head -n $count");
    ext_bash_garbage->setCategory(tr("Bash (external tool examples)"));
    ext_bash_garbage->setPrompt(tr("Enter number of lines:"));
    ext_bash_garbage->setInput(ExternalTool::ToolInput::AskForInput);
    ext_bash_garbage->setOutput(ExternalTool::ToolOutput::InsertAtCursorPosition);
    ext_bash_garbage->setName(tr("Generate garbage text"));

    m_customTools.append(ext_bash_garbage);

    auto* ext_bash_exec = new ExternalTool(this);

    ext_bash_exec->setScript("IFS=''\n"
                             "read -r fil\n"
                             "eval $fil");
    ext_bash_exec->setCategory(tr("Bash (external tool examples)"));
    ext_bash_exec->setPrompt(tr("Enter Bash code:"));
    ext_bash_exec->setInput(ExternalTool::ToolInput::AskForInput);
    ext_bash_exec->setOutput(ExternalTool::ToolOutput::InsertAtCursorPosition);
    ext_bash_exec->setName(tr("Run Bash code"));

    m_customTools.append(ext_bash_exec);
  }

  foreach (ExternalTool* tool, m_customTools) {
    auto* act = new QAction(tool->name(), tool);

    act->setData(QVariant::fromValue(tool));
    act->setShortcut(QKeySequence::fromString(tool->shortcut(), QKeySequence::SequenceFormat::PortableText));
    act->setShortcutContext(Qt::ApplicationShortcut);

    tool->setAction(act);

    connect(act, &QAction::triggered, this, &ExternalTools::runSelectedExternalTool);
  }
}

void ExternalTools::reloadTools() {
  loadCustomTools();
  emit externalToolsChanged();
}

void ExternalTools::runTool(ExternalTool* tool_to_run, TextEditor* editor) {
  if (tool_to_run->isRunning()) {
    m_application->outputSidebar()->displayOutput(OutputSource::Application,
                                                  tr("Tool '%1' is already running.").arg(tool_to_run->name()),
                                                  QMessageBox::Icon::Warning);
  }
  else {
    QPointer<TextEditor> ptr_editor = editor;
    QString data;

    switch (tool_to_run->input()) {
      case ExternalTool::ToolInput::SelectionDocument:
        data = !ptr_editor->selectionEmpty() ? ptr_editor->getSelText() : ptr_editor->getText(ptr_editor->length() + 1);
        break;

      case ExternalTool::ToolInput::AskForInput: {
        bool ok;

        data = QInputDialog::getText(qApp->mainFormWidget(), tr("Enter input for external tool"),
                                     tool_to_run->prompt(), QLineEdit::EchoMode::Normal, tool_to_run->promptValue(), &ok);

        if (!ok) {
          return;
        }

        break;
      }

      case ExternalTool::ToolInput::CurrentLine:
        data = ptr_editor->getCurLine(-1);
        break;

      case ExternalTool::ToolInput::SavedFile: {
        bool ok = false;

        ptr_editor->save(ok);
        data = ptr_editor->filePath();
        break;
      }

      case ExternalTool::ToolInput::NoInput:
        break;
    }

    if (!tool_to_run->isPredefined()) {
      m_application->outputSidebar()->displayOutput(OutputSource::Application, QString("Running '%1' tool...").arg(tool_to_run->name()));
    }

    tool_to_run->runTool(ptr_editor, data);
  }
}

void ExternalTools::onToolPartialOutputObtained(const QString& output) {
  m_application->outputSidebar()->displayOutput(OutputSource::ExternalTool, output, QMessageBox::Icon::Information);
}

void ExternalTools::onToolFinished(const QPointer<TextEditor>& editor, const QString& output_text,
                                   const QString& error_text, bool success) {
  Q_UNUSED(success)

  if (editor.isNull()) {
    qCriticalNN <<
      QSL("Cannot work properly with tool output, assigned text editor was already destroyed, dumping text to output toolbox.");
    m_application->outputSidebar()->displayOutput(OutputSource::Application,
                                                  tr("Cannot deliver output of external tool, assigned text editor no longer exists."),
                                                  QMessageBox::Icon::Critical);
    return;
  }

  auto* tool = qobject_cast<ExternalTool*>(sender());

  switch (tool->output()) {
    case ExternalTool::ToolOutput::InsertAtCursorPosition: {
      if (!output_text.isEmpty()) {
        QByteArray output_utf = output_text.toUtf8();

        editor->insertText(editor->currentPos(), output_utf.constData());
        editor->gotoPos(editor->currentPos() + output_utf.size());
      }

      break;
    }

    case ExternalTool::ToolOutput::InsertAtCursorPositionAndReplaceSelection: {
      if (!output_text.isEmpty()) {
        QByteArray output_utf = output_text.toUtf8();

        if (editor->selectionEmpty()) {
          editor->insertText(editor->currentPos(), output_utf.constData());
          editor->gotoPos(editor->currentPos() + output_utf.size());
        }
        else {
          editor->replaceSel(output_text.toUtf8().constData());
        }
      }

      break;
    }

    case ExternalTool::ToolOutput::ReplaceCurrentLine: {
      if (!output_text.isEmpty()) {
        QByteArray output_utf = output_text.toUtf8();
        auto line = editor->lineFromPosition(editor->currentPos());
        auto start_line = editor->positionFromLine(line);
        auto end_line = editor->lineEndPosition(line);

        editor->setSel(start_line, end_line);
        editor->replaceSel(output_utf);
      }

      break;
    }

    case ExternalTool::ToolOutput::CopyToClipboard:
      if (!output_text.isEmpty()) {
        qApp->clipboard()->setText(output_text, QClipboard::Mode::Clipboard);
        m_application->outputSidebar()->displayOutput(OutputSource::Application,
                                                      tr("Tool '%1' finished, output copied to clipboard.").arg(tool->name()),
                                                      QMessageBox::Icon::Information);
      }

      break;

    case ExternalTool::ToolOutput::DumpToOutputWindow:
      if (!output_text.isEmpty()) {
        m_application->outputSidebar()->displayOutput(OutputSource::ExternalTool, output_text, QMessageBox::Icon::Information);
      }

      break;

    case ExternalTool::ToolOutput::NewSavedFile: {
      if (!output_text.isEmpty()) {
        m_application->outputSidebar()->displayOutput(OutputSource::Application,
                                                      tr("Tool '%1' finished, opening output in new tab.").arg(tool->name()),
                                                      QMessageBox::Icon::Information);

        m_application->loadTextEditorFromFile(IOFactory::writeToTempFile(output_text.toUtf8()), DEFAULT_TEXT_FILE_ENCODING);
      }

      break;
    }

    case ExternalTool::ToolOutput::ReloadFile:

      // NOTE: We do not probably have to do this.
      // FS watcher will reload automatically.
      break;

    case ExternalTool::ToolOutput::ReplaceSelectionDocument:
      if (!output_text.isEmpty()) {
        if (!editor->selectionEmpty()) {
          editor->replaceSel(output_text.toUtf8().constData());
        }
        else {
          editor->setText(output_text.toUtf8().constData());
        }
      }

      break;

    case ExternalTool::ToolOutput::NoOutput:
      break;
  }

  if (!error_text.isEmpty() && !success) {
    m_application->outputSidebar()->displayOutput(OutputSource::ExternalTool, error_text, QMessageBox::Icon::Critical);
    m_application->outputSidebar()->displayOutput(OutputSource::Application,
                                                  tr("Tool '%1' finished with error(s)...").arg(tool->name()),
                                                  QMessageBox::Icon::Critical);
  }
  else if (!tool->isPredefined()) {
    m_application->outputSidebar()->displayOutput(OutputSource::Application,
                                                  tr("Tool '%1' finished successfully...").arg(tool->name()));
  }
}
