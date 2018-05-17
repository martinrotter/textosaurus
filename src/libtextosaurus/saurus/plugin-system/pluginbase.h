// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef PLUGINBASE_H
#define PLUGINBASE_H

#include <QObject>

class QAction;
class TextApplication;
class Settings;
class IconFactory;
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
    // fundemantals parts of main application
    // logic.
    //
    // NOTE: This method gets called when all passed compoents are initialized and
    // could be called later than methods name(), sidebars() and userActions().
    virtual void start(QWidget* main_form_widget, TextApplication* text_app, Settings* settings, IconFactory* icon_factory) = 0;

    // Stops the plugin.
    //
    // NOTE: Stopping of plugin must be done synchronously.
    // NOTE: Stopping and starting is done on main GUI thread.
    virtual void stop() = 0;
};

Q_DECLARE_INTERFACE(PluginBase, "io.github.martinrotter.textosaurus.pluginbase")

#endif // PLUGINBASE_H
