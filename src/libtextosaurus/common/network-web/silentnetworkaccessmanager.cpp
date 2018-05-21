// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/network-web/silentnetworkaccessmanager.h"

#include <QAuthenticator>
#include <QNetworkReply>

Q_GLOBAL_STATIC(SilentNetworkAccessManager, qz_silent_acmanager)

SilentNetworkAccessManager::SilentNetworkAccessManager(QObject* parent)
  : BaseNetworkAccessManager(parent) {
  connect(this, &SilentNetworkAccessManager::authenticationRequired,
          this, &SilentNetworkAccessManager::onAuthenticationRequired, Qt::DirectConnection);
}

SilentNetworkAccessManager* SilentNetworkAccessManager::instance() {
  return qz_silent_acmanager();
}

void SilentNetworkAccessManager::onAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator) {
  if (reply->property("protected").toBool()) {
    // This feed contains authentication information, it is good.
    authenticator->setUser(reply->property("username").toString());
    authenticator->setPassword(reply->property("password").toString());
    reply->setProperty("authentication-given", true);
    qDebug("Item '%s' requested authentication and got it.", qPrintable(reply->url().toString()));
  }
  else {
    reply->setProperty("authentication-given", false);

    // Authentication is required but this feed does not contain it.
    qWarning("Item '%s' requested authentication but username/password is not available.", qPrintable(reply->url().toString()));
  }
}
