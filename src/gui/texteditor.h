// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <Qsci/qsciscintilla.h>

#include "definitions/definitions.h"
#include "miscellaneous/syntaxhighlighting.h"

class TextApplication;

class TextEditor : public QsciScintilla {
  Q_OBJECT

  public:
    explicit TextEditor(TextApplication* text_app, QWidget* parent = nullptr);

    QString filePath() const;
    QByteArray encoding() const;

    TextApplication* textApplication() const;

    //void setLexerWithName(QsciLexer* lexer = nullptr, const QString& lexer_name = QString());

  public slots:
    void save(bool* ok);
    void saveAs(bool* ok, const QString& encoding = QString());

    void reloadSettings();

    // Closes the editor, user is asked to save unsaved text document.
    // Given parameter is used to indicate if closing was finished (true)
    // or user cancelled it (false.
    void closeEditor(bool* ok);

    // Opens given file in this text editor, file is opened for reading.
    void loadFromFile(QFile& file, const QString& encoding, const Lexer& default_lexer);

  protected:

    //void setLexer(QsciLexer* lexer = 0);
    void contextMenuEvent(QContextMenuEvent* event);
    void closeEvent(QCloseEvent* event);

  signals:
    void requestVisibility();
    void loadedFromFile(QString m_sourceFilePath);
    void savedToFile(QString m_destinationFilePath);

  private:
    void reloadLexer();
    void saveToFile(const QString& file_path, bool* ok, const QString& encoding = QString());

  private:
    TextApplication* m_textApp;
    QString m_filePath;
    QByteArray m_encoding;
    Lexer m_lexer;
};

#endif // TEXTEDITOR_H
