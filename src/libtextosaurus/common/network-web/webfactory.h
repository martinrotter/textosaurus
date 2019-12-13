// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef WEBFACTORY_H
#define WEBFACTORY_H

#include <QObject>

#include <QMap>

class WebFactory : public QObject {
  Q_OBJECT

  public:
    explicit WebFactory(QObject* parent = nullptr);

  public slots:
    bool openUrlInExternalBrowser(const QString& url) const;
};

#endif // WEBFACTORY_H
