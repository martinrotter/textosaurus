// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>

#include <QLabel>

class TEXTOSAURUS_DLLSPEC StatusBar : public QStatusBar {
  Q_OBJECT

  public:
    explicit StatusBar(QWidget* parent = nullptr);
    virtual ~StatusBar() = default;

    void setEol(int eol_type);
    void setEncoding(const QString& encoding);
    void setFileType(const QString& file_type);

    bool isActive() const;

  public slots:
    void setIsActive(bool isActive);
    void resetActiveState();

  private:
    void updateInfo();

  private:
    QLabel* m_lblInfo;
    QString m_infoEncoding;
    QString m_infoEol;
    QString m_infoLexer;
    bool m_isActive;
};

#endif // STATUSBAR_H
