// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/network-web/networkfactory.h"

#include "common/miscellaneous/settings.h"
#include "common/network-web/downloader.h"
#include "common/network-web/silentnetworkaccessmanager.h"
#include "definitions/definitions.h"

#include <QEventLoop>
#include <QIcon>
#include <QPixmap>
#include <QTextDocument>
#include <QTimer>

QString NetworkFactory::networkErrorText(QNetworkReply::NetworkError error_code) {
  switch (error_code) {
    case QNetworkReply::ProtocolUnknownError:
    case QNetworkReply::ProtocolFailure:

      //: Network status.
      return tr("protocol error");

    case QNetworkReply::ContentAccessDenied:
      return tr("access to content was denied");

    case QNetworkReply::HostNotFoundError:

      //: Network status.
      return tr("host not found");

    case QNetworkReply::OperationCanceledError:
    case QNetworkReply::TimeoutError:
      return tr("connection timed out or was cancelled");

    case QNetworkReply::RemoteHostClosedError:
    case QNetworkReply::ConnectionRefusedError:

      //: Network status.
      return tr("connection refused");

    case QNetworkReply::ProxyTimeoutError:

      //: Network status.
      return tr("connection timed out");

    case QNetworkReply::SslHandshakeFailedError:

      //: Network status.
      return tr("SSL handshake failed");

    case QNetworkReply::ProxyConnectionClosedError:
    case QNetworkReply::ProxyConnectionRefusedError:

      //: Network status.
      return tr("proxy server connection refused");

    case QNetworkReply::TemporaryNetworkFailureError:

      //: Network status.
      return tr("temporary failure");

    case QNetworkReply::AuthenticationRequiredError:

      //: Network status.
      return tr("authentication failed");

    case QNetworkReply::ProxyAuthenticationRequiredError:

      //: Network status.
      return tr("proxy authentication required");

    case QNetworkReply::ProxyNotFoundError:

      //: Network status.
      return tr("proxy server not found");

    case QNetworkReply::NoError:

      //: Network status.
      return tr("no errors");

    case QNetworkReply::UnknownContentError:

      //: Network status.
      return tr("unknown content");

    case QNetworkReply::ContentNotFoundError:

      //: Network status.
      return tr("content not found");

    case QNetworkReply::ProtocolInvalidOperationError:
      return tr("invalid operation, maybe input is too big");

    default:

      //: Network status.
      return tr("unknown error");
  }
}

Downloader* NetworkFactory::performAsyncNetworkOperation(const QString& url, int timeout, const QByteArray& input_data,
                                                         QNetworkAccessManager::Operation operation,
                                                         const QList<QPair<QByteArray, QByteArray>>& additional_headers,
                                                         bool protected_contents, const QString& username,
                                                         const QString& password) {
  auto* downloader = new Downloader();

  QObject::connect(downloader, &Downloader::completed, downloader, &Downloader::deleteLater);

  foreach (const auto& header, additional_headers) {
    if (!header.first.isEmpty()) {
      downloader->appendRawHeader(header.first, header.second);
    }
  }

  downloader->manipulateData(url, operation, input_data, timeout, protected_contents, username, password);
  return downloader;
}

NetworkResult NetworkFactory::performNetworkOperation(const QString& url, int timeout, const QByteArray& input_data,
                                                      QByteArray& output, QNetworkAccessManager::Operation operation,
                                                      const QList<QPair<QByteArray, QByteArray>>& additional_headers,
                                                      bool protected_contents,
                                                      const QString& username, const QString& password) {
  Downloader downloader;
  QEventLoop loop;
  NetworkResult result;

  // We need to quit event loop when the download finishes.
  QObject::connect(&downloader, &Downloader::completed, &loop, &QEventLoop::quit);

  foreach (const auto& header, additional_headers) {
    if (!header.first.isEmpty()) {
      downloader.appendRawHeader(header.first, header.second);
    }
  }

  downloader.manipulateData(url, operation, input_data, timeout, protected_contents, username, password);
  loop.exec();

  output = downloader.lastOutputData();
  result.first = downloader.lastOutputError();
  result.second = downloader.lastContentType();
  return result;
}
