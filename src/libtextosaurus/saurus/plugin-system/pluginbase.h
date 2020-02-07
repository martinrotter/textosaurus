// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef PLUGINBASE_H
#define PLUGINBASE_H

#include <QObject>

class QAction;
class TextApplication;
class Settings;
class IconFactory;
class WebFactory;
class BaseSidebar;

class PluginBase {
  public:
    explicit PluginBase() = default;
    virtual ~PluginBase() = default;

    // Returns localized name of plugin.
    virtual QString name() const = 0;
    virtual QString id() const = 0;

    // Returns list of sidebars.
    virtual QList<BaseSidebar*> sidebars() = 0;

    // Returns list of custom user actions.
    virtual QList<QAction*> userActions() = 0;

    // Gives plugin ability to access some
    // fundamentals parts of main application
    // logic.
    //
    // NOTE: This method gets called when all passed components are initialized and
    // could be called later than methods name(), sidebars() and userActions().
    virtual void start(const QString& library_file, QWidget* main_form_widget,
                       TextApplication* text_app, Settings* settings,
                       IconFactory* icon_factory, WebFactory* web_factory);

    // Stops the plugin.
    //
    // NOTE: Stopping of plugin must be done synchronously.
    // NOTE: Stopping and starting is done on main GUI thread.
    virtual void stop() = 0;

  public:
    QWidget* mainForm() const;
    TextApplication* textApp() const;
    Settings* settings() const;
    IconFactory* iconFactory() const;
    WebFactory* webFactory() const;

  private:
    QString m_libraryFile = QString();
    QWidget* m_mainForm = nullptr;
    TextApplication* m_textApp = nullptr;
    Settings* m_settings = nullptr;
    IconFactory* m_iconFactory = nullptr;
    WebFactory* m_webFactory = nullptr;
};

inline QWidget* PluginBase::mainForm() const {
  return m_mainForm;
}

inline TextApplication* PluginBase::textApp() const {
  return m_textApp;
}

inline Settings* PluginBase::settings() const {
  return m_settings;
}

inline IconFactory* PluginBase::iconFactory() const {
  return m_iconFactory;
}

inline WebFactory* PluginBase::webFactory() const {
  return m_webFactory;
}

inline void PluginBase::start(const QString& library_file, QWidget* main_form_widget,
                              TextApplication* text_app, Settings* settings,
                              IconFactory* icon_factory, WebFactory* web_factory) {
  m_libraryFile = library_file;
  m_mainForm = main_form_widget;
  m_textApp = text_app;
  m_settings = settings;
  m_iconFactory = icon_factory;
  m_webFactory = web_factory;
}

Q_DECLARE_INTERFACE(PluginBase, "io.github.martinrotter.textosaurus.pluginbase")

#endif // PLUGINBASE_H
