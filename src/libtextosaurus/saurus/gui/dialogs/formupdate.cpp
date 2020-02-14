// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/dialogs/formupdate.h"

#include "ui_formupdate.h"

#include "common/gui/guiutilities.h"
#include "common/gui/messagebox.h"
#include "common/miscellaneous/iconfactory.h"
#include "common/miscellaneous/iofactory.h"
#include "common/network-web/downloader.h"
#include "common/network-web/networkfactory.h"
#include "common/network-web/webfactory.h"
#include "definitions/definitions.h"
#include "saurus/miscellaneous/application.h"

#include <QNetworkReply>

#if defined(Q_OS_WIN)
#include <Windows.h>
#endif

FormUpdate::FormUpdate(QWidget* parent)
  : QDialog(parent) {
  m_ui.setupUi(this);
  m_ui.m_lblCurrentRelease->setText(APP_VERSION);
  m_ui.m_tabInfo->removeTab(1);
  m_ui.m_buttonBox->setEnabled(false);

  // Set flags and attributes.
  GuiUtilities::applyDialogProperties(*this, qApp->icons()->fromTheme(QSL("help-about")));
  connect(&m_downloader, &Downloader::progress, this, &FormUpdate::updateProgress);
  connect(&m_downloader, &Downloader::completed, this, &FormUpdate::updateCompleted);

  if (isSelfUpdateSupported()) {
    m_btnUpdate = m_ui.m_buttonBox->addButton(tr("Download selected update"), QDialogButtonBox::ActionRole);
    m_btnUpdate->setToolTip(tr("Download new installation files."));
  }
  else {
    m_btnUpdate = m_ui.m_buttonBox->addButton(tr("Go to application website"), QDialogButtonBox::ActionRole);
    m_btnUpdate->setToolTip(tr("Go to application website to get update packages manually."));
  }

  m_btnUpdate->setVisible(false);
  connect(m_btnUpdate, &QPushButton::clicked, this, &FormUpdate::startUpdate);
  checkForUpdates();
}

bool FormUpdate::isSelfUpdateSupported() {
#if defined(Q_OS_WIN)
  return true;
#else
  return false;
#endif
}

void FormUpdate::checkForUpdates() {
  connect(qApp->system(),
          &SystemFactory::updatesChecked,
          this,
          [this](const QPair<QList<UpdateInfo>, QNetworkReply::NetworkError>& update) {
    m_ui.m_buttonBox->setEnabled(true);
    disconnect(qApp->system(), &SystemFactory::updatesChecked, nullptr, nullptr);

    if (update.second != QNetworkReply::NoError) {
      m_updateInfo = UpdateInfo();
      m_ui.m_tabInfo->setEnabled(false);

      //: Unknown release.
      m_ui.m_lblAvailableRelease->setText(tr("unknown"));
      m_ui.m_txtChanges->clear();
      m_ui.m_lblStatus->setStatus(WidgetWithStatus::StatusType::Error,
                                  tr("Error: '%1'.").arg(NetworkFactory::networkErrorText(update.second)),
                                  tr("List with updates was not\ndownloaded successfully."));
    }
    else {
      const bool self_update_supported = isSelfUpdateSupported();

      if (!update.first.isEmpty()) {
        m_updateInfo = update.first.at(0);
        m_ui.m_tabInfo->setEnabled(true);
        m_ui.m_lblAvailableRelease->setText(m_updateInfo.m_availableVersion);
        m_ui.m_txtChanges->setText(m_updateInfo.m_changes);
      }

      if (qApp->system()->isVersionNewer(m_updateInfo.m_availableVersion, APP_VERSION)) {
        m_btnUpdate->setVisible(true);
        m_ui.m_lblStatus->setStatus(WidgetWithStatus::StatusType::Ok,
                                    tr("New release available."),
                                    tr("This is a new version which can be\ndownloaded."));

        if (self_update_supported) {
          loadAvailableFiles();
        }
      }
      else {
        m_ui.m_lblStatus->setStatus(WidgetWithStatus::StatusType::Warning,
                                    tr("No new release available."),
                                    tr("This release is not newer than\ncurrently installed one."));
      }
    }
  });
  qApp->system()->checkForUpdates();
}

void FormUpdate::updateProgress(qint64 bytes_received, qint64 bytes_total) {
  if (bytes_received - m_lastDownloadedBytes > 500000 || m_lastDownloadedBytes == 0) {
    m_ui.m_lblStatus->setStatus(WidgetWithStatus::StatusType::Information,
                                tr("Downloaded %1% (update size is %2 kB).").arg(QString::number(bytes_total ==
                                                                                                 0 ? 0 : (bytes_received * 100.0) /
                                                                                                 bytes_total,
                                                                                                 'f',
                                                                                                 2),
                                                                                 QString::number(bytes_total / 1000.0,
                                                                                                 'f',
                                                                                                 2)),
                                tr("Downloading update..."));
    m_ui.m_lblStatus->repaint();
    m_lastDownloadedBytes = bytes_received;
  }
}

void FormUpdate::saveUpdateFile(const QByteArray& file_contents) {
  const QString url_file = m_ui.m_listFiles->currentItem()->data(Qt::UserRole).toString();
  const QString temp_directory = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

  if (!temp_directory.isEmpty()) {
    const QString output_file_name = url_file.mid(url_file.lastIndexOf('/') + 1);
    QFile output_file(temp_directory + QDir::separator() + output_file_name);

    if (output_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      qDebug().noquote().nospace() << QSL("Storing update file to temporary location '")
                                   << QDir::toNativeSeparators(output_file.fileName())
                                   << QSL("'.");
      output_file.write(file_contents);
      output_file.flush();
      output_file.close();
      qDebug().noquote() << QSL("Update file contents was successfuly saved.");
      m_updateFilePath = output_file.fileName();
      m_readyToInstall = true;
    }
    else {
      qDebug().noquote().nospace() << QSL("Cannot save downloaded update file because target temporary file '")
                                   << output_file_name
                                   << QSL("' cannot be opened for writing.");
    }
  }
  else {
    qDebug().noquote() << QSL("Cannot save downloaded update file because no TEMP directory is available.");
  }
}

void FormUpdate::loadAvailableFiles() {
  m_ui.m_listFiles->clear();

  foreach (const UpdateUrl& url, m_updateInfo.m_urls) {
    if (qApp->system()->supportedUpdateFiles().match(url.m_name).hasMatch()) {
      QListWidgetItem* item = new QListWidgetItem(url.m_name + QSL(" (") + url.m_size + QSL(")"));

      item->setData(Qt::UserRole, url.m_fileUrl);
      item->setToolTip(url.m_fileUrl);
      m_ui.m_listFiles->addItem(item);
    }
  }

  if (m_ui.m_listFiles->count() > 0) {
    m_ui.m_listFiles->setCurrentRow(0);
  }
  else {
    m_btnUpdate->setEnabled(false);
  }

  m_ui.m_tabInfo->addTab(m_ui.tabFiles, tr("Available update files"));
  m_ui.m_tabInfo->setCurrentIndex(1);
}

void FormUpdate::updateCompleted(QNetworkReply::NetworkError status, const QByteArray& contents) {
  qDebug().noquote().nospace() << QSL("Download of application update file was completed with code '")
                               << status
                               << QSL("'.");

  switch (status) {
    case QNetworkReply::NoError:
      saveUpdateFile(contents);
      m_ui.m_lblStatus->setStatus(WidgetWithStatus::StatusType::Ok, tr("Downloaded successfully"),
                                  tr("Package was downloaded successfully.\nYou can install it now."));
      m_btnUpdate->setText(tr("Install"));
      m_btnUpdate->setEnabled(true);
      break;

    default:
      m_ui.m_lblStatus->setStatus(WidgetWithStatus::StatusType::Error, tr("Error occured"),
                                  tr("Error occured during downloading of the package."));
      m_btnUpdate->setText(tr("Error occured"));
      break;
  }
}

void FormUpdate::startUpdate() {
  QString url_file;
  const bool update_for_this_system = isSelfUpdateSupported();

  if (update_for_this_system && m_ui.m_listFiles->currentItem() != nullptr) {
    url_file = m_ui.m_listFiles->currentItem()->data(Qt::UserRole).toString();
    m_ui.m_listFiles->setEnabled(false);
  }
  else {
    url_file = APP_URL;
  }

  if (m_readyToInstall) {
    close();
    qDebug().noquote().nospace() << QSL("Preparing to launch external installer '")
                                 << QDir::toNativeSeparators(m_updateFilePath)
                                 << QSL("'.");
#if defined(Q_OS_WIN)
    if (m_updateFilePath.endsWith(QL1S("exe"))) {
      HINSTANCE exec_result = ShellExecute(nullptr,
                                           nullptr,
                                           reinterpret_cast<const WCHAR*>(QDir::toNativeSeparators(m_updateFilePath).utf16()),
                                           nullptr,
                                           nullptr,
                                           SW_NORMAL);

      if (exec_result <= HINSTANCE(32)) {
        qDebug().noquote() << QSL("External updater was not launched due to error.");
        QMessageBox::critical(this, tr("Cannot Start Installer"), tr("Cannot launch external updater. Update application manually."));
      }
      else {
        qApp->quitApplication();
      }
    }
    else {
      if (!qApp->system()->openFolderFile(m_updateFilePath)) {
        qDebug().noquote() << QSL("External updater was not launched due to error.");
        QMessageBox::critical(this, tr("Cannot Open Update File"), tr("Cannot open application update file. Update application manually."));
      }
      else {
        qApp->quitApplication();
      }
    }
#endif
  }
  else if (update_for_this_system) {
    updateProgress(0, 100);
    m_btnUpdate->setText(tr("Downloading update..."));
    m_btnUpdate->setEnabled(false);
    m_downloader.downloadFile(url_file);
  }
  else {
    // Self-update and package are not available.
    if (!qApp->web()->openUrlInExternalBrowser(url_file)) {
      qApp->showGuiMessage(tr("Cannot navigate to installation file. Check new installation downloads manually on project website."),
                           QMessageBox::Icon::Warning);
    }
  }
}
