// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef SILENTNETWORKACCESSMANAGER_H
#define SILENTNETWORKACCESSMANAGER_H

#include "common/network-web/basenetworkaccessmanager.h"

#include <QPointer>

// Network manager used for more communication for feeds.
// This network manager does not provide any GUI interaction options.
class SilentNetworkAccessManager : public BaseNetworkAccessManager {
  Q_OBJECT

  public:
    explicit SilentNetworkAccessManager(QObject* parent = nullptr);
    virtual ~SilentNetworkAccessManager() = default;

    // Returns pointer to global silent network manager
    static SilentNetworkAccessManager* instance();

  public slots:
    void onAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator);
};

#endif // SILENTNETWORKACCESSMANAGER_H
