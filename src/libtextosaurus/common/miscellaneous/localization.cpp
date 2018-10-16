// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/miscellaneous/localization.h"

#include "saurus/miscellaneous/application.h"

#include <QDir>
#include <QFileInfoList>
#include <QLocale>
#include <QTranslator>

Localization::Localization(QObject* parent)
  : QObject(parent) {}

QString Localization::desiredLanguage() const {
  return qApp->settings()->value(GROUP(General), SETTING(General::Language)).toString();
}

void Localization::loadActiveLanguage() {
  auto* qt_translator = new QTranslator(qApp);
  auto* app_translator = new QTranslator(qApp);
  QString desired_localization = desiredLanguage();

  qDebug().nospace().noquote() << QSL("Starting to load active localization. Desired localization is '")
                               << desired_localization
                               << QSL("'.");

  if (app_translator->load(QLocale(desired_localization), "textosaurus", QSL("_"), APP_LANG_PATH)) {
    const QString real_loaded_locale = app_translator->translate("QObject", "LANG_ABBREV");

    Application::installTranslator(app_translator);
    qDebug().nospace().noquote() << QSL("Application localization '")
                                 << desired_localization
                                 << QSL("' loaded successfully, specifically sublocalization '")
                                 << real_loaded_locale
                                 << QSL("' was loaded.");
    desired_localization = real_loaded_locale;
  }
  else {
    qWarning().nospace().noquote() << QSL("Application localization '")
                                   << desired_localization
                                   << QSL("' was not loaded. Loading '")
                                   << DEFAULT_LOCALE
                                   << QSL("' instead.");
    desired_localization = DEFAULT_LOCALE;
  }

  if (qt_translator->load(QLocale(desired_localization), "qtbase", QSL("_"), APP_LANG_PATH)) {
    Application::installTranslator(qt_translator);
    qDebug().nospace().noquote() << QSL("Qt localization '")
                                 << desired_localization
                                 << QSL("' loaded successfully.");
  }
  else {
    qWarning().nospace().noquote() << QSL("Qt localization '")
                                   << desired_localization
                                   << QSL("' was NOT loaded.");
  }

  m_loadedLanguage = desired_localization;
  m_loadedLocale = QLocale(desired_localization);
  QLocale::setDefault(m_loadedLocale);
}

QList<Language> Localization::installedLanguages() const {
  QList<Language> languages;
  const QDir file_dir(APP_LANG_PATH);

  // Iterate all found language files.
  foreach (const QFileInfo& file, file_dir.entryInfoList(QStringList() << "textosaurus_*.qm", QDir::Files, QDir::Name)) {
    QTranslator translator;

    if (translator.load(file.absoluteFilePath())) {
      Language new_language;

      new_language.m_code = translator.translate("QObject", "LANG_ABBREV");
      new_language.m_name = QLocale(new_language.m_code).nativeLanguageName();
      languages << new_language;
    }
  }

  return languages;
}

QString Localization::loadedLanguage() const {
  return m_loadedLanguage;
}

QLocale Localization::loadedLocale() const {
  return m_loadedLocale;
}
