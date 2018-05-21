// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef WEBFACTORY_H
#define WEBFACTORY_H

#include <QObject>

#include <QMap>

class WebFactory : public QObject {
  Q_OBJECT

  public:
    explicit WebFactory(QObject* parent = nullptr);
    virtual ~WebFactory() = default;

  public slots:
    bool openUrlInExternalBrowser(const QString& url) const;

  private:
    QMap<QString, QString> m_escapes;
    QMap<QString, QString> m_deEscapes;
};

#endif // WEBFACTORY_H
