// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef NETWORKFACTORY_H
#define NETWORKFACTORY_H

#include <QCoreApplication>
#include <QNetworkReply>
#include <QPair>
#include <QVariant>

typedef QPair<QNetworkReply::NetworkError, QVariant> NetworkResult;

class Downloader;

class NetworkFactory {
  Q_DECLARE_TR_FUNCTIONS(NetworkFactory)

  private:
    explicit NetworkFactory();

  public:

    // Returns human readable text for given network error.
    static QString networkErrorText(QNetworkReply::NetworkError error_code);
    static Downloader* performAsyncNetworkOperation(const QString& url,
                                                    int timeout,
                                                    const QByteArray& input_data,
                                                    QNetworkAccessManager::Operation operation,
                                                    QList<QPair<QByteArray,
                                                                QByteArray>> additional_headers = QList<QPair<QByteArray, QByteArray>>(),
                                                    bool protected_contents = false,
                                                    const QString& username = QString(),
                                                    const QString& password = QString());
    static NetworkResult performNetworkOperation(const QString& url, int timeout,
                                                 const QByteArray& input_data,
                                                 QByteArray& output,
                                                 QNetworkAccessManager::Operation operation,
                                                 QList<QPair<QByteArray,
                                                             QByteArray>> additional_headers = QList<QPair<QByteArray, QByteArray>>(),
                                                 bool protected_contents = false,
                                                 const QString& username = QString(),
                                                 const QString& password = QString());
};

#endif // NETWORKFACTORY_H
