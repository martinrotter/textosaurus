// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <Qsci/qsciscintilla.h>

class TextEditor : public QsciScintilla {
  Q_OBJECT

  public:
    explicit TextEditor(QWidget* parent = nullptr);

    QString filePath() const;
    void setFilePath(const QString& file_path);

  public slots:
    void loadFromFile(QFile& file);

  private:
    QString m_filePath;
};

#endif // TEXTEDITOR_H
