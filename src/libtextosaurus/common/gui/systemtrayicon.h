// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>

#include "definitions/definitions.h"

#include <QMenu>
#include <QPixmap>
#include <QPointer>

#include <functional>

class FormMain;
class QEvent;

#if defined(Q_OS_WIN)

class TrayIconMenu : public QMenu {
  Q_OBJECT

  public:

    // Constructors and destructors.
    explicit TrayIconMenu(const QString& title, QWidget* parent);
    ~TrayIconMenu() = default;

  protected:
    virtual bool event(QEvent* event) override;
};

#endif

class TEXTOSAURUS_DLLSPEC SystemTrayIcon : public QSystemTrayIcon {
  Q_OBJECT

  public:
    explicit SystemTrayIcon(const QString& normal_icon, QMenu* menu,
                            std::function<void()> visibility_switcher, QWidget* parent = nullptr);
    virtual ~SystemTrayIcon();

    void showMessage(const QString& title, const QString& message, MessageIcon icon = Information,
                     int milliseconds_timeout_hint = TRAY_ICON_BUBBLE_TIMEOUT, QObject* obj = nullptr);

    // Returns true if tray icon CAN be constructed on this machine.
    static bool isSystemTrayAvailable();

    // Returns true if tray icon CAN be costructed and IS enabled in
    // application settings.
    static bool isSystemTrayActivated();

    // Determines whether balloon tips are enabled or not on tray icons.
    static bool areNotificationsEnabled();

  public slots:
    void show();

  private slots:
    void showPrivate();
    void onActivated(QSystemTrayIcon::ActivationReason reason);

  signals:
    void shown();

  private:
    QIcon m_normalIcon;

    std::function<void()> m_visibilitySwitcher;
    QMetaObject::Connection m_connection;
    QPointer<QObject> m_messageTarget;
};

#endif // SYSTEMTRAYICON_H
