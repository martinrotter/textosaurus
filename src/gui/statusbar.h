// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>

#include <QLabel>

class StatusBar : public QStatusBar {
  Q_OBJECT

  public:
    explicit StatusBar(QWidget* parent = nullptr);
    virtual ~StatusBar();

    void setEncoding(const QString& encoding);
    void setFileType(const QString& file_type);

  private:
    void updateInfo();

  private:
    QLabel* m_lblInfo;
    QString m_infoEncoding;
    QString m_infoEol;
    QString m_infoLexer;
};

#endif // STATUSBAR_H
