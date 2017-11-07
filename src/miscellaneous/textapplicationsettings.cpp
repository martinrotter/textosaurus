// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/textapplicationsettings.h"

#include "miscellaneous/application.h"
#include "miscellaneous/settings.h"
#include "miscellaneous/textapplication.h"

#include <QAction>
#include <QDir>
#include <QFileInfo>

TextApplicationSettings::TextApplicationSettings(TextApplication* parent)
  : QObject(parent), m_textApplication(parent), m_externalTools(new ExternalTools(parent)), m_fileFilters(QStringList()) {}

TextApplicationSettings::~TextApplicationSettings() {
  qDebug("Destroying TextApplicationSettings");
}

QsciLexer* TextApplicationSettings::lexerForFile(const QString& file_name, const QString& file_filter) {

  return nullptr;
}

QStringList TextApplicationSettings::fileFilters() {
  if (m_fileFilters.isEmpty()) {
    m_fileFilters << tr("All files (*)")
                  << tr("Text files (*.txt)")
                  << tr("AVS files (*.avs)")
                  << tr("Shell scripts (*.sh *.bash *.zsh *.ksh)")
                  << tr("Batch scripts (*.bat *.cmd)")
                  << tr("CMake scripts (*.txt *.cmake)")
                  << tr("CoffeeScript files (*.litcoffee)")
                  << tr("C++ files (*.h *.hpp *.cpp *.c *.cc *.cxx)")
                  << tr("C# files (*.cs)")
                  << tr("D files (*.d)")
                  << tr("Diff files (*.diff)")
                  << tr("Fortran files (*.for *.f90 *.f95 *.f2k *.f23)")
                  << tr("Fortran77 files (*.f77)")
                  << tr("HTML files (*.html *.htm *.shtml *.shtm *.xhtml *.xht *.hta)")
                  << tr("IDL files (*.avs)")
                  << tr("Java files (*.java)")
                  << tr("JavaScript files (*.js)")
                  << tr("JSON files (*.json)")
                  << tr("Lua files (*.lua)")
                  << tr("Make files (*.mak *makefile)")
                  << tr("Markdown files (*.markdown)")

                  << tr("Matlab files (*.m)")
                  << tr("Octave files (*.m)")
                  << tr("Pascal files (*.pas *.pp *.p *.inc *.lpr)")
                  << tr("Perl files (*.pl *.pm *.plx)")
                  << tr("PO files (*.po)")
                  << tr("PostScript files (*.ps)")
                  << tr("POV files (*.pov)")
                  << tr("Properties files (*.properties)")
                  << tr("Python files (*.py *.pyw)")
                  << tr("Ruby files (*.rb *.rbw)")
                  << tr("Spice files (*.scp *.out)")
                  << tr("SQL files (*.sql)")
                  << tr("TCL files (*.tcl)")
                  << tr("TeX files (*.tex *.latex)")
                  << tr("VeriLog files (*.v *.sv *.vh *.svh)")
                  << tr("VHDL files (*.vhd *.vhdl)")
                  << tr("XML files (*.xml *.xaml *.xsl *.xslt *.xsd *.xul *.kml *.svg *.mxml "
          "*.xsml *.wsdl *.xlf *.xliff *.xbl *.sxbl *.sitemap *.gml *.gpx *.plist)")
                  << tr("YAML files (*.yml *.yaml)");
  }

  return m_fileFilters;
}

QStringList TextApplicationSettings::recentFiles() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::RecentFiles)).toStringList();
}

void TextApplicationSettings::setRecentFiles(const QStringList& recent_files) {
  qApp->settings()->setValue(GROUP(Editor), Editor::RecentFiles, recent_files);
}

QsciScintilla::EolMode TextApplicationSettings::eolMode() const {
  return static_cast<QsciScintilla::EolMode>(qApp->settings()->value(GROUP(Editor), SETTING(Editor::EolMode)).toInt());
}

bool TextApplicationSettings::wordWrapEnabled() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::WordWrap)).toBool();
}

QString TextApplicationSettings::loadSaveDefaultDirectory() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::DefaultLoadSaveDirectory)).toString();
}

bool TextApplicationSettings::viewWhitespaces() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::ViewWhitespaces)).toBool();
}

bool TextApplicationSettings::viewEols() const {
  return qApp->settings()->value(GROUP(Editor), SETTING(Editor::ViewEols)).toBool();
}

void TextApplicationSettings::setLoadSaveDefaultDirectory(const QString& file_path) {
  const QString normalized_file_path = QDir::toNativeSeparators(file_path);
  const QString normalized_folder = QDir::toNativeSeparators(QFileInfo(file_path).absolutePath());

  qApp->settings()->setValue(GROUP(Editor), Editor::DefaultLoadSaveDirectory, normalized_folder);

  // Also, we remember this for "Recently opened files" feature.
  QStringList recent_files = recentFiles();

  if (recent_files.contains(normalized_file_path)) {
    recent_files.removeOne(normalized_file_path);
  }

  // We move it to last position.
  recent_files.append(normalized_file_path);
  setRecentFiles(recent_files);
}

void TextApplicationSettings::setWordWrapEnabled(bool enabled) {
  qApp->settings()->setValue(GROUP(Editor), Editor::WordWrap, enabled);
  emit settingsChanged(true, false);
}

void TextApplicationSettings::setEolModeFromAction(QAction* act) {
  QsciScintilla::EolMode new_mode = static_cast<QsciScintilla::EolMode>(act->data().toInt());

  qApp->settings()->setValue(GROUP(Editor), Editor::EolMode, int(new_mode));
  emit settingsChanged(true, false);
}

ExternalTools* TextApplicationSettings::externalTools() const {
  return m_externalTools;
}

void TextApplicationSettings::setViewWhitespaces(bool view) {
  qApp->settings()->setValue(GROUP(Editor), Editor::ViewWhitespaces, view);
  emit settingsChanged(true, false);
}

void TextApplicationSettings::setViewEols(bool view) {
  qApp->settings()->setValue(GROUP(Editor), Editor::ViewEols, view);
  emit settingsChanged(true, false);
}
