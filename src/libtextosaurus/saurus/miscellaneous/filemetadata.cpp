// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/miscellaneous/filemetadata.h"

#include "common/exceptions/ioexception.h"
#include "common/exceptions/operationcancelledexception.h"
#include "common/gui/messagebox.h"
#include "common/miscellaneous/cryptofactory.h"
#include "common/miscellaneous/textfactory.h"
#include "saurus/gui/dialogs/formdecryptpasswordprompt.h"
#include "saurus/miscellaneous/application.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/miscellaneous/textapplicationsettings.h"

#include <QDir>
#include <QFile>
#include <QInputDialog>
#include <QTextCodec>

FileMetadata FileMetadata::getInitialMetadata(const QByteArray& data, const QString& file_path, const QString& explicit_encoding) {
  if (data.size() >= MAX_TEXT_FILE_SIZE) {
    QMessageBox::critical(qApp->mainFormWidget(), QObject::tr("Cannot Open File"),
                          QObject::tr("File '%1' too big. %2 can only open files smaller than %3 MB.")
                          .arg(QDir::toNativeSeparators(file_path), QSL(APP_NAME), QString::number(MAX_TEXT_FILE_SIZE / 1000000.0)));
    return FileMetadata();
  }

  QString encoding;
  Lexer default_lexer;
  int eol_mode = TextFactory::detectEol(data.left(FILE_CHUNK_LENGTH_FOR_EOL_DETECTION));

  if (eol_mode < 0) {
    qWarning("Auto-detection of EOL mode for file '%s' failed, using app default.", qPrintable(file_path));
    eol_mode = qApp->textApplication()->settings()->eolMode();
  }
  else {
    qDebug("Auto-detected EOL mode is '%d'.", eol_mode);
  }

  if (explicit_encoding.isEmpty()) {
    qDebug("No explicit encoding for file '%s'. Try to detect one.", qPrintable(file_path));

    if ((encoding = TextFactory::detectEncoding(data.left(FILE_CHUNK_LENGTH_FOR_ENCODING_DETECTION))).isEmpty()) {
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

  if (data.size() > BIG_TEXT_FILE_SIZE) {
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
  metadata.m_filePath = QDir::toNativeSeparators(file_path);

  return metadata;
}

QByteArray FileMetadata::obtainRawFileData(const QString& file_path) {
  QFile file(file_path);

  if (!file.open(QIODevice::OpenModeFlag::ReadOnly)) {
    throw IOException(QObject::tr("insufficient permissions").arg(file_path));
  }

  QByteArray data;

  if (CryptoFactory::isEncrypted(file)) {
    // File is encrypted, decrypt it but ask for password first.
    bool ok;
    QString password = FormDecryptPasswordPrompt::getPasswordFromUser(file, ok);

    if (ok) {
      data = CryptoFactory::decryptData(password, file);
    }
    else {
      throw OperationCancelledException();
    }
  }
  else {
    file.seek(0);
    data = file.readAll();
  }

  file.close();
  return data;
}
