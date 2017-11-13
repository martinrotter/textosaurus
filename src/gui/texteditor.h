// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <scintilla/qt/ScintillaEdit/ScintillaEdit.h>

#include "definitions/definitions.h"
#include "miscellaneous/syntaxhighlighting.h"

#include <QFile>

#define RGB_TO_INT(r, g, b) ((r) << 16) | ((g) << 8) | (b)

class TextApplication;

class TextEditor : public ScintillaEdit {
  Q_OBJECT

  public:
    explicit TextEditor(TextApplication* text_app, QWidget* parent = nullptr);

    QString filePath() const;
    Lexer lexer() const;

    QByteArray encoding() const;
    void setEncoding(const QByteArray& encoding);

  public slots:
    void save(bool* ok);
    void saveAs(bool* ok, const QString& encoding = QString());

    void reloadFont();
    void reloadSettings();
    void reloadLexer(const Lexer& default_lexer);

    // Closes the editor, user is asked to save unsaved text document.
    // Given parameter is used to indicate if closing was finished (true)
    // or user cancelled it (false).
    void closeEditor(bool* ok);

    // Opens given file in this text editor, file is opened for reading.
    void loadFromFile(QFile& file, const QString& encoding, const Lexer& default_lexer);

  protected:
    void closeEvent(QCloseEvent* event);

  signals:
    void requestedVisibility();
    void loadedFromFile(QString m_sourceFilePath);
    void savedToFile(QString m_destinationFilePath);

  private:
    void saveToFile(const QString& file_path, bool* ok, const QString& encoding = QString());

  private:
    TextApplication* m_textApp;
    QString m_filePath;
    QByteArray m_encoding;
    Lexer m_lexer;
};

#endif // TEXTEDITOR_H
