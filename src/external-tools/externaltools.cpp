// For license of this file, see <project-root-folder>/LICENSE.md.

#include "external-tools/externaltools.h"

#include "external-tools/externaltool.h"
#include "external-tools/predefinedtools.h"
#include "gui/docks/outputwindow.h"
#include "miscellaneous/application.h"
#include "miscellaneous/settings.h"
#include "miscellaneous/textapplication.h"
#include "network-web/networkfactory.h"

#include <functional>
#include <QDir>
#include <QInputDialog>

#include <QAction>
#include <QClipboard>
#include <QDateTime>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QPointer>
#include <QRegularExpression>
#include <QtConcurrent/QtConcurrentRun>

ExternalTools::ExternalTools(TextApplication* parent) : QObject(parent), m_application(parent), m_tools(QList<ExternalTool*>()),
  m_sampleToolsAdded(false) {
  connect(this, &ExternalTools::toolFinished, this, &ExternalTools::onToolFinished);
}

ExternalTools::~ExternalTools() {
  qDeleteAll(m_tools);
  qDebug("Destroying ExternalTools.");
}

QList<QAction*> ExternalTools::generateActions(QWidget* parent) const {
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

    connect(act, &QAction::triggered, this, &ExternalTools::runSelectedExternalTool);
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
    sett_ext_tools.setValue(QSL("prompt"), tool->prompt());

    sett_ext_tools.endGroup();
  }
}

void ExternalTools::runSelectedExternalTool() {
  TextEditor* editor = m_application->currentEditor();

  if (editor != nullptr) {
    ExternalTool* tool_to_run = qobject_cast<QAction*>(sender())->data().value<ExternalTool*>();

    m_application->outputWindow()->displayOutput(OutputSource::ExternalTool, QString("Running '%1' tool...").arg(tool_to_run->name()));
    runTool(tool_to_run, editor);
  }
}

void ExternalTools::loadPredefinedTools() {
  PredefinedTool* insert_date_time = new PredefinedTool(&PredefinedTools::currentDateTime, this);

  insert_date_time->setCategory(tr("Insert..."));
  insert_date_time->setName(tr("Date/time"));
  insert_date_time->setInput(ToolInput::NoInput);
  insert_date_time->setOutput(ToolOutput::InsertAtCursorPosition);

  m_tools.append(insert_date_time);

  PredefinedTool* json_beautify = new PredefinedTool(&PredefinedTools::jsonBeautify, this);

  json_beautify->setCategory(tr("JSON"));
  json_beautify->setName(tr("Beautify"));
  json_beautify->setInput(ToolInput::SelectionDocument);
  json_beautify->setOutput(ToolOutput::ReplaceSelectionDocument);

  m_tools.append(json_beautify);

  PredefinedTool* json_minify = new PredefinedTool(&PredefinedTools::jsonMinify, this);

  json_minify->setCategory(tr("JSON"));
  json_minify->setName(tr("Minify"));
  json_minify->setInput(ToolInput::SelectionDocument);
  json_minify->setOutput(ToolOutput::ReplaceSelectionDocument);

  m_tools.append(json_minify);

  PredefinedTool* xml_check = new PredefinedTool(&PredefinedTools::xmlCheck, this);

  xml_check->setCategory(tr("XML"));
  xml_check->setName(tr("Check XML syntax"));
  xml_check->setInput(ToolInput::SelectionDocument);
  xml_check->setOutput(ToolOutput::DumpToOutputWindow);

  m_tools.append(xml_check);

  PredefinedTool* xml_beautify = new PredefinedTool(&PredefinedTools::xmlBeautify, this);

  xml_beautify->setCategory(tr("XML"));
  xml_beautify->setName(tr("Beautify"));
  xml_beautify->setInput(ToolInput::SelectionDocument);
  xml_beautify->setOutput(ToolOutput::ReplaceSelectionDocument);

  m_tools.append(xml_beautify);

  PredefinedTool* xml_linearize = new PredefinedTool(&PredefinedTools::xmlLinearize, this);

  xml_linearize->setCategory(tr("XML"));
  xml_linearize->setName(tr("Linearize/Minfy"));
  xml_linearize->setInput(ToolInput::SelectionDocument);
  xml_linearize->setOutput(ToolOutput::ReplaceSelectionDocument);

  m_tools.append(xml_linearize);

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
    tool->setPrompt(sett_ext_tools.value(QSL("prompt")).toString());
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

    ext_bash_xml->setScript("IFS=''"
                            "read -r fil"
                            "xmllint --format \"$fil\" > \"${fil}.out\" 2> /dev/null"
                            "mv \"${fil}.out\" \"$fil\"");
    ext_bash_xml->setCategory(tr("Bash (external tool examples)"));
    ext_bash_xml->setInput(ToolInput::SavedFile);
    ext_bash_xml->setOutput(ToolOutput::ReloadFile);
    ext_bash_xml->setName("XML - beautify");

    m_tools.append(ext_bash_xml);

    ExternalTool* ext_bash_json = new ExternalTool(this);

    ext_bash_json->setScript("import sys, json;\n\ndata = json.load(sys.stdin)\nprint(json.dumps(data, indent=2))");
    ext_bash_json->setCategory(tr("Python (external tool examples)"));
    ext_bash_json->setInterpreter(QSL("python3.6"));
    ext_bash_json->setInput(ToolInput::SelectionDocument);
    ext_bash_json->setOutput(ToolOutput::ReplaceSelectionDocument);
    ext_bash_json->setName("JSON - beautify");

    m_tools.append(ext_bash_json);

    ExternalTool* ext_bash_sha256 = new ExternalTool(this);

    ext_bash_sha256->setScript("sha256sum | head -c 64");
    ext_bash_sha256->setCategory(tr("Bash (external tool examples)"));
    ext_bash_sha256->setInput(ToolInput::SelectionDocument);
    ext_bash_sha256->setOutput(ToolOutput::ReplaceSelectionDocument);
    ext_bash_sha256->setName(tr("Get SHA256 sum of selected/all text"));

    m_tools.append(ext_bash_sha256);

    ExternalTool* ext_python_reverse = new ExternalTool(this);

    ext_python_reverse->setScript("print raw_input().lower()[::-1]");
    ext_python_reverse->setInterpreter(QSL("python3.6"));
    ext_python_reverse->setCategory(tr("Python (external tool examples)"));
    ext_python_reverse->setInput(ToolInput::CurrentLine);
    ext_python_reverse->setOutput(ToolOutput::ReplaceCurrentLine);
    ext_python_reverse->setName(tr("Reverse current line"));

    m_tools.append(ext_python_reverse);

    ExternalTool* ext_bash_seq = new ExternalTool(this);

    ext_bash_seq->setScript("IFS=' '\nread -r a b\nunset IFS\nfor i in $(seq $a $b); do printf \"$i \"; done");
    ext_bash_seq->setCategory(tr("Bash (external tool examples)"));
    ext_bash_seq->setPrompt(tr("Enter sequence bounds (for example \"0 10\"):"));
    ext_bash_seq->setInput(ToolInput::AskForInput);
    ext_bash_seq->setOutput(ToolOutput::InsertAtCursorPosition);
    ext_bash_seq->setName(tr("Generate sequence"));

    m_tools.append(ext_bash_seq);

    ExternalTool* ext_python_eval = new ExternalTool(this);

    ext_python_eval->setScript("import sys\nimport math\n\nprint(eval(sys.stdin.read()))");
    ext_python_eval->setCategory(tr("Python (external tool examples)"));
    ext_python_eval->setInterpreter(QSL("python3.6"));
    ext_python_eval->setPrompt(tr("Enter Python code:"));
    ext_python_eval->setInput(ToolInput::AskForInput);
    ext_python_eval->setOutput(ToolOutput::InsertAtCursorPosition);
    ext_python_eval->setName(tr("Run Python code"));

    m_tools.append(ext_python_eval);

    ExternalTool* ext_bash_garbage = new ExternalTool(this);

    ext_bash_garbage->setScript("read -r count\n\n"
                                "tr -dc a-z1-4 </dev/urandom | tr 1-2 ' \n' | awk 'length==0 || length>50' | tr 3-4 ' ' | sed 's/^ *//' | cat -s | sed 's/ / /g' | fmt | head -n $count");
    ext_bash_garbage->setCategory(tr("Bash (external tool examples)"));
    ext_bash_garbage->setPrompt(tr("Enter number of lines:"));
    ext_bash_garbage->setInput(ToolInput::AskForInput);
    ext_bash_garbage->setOutput(ToolOutput::InsertAtCursorPosition);
    ext_bash_garbage->setName(tr("Generate garbage text"));

    m_tools.append(ext_bash_garbage);

    ExternalTool* ext_bash_exec = new ExternalTool(this);

    ext_bash_exec->setScript("IFS=''\n"
                             "read -r fil\n"
                             "eval $fil");
    ext_bash_exec->setCategory(tr("Bash (external tool examples)"));
    ext_bash_exec->setPrompt(tr("Enter Bash code:"));
    ext_bash_exec->setInput(ToolInput::AskForInput);
    ext_bash_exec->setOutput(ToolOutput::InsertAtCursorPosition);
    ext_bash_exec->setName(tr("Run Bash code"));

    m_tools.append(ext_bash_exec);

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
  if (tool_to_run->isRunning()) {
    emit toolFinished(tool_to_run, editor, tr("Tool '%1' is already running.").arg(tool_to_run->name()), false);

    return;
  }

  QPointer<TextEditor> ptr_editor = editor;
  QString data;

  switch (tool_to_run->input()) {
    case ToolInput::SelectionDocument:
      data = !ptr_editor->selectionEmpty() ? ptr_editor->getSelText() : ptr_editor->getText(ptr_editor->length() + 1);
      break;

    case ToolInput::AskForInput: {
      bool ok;

      data = QInputDialog::getText(qApp->mainFormWidget(), tr("Enter input for external tool"),
                                   tool_to_run->prompt(), QLineEdit::EchoMode::Normal, QString(), &ok);

      if (!ok) {
        return;
      }

      break;
    }

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

void ExternalTools::onToolFinished(ExternalTool* tool, const QPointer<TextEditor>& editor, const QString& output_text, bool success) {
  if (editor.isNull()) {
    qCritical("Cannot work properly with tool output, assigned text editor was already destroyed, dumping text to output toolbox.");
    m_application->outputWindow()->displayOutput(OutputSource::ExternalTool,
                                                 tr("Cannot deliver output of external tool, assigned text editor no longer exists."),
                                                 QMessageBox::Icon::Critical);
    return;
  }

  if (!success) {
    m_application->outputWindow()->displayOutput(OutputSource::ExternalTool, output_text, QMessageBox::Icon::Critical);
    return;
  }

  switch (tool->output()) {
    case ToolOutput::InsertAtCursorPosition: {
      QByteArray output_utf = output_text.toUtf8();

      editor->insertText(editor->currentPos(), output_utf.constData());
      editor->gotoPos(editor->currentPos() + output_utf.size());
      break;
    }

    case ToolOutput::ReplaceCurrentLine: {
      QByteArray output_utf = output_text.toUtf8();
      auto line = editor->lineFromPosition(editor->currentPos());
      auto start_line = editor->positionFromLine(line);
      auto end_line = editor->lineEndPosition(line);

      editor->setSel(start_line, end_line);
      editor->replaceSel(output_utf);
      break;
    }

    case ToolOutput::CopyToClipboard:
      qApp->clipboard()->setText(output_text, QClipboard::Mode::Clipboard);
      m_application->outputWindow()->displayOutput(OutputSource::ExternalTool,
                                                   tr("Tool '%1' finished, output copied to clipboard.").arg(tool->name()),
                                                   QMessageBox::Icon::Information);
      break;

    case ToolOutput::DumpToOutputWindow:
      m_application->outputWindow()->displayOutput(OutputSource::ExternalTool, output_text, QMessageBox::Icon::Information);
      break;

    case ToolOutput::NewSavedFile: {
      m_application->outputWindow()->displayOutput(OutputSource::ExternalTool,
                                                   tr("Tool '%1' finished, opening output in new tab.").arg(tool->name()),
                                                   QMessageBox::Icon::Information);

      m_application->loadTextEditorFromFile(IOFactory::writeToTempFile(output_text.toUtf8()), DEFAULT_TEXT_FILE_ENCODING);
      break;
    }

    case ToolOutput::ReloadFile:
      editor->reloadFromDisk();
      break;

    case ToolOutput::ReplaceSelectionDocument:
      if (!editor->selectionEmpty()) {
        editor->replaceSel(output_text.toUtf8().constData());
      }
      else {
        editor->setText(output_text.toUtf8().constData());
      }

      break;

    case ToolOutput::NoOutput:
    default:
      break;
  }
}
