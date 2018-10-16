// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef FORMUPDATE_H
#define FORMUPDATE_H

#include <QDialog>

#include "ui_formupdate.h"

#include "common/miscellaneous/systemfactory.h"
#include "common/network-web/downloader.h"

#include <QNetworkReply>
#include <QPushButton>

class FormUpdate : public QDialog {
  Q_OBJECT

  public:
    explicit FormUpdate(QWidget* parent);
    virtual ~FormUpdate() = default;

    // Returns true if application can self-update
    // on current platform.
    bool isSelfUpdateSupported();

  private slots:

    // Check for updates and interprets the results.
    void checkForUpdates();
    void startUpdate();

    void updateProgress(qint64 bytes_received, qint64 bytes_total);
    void updateCompleted(QNetworkReply::NetworkError status, const QByteArray& contents);
    void saveUpdateFile(const QByteArray& file_contents);

  private:
    void loadAvailableFiles();

    Ui::FormUpdate m_ui = {};
    QPushButton* m_btnUpdate;
    Downloader m_downloader;
    QString m_updateFilePath;
    UpdateInfo m_updateInfo;
    bool m_readyToInstall = false;
    qint64 m_lastDownloadedBytes = 0;
};

#endif // FORMUPDATE_H
