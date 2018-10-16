// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/network-web/webfactory.h"

#include "saurus/miscellaneous/application.h"

#include <QDesktopServices>
#include <QProcess>
#include <QUrl>

WebFactory::WebFactory(QObject* parent) : QObject(parent) {}

bool WebFactory::openUrlInExternalBrowser(const QString& url) const {
  if (qApp->settings()->value(GROUP(Browser), SETTING(Browser::CustomExternalBrowserEnabled)).toBool()) {
    const QString browser = qApp->settings()->value(GROUP(Browser), SETTING(Browser::CustomExternalBrowserExecutable)).toString();
    const QString arguments = qApp->settings()->value(GROUP(Browser), SETTING(Browser::CustomExternalBrowserArguments)).toString();
    const QString call_line = "\"" + browser + "\" \"" + arguments.arg(url) + "\"";

    qDebug().noquote().nospace() << QSL("Running command '") << call_line << QSL("'.");
    const bool result = QProcess::startDetached(call_line);

    if (!result) {
      qDebug() << QSL("External web browser call failed.");
    }

    return result;
  }
  else {
    return QDesktopServices::openUrl(url);
  }
}
