// For license of this file, see <project-root-folder>/LICENSE.md.

#include "common/network-web/webfactory.h"

#include "saurus/miscellaneous/application.h"

#include <QDesktopServices>
#include <QProcess>
#include <QUrl>

WebFactory::WebFactory(QObject* parent)
  : QObject(parent), m_escapes(QMap<QString, QString>()), m_deEscapes(QMap<QString, QString>()) {}

bool WebFactory::openUrlInExternalBrowser(const QString& url) const {
  if (qApp->settings()->value(GROUP(Browser), SETTING(Browser::CustomExternalBrowserEnabled)).toBool()) {
    const QString browser = qApp->settings()->value(GROUP(Browser), SETTING(Browser::CustomExternalBrowserExecutable)).toString();
    const QString arguments = qApp->settings()->value(GROUP(Browser), SETTING(Browser::CustomExternalBrowserArguments)).toString();
    const QString call_line = "\"" + browser + "\" \"" + arguments.arg(url) + "\"";

    qDebug("Running command '%s'.", qPrintable(call_line));
    const bool result = QProcess::startDetached(call_line);

    if (!result) {
      qDebug("External web browser call failed.");
    }

    return result;
  }
  else {
    return QDesktopServices::openUrl(url);
  }
}
