// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <Qsci/qsciscintilla.h>

class TextEditor : public QsciScintilla {
  Q_OBJECT

  public:
    explicit TextEditor(QWidget* parent = nullptr);

    QString filePath() const;
    QByteArray encoding() const;

  public slots:
    void reloadSettings();

    // Opens given file in this text editor, file is opened for reading.
    void loadFromFile(QFile& file, const QString& encoding);

  signals:
    void loadedFromFile(QString m_sourceFilePath);
    void savedToFile(QString m_destinationFilePath);

  private:
    QString m_filePath;
    QByteArray m_encoding;
};

#endif // TEXTEDITOR_H
