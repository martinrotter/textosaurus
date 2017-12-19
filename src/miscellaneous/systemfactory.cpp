// For license of this file, see <project-root-folder>/LICENSE.md.

#include "miscellaneous/systemfactory.h"

#include "miscellaneous/application.h"
#include "miscellaneous/systemfactory.h"
#include "network-web/networkfactory.h"

#if defined(Q_OS_WIN)
#include <QSettings>
#endif

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QRegularExpression>
#include <QString>

using UpdateCheck = QPair<UpdateInfo, QNetworkReply::NetworkError>;

SystemFactory::SystemFactory(QObject* parent) : QObject(parent) {}

QRegularExpression SystemFactory::supportedUpdateFiles() {
#if defined(Q_OS_WIN)
  return QRegularExpression(QSL(".+win.+\\.(exe|7z)"));
#elif defined(Q_OS_MAC)
  return QRegularExpression(QSL(".dmg"));
#elif defined(Q_OS_LINUX)
  return QRegularExpression(QSL(".AppImage"));
#else
  return QRegularExpression(QSL(".*"));
#endif
}

#if defined(Q_OS_WIN)
bool SystemFactory::removeTrolltechJunkRegistryKeys() {
  if (qApp->settings()->value(GROUP(General), SETTING(General::RemoveTrolltechJunk)).toBool()) {
    QSettings registry_key(QSL("HKEY_CURRENT_USER\\Software\\TrollTech"), QSettings::NativeFormat);

    registry_key.remove(QSL(""));
    registry_key.sync();
    return registry_key.status() == QSettings::NoError;
  }
  else {
    return false;
  }
}

#endif

void SystemFactory::checkForUpdates() const {
  Downloader* downloader = new Downloader();

  connect(downloader, &Downloader::completed, this, [this, downloader]() {
    QPair<QList<UpdateInfo>, QNetworkReply::NetworkError> result;
    result.second = downloader->lastOutputError();

    if (result.second == QNetworkReply::NoError) {
      QByteArray obtained_data = downloader->lastOutputData();
      result.first = parseUpdatesFile(obtained_data);
    }
    else {
      result.first = QList<UpdateInfo>();
    }

    emit updatesChecked(result);
    downloader->deleteLater();
  });

  downloader->downloadFile(RELEASES_LIST);
}

bool SystemFactory::isVersionNewer(const QString& new_version, const QString& base_version) {
  QStringList base_version_tkn = base_version.split(QL1C('.'));
  QStringList new_version_tkn = new_version.split(QL1C('.'));

  while (!base_version_tkn.isEmpty() && !new_version_tkn.isEmpty()) {
    const int base_number = base_version_tkn.takeFirst().toInt();
    const int new_number = new_version_tkn.takeFirst().toInt();

    if (new_number > base_number) {
      // New version is indeed higher thatn current version.
      return true;
    }
    else if (new_number < base_number) {
      return false;
    }
  }

  // Versions are either the same or they have unequal sizes.
  if (base_version_tkn.isEmpty() && new_version_tkn.isEmpty()) {
    // Versions are the same.
    return false;
  }
  else {
    if (new_version_tkn.isEmpty()) {
      return false;
    }
    else {
      return new_version_tkn.join(QString()).toInt() > 0;
    }
  }
}

bool SystemFactory::openFolderFile(const QString& file_path) {
#if defined(Q_OS_WIN)
  return QProcess::startDetached(QString("explorer.exe /select, \"") + QDir::toNativeSeparators(file_path) + "\"");
#else
  const QString folder = QDir::toNativeSeparators(QFileInfo(file_path).absoluteDir().absolutePath());

  return QDesktopServices::openUrl(QUrl::fromLocalFile(folder));
#endif
}

QList<UpdateInfo> SystemFactory::parseUpdatesFile(const QByteArray& updates_file) const {
  QList<UpdateInfo> updates;
  QJsonArray document = QJsonDocument::fromJson(updates_file).array();

  for (int i = 0; i < document.size(); i++) {
    QJsonObject release = document.at(i).toObject();
    UpdateInfo update;

    update.m_date = QDateTime::fromString(release["published_at"].toString(), QSL("yyyy-MM-ddTHH:mm:ssZ"));
    update.m_availableVersion = release["tag_name"].toString();
    update.m_changes = release["body"].toString();
    QJsonArray assets = release["assets"].toArray();

    for (int j = 0; j < assets.size(); j++) {
      QJsonObject asset = assets.at(j).toObject();
      UpdateUrl url;

      url.m_fileUrl = asset["browser_download_url"].toString();
      url.m_name = asset["name"].toString();
      url.m_size = asset["size"].toVariant().toString() + tr(" bytes");
      update.m_urls.append(url);
    }

    updates.append(update);
  }

  std::sort(updates.begin(), updates.end(), [](const UpdateInfo& a, const UpdateInfo& b) -> bool {
    return a.m_date > b.m_date;
  });
  return updates;
}
