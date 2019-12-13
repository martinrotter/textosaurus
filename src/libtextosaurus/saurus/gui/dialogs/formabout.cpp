// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/dialogs/formabout.h"

#include "common/exceptions/applicationexception.h"
#include "common/gui/guiutilities.h"
#include "common/miscellaneous/cryptofactory.h"
#include "common/miscellaneous/iconfactory.h"
#include "common/miscellaneous/settingsproperties.h"
#include "common/miscellaneous/textfactory.h"
#include "definitions/definitions.h"
#include "saurus/miscellaneous/application.h"

#include <QFile>
#include <QFontDatabase>
#include <QTextStream>

FormAbout::FormAbout(QWidget* parent) : QDialog(parent) {
  m_ui.setupUi(this);
  m_ui.m_lblIcon->setPixmap(QPixmap(APP_ICON_PATH));
  GuiUtilities::applyDialogProperties(*this, qApp->icons()->fromTheme(QSL("help-about")), tr("About %1").arg(APP_NAME));
  loadLicenseAndInformation();
  loadSettingsAndPaths();
}

void FormAbout::loadSettingsAndPaths() {
  if (qApp->settings()->type() == SettingsType::Portable) {
    m_ui.m_txtPathsSettingsType->setText(tr("FULLY portable"));
  }
  else if (qApp->settings()->type() == SettingsType::NonPortable) {
    m_ui.m_txtPathsSettingsType->setText(tr("NOT portable"));
  }
  else {
    m_ui.m_txtPathsSettingsType->setText(tr("FORCED by user"));
  }

  m_ui.m_txtPathsSettingsFile->setText(QDir::toNativeSeparators(qApp->settings()->fileName()));
}

void FormAbout::loadLicenseAndInformation() {
  try {
    m_ui.m_txtLicenseGnu->setText(IOFactory::readFile(APP_INFO_PATH + QL1S("/COPYING_GNU_GPL_HTML")));
    m_ui.m_txtLicenseGnu->setFont(QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont));
  }
  catch (...) {
    m_ui.m_txtLicenseGnu->setText(tr("License not found."));
  }

  try {
    m_ui.m_txtLicenseGnuLgpl->setText(IOFactory::readFile(APP_INFO_PATH + QL1S("/COPYING_GNU_LGPL_HTML")));
    m_ui.m_txtLicenseGnuLgpl->setFont(QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont));
  }
  catch (...) {
    m_ui.m_txtLicenseGnuLgpl->setText(tr("License not found."));
  }

  try {
    m_ui.m_txtLicenseMd4c->setText(IOFactory::readFile(APP_INFO_PATH + QL1S("/COPYING_MD4C")));
    m_ui.m_txtLicenseMd4c->setFont(QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont));
  }
  catch (...) {
    m_ui.m_txtLicenseMd4c->setText(tr("License not found."));
  }

  try {
    m_ui.m_txtLicenseMpl->setText(IOFactory::readFile(APP_INFO_PATH + QL1S("/COPYING_MPL")));
    m_ui.m_txtLicenseMpl->setFont(QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont));
  }
  catch (...) {
    m_ui.m_txtLicenseMpl->setText(tr("License not found."));
  }

  try {
    m_ui.m_txtChangelog->setText(IOFactory::readFile(APP_INFO_PATH + QL1S("/CHANGELOG")));
    m_ui.m_txtChangelog->setFont(QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont));
  }
  catch (...) {
    m_ui.m_txtChangelog->setText(tr("Changelog not found."));
  }

  // Set other informative texts.
  m_ui.m_lblDesc->setText(tr("<b>%8</b><br/>"
                             "<b>Version:</b> %1 (built on %2/%3)<br/>"
                             "<b>Revision:</b> %4<br/>"
                             "<b>Build date:</b> %5<br/>"
                             "<b>Qt:</b> %6 (compiled against %7)<br/>"
                             "<b>OpenSSL:</b> %9").arg(
                          #if defined(FLATPAK_MODE)
                            qApp->applicationVersion() + QL1S(" (Flatpak) "),
                          #else
                            qApp->applicationVersion(),
                          #endif
                            APP_SYSTEM_NAME,
                            APP_SYSTEM_VERSION,
                            APP_REVISION,
                            TextFactory::parseDateTime(QString("%1 %2").arg(__DATE__,
                                                                            __TIME__)).toString(Qt::DefaultLocaleShortDate),
                            qVersion(),
                            QT_VERSION_STR,
                            APP_NAME,
                            CryptoFactory::openSslVersion()));

  m_ui.m_txtInfo->setText(tr("<body>%5 is simple cross-platform text editor based on Qt and Scintilla."
                             "<br><br>This software is distributed under the terms of GNU General Public License, version 3."
                             "<br><br>Contacts:"
                             "<ul><li><a href=\"mailto://%1\">%1</a> ~e-mail</li>"
                             "<li><a href=\"%2\">%2</a> ~website</li></ul>"
                             "You can obtain source code for %5 from its website."
                             "<br><br><br>Copyright (C) 2017-%3 %4</body>").arg(APP_EMAIL, APP_URL,
                                                                                QString::number(QDateTime::currentDateTime()
                                                                                                .date()
                                                                                                .year()),
                                                                                APP_AUTHOR, APP_NAME));
}
