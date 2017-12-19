// For license of this file, see <project-root-folder>/LICENSE.md.

#include "network-web/webfactory.h"

#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"

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

QString WebFactory::stripTags(QString text) {
  return text.remove(QRegExp(QSL("<[^>]*>")));
}

QString WebFactory::escapeHtml(const QString& html) {
  if (m_escapes.isEmpty()) {
    generateEscapes();
  }

  QString output = html;

  QMapIterator<QString, QString> i(m_escapes);

  while (i.hasNext()) {
    i.next();
    output = output.replace(i.key(), i.value());
  }

  return output;
}

QString WebFactory::deEscapeHtml(const QString& text) {
  if (m_deEscapes.isEmpty()) {
    generateDeescapes();
  }

  QString output = text;

  QMapIterator<QString, QString> i(m_deEscapes);

  while (i.hasNext()) {
    i.next();
    output = output.replace(i.key(), i.value());
  }

  return output;
}

QString WebFactory::toSecondLevelDomain(const QUrl& url) {
  const QString top_level_domain = url.topLevelDomain();
  const QString url_host = url.host();

  if (top_level_domain.isEmpty() || url_host.isEmpty()) {
    return QString();
  }

  QString domain = url_host.left(url_host.size() - top_level_domain.size());

  if (domain.count(QL1C('.')) == 0) {
    return url_host;
  }

  while (domain.count(QL1C('.')) != 0) {
    domain = domain.mid(domain.indexOf(QL1C('.')) + 1);
  }

  return domain + top_level_domain;
}

void WebFactory::generateEscapes() {
  m_escapes[QSL("&lt;")] = QL1C('<');
  m_escapes[QSL("&gt;")] = QL1C('>');
  m_escapes[QSL("&amp;")] = QL1C('&');
  m_escapes[QSL("&quot;")] = QL1C('\"');
  m_escapes[QSL("&nbsp;")] = QL1C(' ');
  m_escapes[QSL("&plusmn;")] = QSL("±");
  m_escapes[QSL("&times;")] = QSL("×");
  m_escapes[QSL("&#039;")] = QL1C('\'');
}

void WebFactory::generateDeescapes() {
  m_deEscapes[QSL("<")] = QSL("&lt;");
  m_deEscapes[QSL(">")] = QSL("&gt;");
  m_deEscapes[QSL("&")] = QSL("&amp;");
  m_deEscapes[QSL("\"")] = QSL("&quot;");
  m_deEscapes[QSL("±")] = QSL("&plusmn;");
  m_deEscapes[QSL("×")] = QSL("&times;");
  m_deEscapes[QSL("\'")] = QSL("&#039;");
}
