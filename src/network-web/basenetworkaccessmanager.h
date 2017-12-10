// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef BASENETWORKACCESSMANAGER_H
#define BASENETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>

// This is base class for all network access managers.
class BaseNetworkAccessManager : public QNetworkAccessManager {
  Q_OBJECT

  public:

    // Constructors and desctructors.
    explicit BaseNetworkAccessManager(QObject* parent = nullptr);
    virtual ~BaseNetworkAccessManager();

  public slots:
    virtual void loadSettings();

  protected slots:

    // Called when some SSL-related errors are detected.
    void onSslErrors(QNetworkReply* reply, const QList<QSslError>& error);

  protected:

    // Creates custom request.
    QNetworkReply* createRequest(Operation op, const QNetworkRequest& request, QIODevice* outgoingData);
};

#endif // BASENETWORKACCESSMANAGER_H
