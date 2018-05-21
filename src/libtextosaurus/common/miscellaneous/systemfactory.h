// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef SYSTEMFACTORY_H
#define SYSTEMFACTORY_H

#include <QObject>

#include "common/network-web/downloader.h"

#include <QHash>
#include <QMetaType>
#include <QNetworkReply>
#include <QPair>

struct UpdateUrl {
  QString m_fileUrl;
  QString m_name;
  QString m_size;
};
struct UpdateInfo {
  QString m_availableVersion;
  QString m_changes;
  QDateTime m_date;

  QList<UpdateUrl> m_urls;
};

Q_DECLARE_METATYPE(UpdateInfo)

class TEXTOSAURUS_DLLSPEC SystemFactory : public QObject {
  Q_OBJECT

  public:
    explicit SystemFactory(QObject* parent = nullptr);
    virtual ~SystemFactory() = default;

#if defined(Q_OS_WIN)
    bool removeTrolltechJunkRegistryKeys();
#endif

    // Tries to download list with new updates.
    void checkForUpdates();

    QRegularExpression supportedUpdateFiles();

    // Checks if update is newer than current application version.
    bool isVersionNewer(const QString& new_version, const QString& base_version);
    bool openFolderFile(const QString& file_path);

  signals:
    void updatesChecked(QPair<QList<UpdateInfo>, QNetworkReply::NetworkError> updates);

  private:
    QList<UpdateInfo> parseUpdatesFile(const QByteArray& updates_file) const;
};

#endif // SYSTEMFACTORY_H
