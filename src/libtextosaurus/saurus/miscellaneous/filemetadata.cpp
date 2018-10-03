// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/miscellaneous/filemetadata.h"

#include "common/miscellaneous/textfactory.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/miscellaneous/textapplicationsettings.h"

#include "common/gui/messagebox.h"

#include <QDir>
#include <QFile>

FileMetadata FileMetadata::getInitialMetadata(const QString& file_path, const QString& explicit_encoding) {
  QFile file(file_path);

  if (!file.exists()) {
    qWarning("File '%s' does not exist and cannot be opened.", qPrintable(file_path));
    return FileMetadata();
  }

  if (file.size() >= MAX_TEXT_FILE_SIZE) {
    QMessageBox::critical(qApp->mainFormWidget(), QObject::tr("Cannot Open File"),
                          QObject::tr("File '%1' too big. %2 can only open files smaller than %3 MB.")
                          .arg(QDir::toNativeSeparators(file_path), QSL(APP_NAME), QString::number(MAX_TEXT_FILE_SIZE / 1000000.0)));
    return FileMetadata();
  }

  if (!file.open(QIODevice::OpenModeFlag::ReadOnly)) {
    QMessageBox::critical(qApp->mainFormWidget(), QObject::tr("Cannot read file"),
                          QObject::tr("File '%1' cannot be opened for reading. Insufficient permissions.")
                          .arg(QDir::toNativeSeparators(file_path)));
    return FileMetadata();
  }

  QString encoding;
  Lexer default_lexer;
  int eol_mode = TextFactory::detectEol(file_path);

  if (eol_mode < 0) {
    qWarning("Auto-detection of EOL mode for file '%s' failed, using app default.", qPrintable(file_path));
    eol_mode = qApp->textApplication()->settings()->eolMode();
  }
  else {
    qDebug("Auto-detected EOL mode is '%d'.", eol_mode);
  }

  if (explicit_encoding.isEmpty()) {
    qDebug("No explicit encoding for file '%s'. Try to detect one.", qPrintable(file_path));

    if ((encoding = TextFactory::detectEncoding(file_path)).isEmpty()) {
      // No encoding auto-detected.
      encoding = DEFAULT_TEXT_FILE_ENCODING;
      qWarning("Auto-detection of encoding failed, using default encoding.");
    }
    else {
      qDebug("Auto-detected encoding is '%s'.", qPrintable(encoding));
    }
  }
  else {
    encoding = explicit_encoding;
  }

  if (file.size() > BIG_TEXT_FILE_SIZE) {
    // File is quite big, we turn some features off to make sure it loads faster.
    MessageBox::show(qApp->mainFormWidget(), QMessageBox::Icon::Warning, QObject::tr("Loading Big File"),
                     QObject::tr("This file is big. %2 will switch some features (for example 'Word Wrap' or syntax highlighting) off to "
                                 "make sure that file loading is not horribly slow.").arg(QSL(APP_NAME)),
                     QString(),
                     QDir::toNativeSeparators(file_path));

    qApp->textApplication()->settings()->setWordWrapEnabled(false);
    default_lexer = qApp->textApplication()->settings()->syntaxHighlighting()->defaultLexer();
  }
  else {
    // We try to detect default lexer.
    default_lexer = qApp->textApplication()->settings()->syntaxHighlighting()->lexerForFile(file_path);
  }

  FileMetadata metadata;

  metadata.m_encoding = encoding;
  metadata.m_eolMode = eol_mode;
  metadata.m_lexer = default_lexer;

  if (file.isOpen()) {
    file.close();
  }

  return metadata;
}
