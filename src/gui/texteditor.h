// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <scintilla/qt/ScintillaEdit/ScintillaEdit.h>

#include "definitions/definitions.h"
#include "miscellaneous/syntaxhighlighting.h"

#include <QFile>

#define RGB_TO_SPRT(r, g, b) ((r) << 16) | ((g) << 8) | (b)

class TextApplication;

class TextEditor : public ScintillaEdit {
  Q_OBJECT

  public:
    explicit TextEditor(TextApplication* text_app, QWidget* parent = nullptr);

    QString filePath() const;
    Lexer lexer() const;
    QByteArray encoding() const;
    void setEncoding(const QByteArray& encoding);

    bool settingsDirty() const;
    void setSettingsDirty(bool settings_dirty);

    static TextEditor* fromTextFile(TextApplication* app, const QString& file_path, const QString& explicit_encoding = QString());

  public slots:
    void reloadFromDisk();
    void toggleFolding(int position, int modifiers, int margin);
    void printPreview();
    void print();
    void save(bool* ok);
    void saveAs(bool* ok, const QString& encoding = QString());

    // Shows/hides line numbers margin and adjusts its size to
    // current line count and zoom.
    void updateLineNumberMarginVisibility();
    void reloadFont();
    void reloadSettings();
    void reloadLexer(const Lexer& default_lexer);

    // Closes the editor, user is asked to save unsaved text document.
    // Given parameter is used to indicate if closing was finished (true)
    // or user cancelled it (false).
    void closeEditor(bool* ok);
    void loadFromFile(QFile& file, const QString& encoding, const Lexer& default_lexer, int initial_eol_mode);
    void loadFromString(const QString& contents);

  private slots:
    void onModified(int type, int position, int length, int lines_added, const QByteArray& text,
                    int line, int fold_now, int fold_prev);

  protected:
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void closeEvent(QCloseEvent* event);

  signals:
    void editorReloaded();
    void requestedVisibility();
    void loadedFromFile(QString source_file_path);
    void savedToFile(QString destination_file_path);

  private:
    bool isMarginVisible(int margin_number) const;
    void updateLineNumberMarginWidth(int zoom, QFont font, int line_count);
    void saveToFile(const QString& file_path, bool* ok, const QString& encoding = QString());

  private:
    int m_indicatorStart, m_indicatorStop;
    bool m_settingsDirty;
    TextApplication* m_textApp;
    QString m_filePath;
    QByteArray m_encoding;
    Lexer m_lexer;
};

#endif // TEXTEDITOR_H
