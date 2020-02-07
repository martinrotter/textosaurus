// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include "definitions/definitions.h"

// *INDENT-OFF*
#include PATH(SCINTILLA_DIR,qt/ScintillaEdit/ScintillaEdit.h)

// *INDENT-ON*

#include "saurus/miscellaneous/syntaxhighlighting.h"

#include <QFile>
#include <QMessageBox>

#define QCOLOR_TO_SPRT(col) ((((col).blue()) << 16) | (((col).green()) << 8) | ((col).red()))
#define RGB_TO_SPRT(b, g, r) (((r) << 16) | ((g) << 8) | (b))

class TextApplication;
class QFileSystemWatcher;

class TEXTOSAURUS_DLLSPEC TextEditor : public ScintillaEdit {
  Q_OBJECT

  public:
    explicit TextEditor(TextApplication* text_app, QWidget* parent = nullptr);

    QString filePath() const;
    Lexer lexer() const;

    QByteArray encoding() const;
    void setEncoding(const QByteArray& encoding);

    bool settingsDirty() const;
    void setSettingsDirty(bool settings_dirty);

    void setReadOnly(bool read_only);

    bool isLog() const;
    void setIsLog(bool is_log);

    bool autoIndentEnabled() const;
    void setAutoIndentEnabled(bool auto_indent_enabled);
    void setFilePath(const QString& file_path);

    QMessageBox::StandardButton currentSaveAgreement() const;

    QString encryptionPassword() const;
    void setEncryptionPassword(const QString& encryption_password);

    static TextEditor* fromTextFile(TextApplication* app, const QString& file_path,
                                    const QString& explicit_encoding, const QString& explicit_filter);

  public slots:
    void resetSaveAgreement();
    void askForSaveAgreement();

    void setFocus();
    void requestVisibility();
    void reloadFromDisk();
    void toggleFolding(int position, int modifiers, int margin);
    void printPreview(bool black_on_white);
    void print(bool black_on_white);
    void save(bool& ok);
    void saveAs(bool& ok, const QString& encoding = QString());

    // Shows/hides line numbers margin and adjusts its size to
    // current line count and zoom.
    void updateLineNumberMarginVisibility();
    void reloadFont();
    void reloadSettings();
    void reloadLexer(const Lexer& default_lexer);

    // Closes the editor, user is asked to save unsaved text document.
    // Given parameter is used to indicate if closing was finished (true)
    // or user cancelled it (false).
    void closeEditor(bool& ok);
    void loadFromFile(const QByteArray& file_data, const QString& file_path, const QString& encoding,
                      const Lexer& default_lexer, int initial_eol_mode);
    void loadFromString(const QString& contents);

  private slots:
    void findAllFromSelectedText();
    void updateUrlHighlights();
    void updateOccurrencesHighlights();
    void uiUpdated(int code);
    void onCharAdded(int chr);
    void onNotification(SCNotification* pscn);
    void onFileExternallyChanged(const QString& file_path);
    void onModified(int type, int position, int length, int lines_added, const QByteArray& text,
                    int line, int fold_now, int fold_prev);

  protected:
    virtual void contextMenuEvent(QContextMenuEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

  signals:
    void readOnlyChanged(bool read_only);
    void editorReloaded();
    void visibilityRequested();
    void loadedFromFile(QString source_file_path);
    void savedToFile(QString destination_file_path);

  private:
    QString requestSaveFileName(QString* selected_filter = nullptr) const;
    void appendSessionFile(const QString& file_name, bool is_nonexistent);

    void detachWatcher();
    void reattachWatcher(const QString& file_path);
    bool isMarginVisible(int margin_number) const;
    void updateLineNumberMarginWidth(sptr_t zoom, QFont font, sptr_t line_count);
    void saveToFile(const QString& file_path, bool& ok, const QString& encoding = QString());

    // Returns filename relative to "user data" folder.
    // NOTE: This is used for saving of unattached editors
    // to session files.
    QString getSessionFile();

  private:
    QMessageBox::StandardButton m_saveAgreement;
    bool m_isLog;
    QFileSystemWatcher* m_fileWatcher;
    bool m_settingsDirty;
    TextApplication* m_textApp;
    QString m_filePath;
    QByteArray m_encoding;
    Lexer m_lexer;
    bool m_autoIndentEnabled;
    QString m_filePathOnEditorQuit;
    QString m_encryptionPassword;
};

#endif // TEXTEDITOR_H
