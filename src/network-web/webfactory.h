// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef WEBFACTORY_H
#define WEBFACTORY_H

#include <QObject>

#include <QMap>

class WebFactory : public QObject {
  Q_OBJECT

  public:
    explicit WebFactory(QObject* parent = nullptr);
    virtual ~WebFactory() = default;

    // Strips "<....>" (HTML, XML) tags from given text.
    QString stripTags(QString text);

    // HTML entity escaping.
    QString escapeHtml(const QString& html);
    QString deEscapeHtml(const QString& text);

    QString toSecondLevelDomain(const QUrl& url);

  public slots:
    bool openUrlInExternalBrowser(const QString& url) const;

  private:
    void generateEscapes();
    void generateDeescapes();

    QMap<QString, QString> m_escapes;
    QMap<QString, QString> m_deEscapes;
};

#endif // WEBFACTORY_H
