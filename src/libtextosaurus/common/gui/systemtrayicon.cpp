// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "common/gui/systemtrayicon.h"

#include "common/miscellaneous/settings.h"
#include "definitions/definitions.h"
#include "saurus/miscellaneous/application.h"

#include <QPainter>
#include <QTimer>

#if defined(Q_OS_WIN)
TrayIconMenu::TrayIconMenu(const QString& title, QWidget* parent) : QMenu(title, parent) {}

bool TrayIconMenu::event(QEvent* event) {
  if (event->type() == QEvent::Show && Application::activeModalWidget() != nullptr) {
    QTimer::singleShot(0, this, SLOT(hide()));
    qApp->showGuiMessage(tr("Close opened modal dialogs first."), QMessageBox::Icon::Warning);
  }

  return QMenu::event(event);
}

#endif

SystemTrayIcon::SystemTrayIcon(const QString& normal_icon, QMenu* menu,
                               std::function<void()> visibility_switcher, QWidget* parent)
  : QSystemTrayIcon(parent), m_normalIcon(normal_icon), m_visibilitySwitcher(std::move(visibility_switcher)),
  m_messageTarget(nullptr) {
  qDebug() << "Creating SystemTrayIcon instance.";

  QSystemTrayIcon::setIcon(m_normalIcon);

  if (menu != nullptr) {
    menu->setParent(parent);
    setContextMenu(menu);
  }

  // Establish new connection for bubble click.
  m_connection = connect(this, &SystemTrayIcon::messageClicked, [this]() {
    if (m_messageTarget.data() != nullptr) {
      QMetaObject::invokeMethod(m_messageTarget.data(), "requestVisibility");
    }
  });

  // Create necessary connections.
  connect(this, &SystemTrayIcon::activated, this, &SystemTrayIcon::onActivated);
}

SystemTrayIcon::~SystemTrayIcon() {
  qDebug() << "Destroying SystemTrayIcon instance.";
  hide();
}

void SystemTrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason) {
  switch (reason) {
    case SystemTrayIcon::Trigger:
    case SystemTrayIcon::DoubleClick:
    case SystemTrayIcon::MiddleClick:
      m_visibilitySwitcher();
      break;

    default:
      break;
  }
}

bool SystemTrayIcon::isSystemTrayAvailable() {
  return QSystemTrayIcon::isSystemTrayAvailable() && QSystemTrayIcon::supportsMessages();
}

bool SystemTrayIcon::isSystemTrayActivated() {
  return SystemTrayIcon::isSystemTrayAvailable() && qApp->settings()->value(GROUP(GUI), SETTING(GUI::UseTrayIcon)).toBool();
}

bool SystemTrayIcon::areNotificationsEnabled() {
  return qApp->settings()->value(GROUP(GUI), SETTING(GUI::EnableNotifications)).toBool();
}

void SystemTrayIcon::showPrivate() {
  // Make sure that application does not exit some window (for example
  // the settings window) gets closed. Behavior for main window
  // is handled explicitly by FormMain::closeEvent() method.
  qApp->setQuitOnLastWindowClosed(false);

  // Display the tray icon.
  QSystemTrayIcon::show();
  emit shown();

  qDebug() << "Tray icon displayed.";
}

void SystemTrayIcon::show() {
#if defined(Q_OS_WIN)

  // Show immediately.
  qDebug() << "Showing tray icon immediately.";
  showPrivate();
#else

  // Delay avoids race conditions and tray icon is properly displayed.
  qDebug() << "Showing tray icon with 1000 ms delay.";
  QTimer::singleShot(1000, this, SLOT(showPrivate()));
#endif
}

void SystemTrayIcon::showMessage(const QString& title, const QString& message, QSystemTrayIcon::MessageIcon icon,
                                 int milliseconds_timeout_hint, QObject* obj) {
  m_messageTarget.clear();

  if (obj != nullptr) {
    m_messageTarget = obj;
  }

  // NOTE: If connections do not work, then use QMetaObject::invokeMethod(...).
  QSystemTrayIcon::showMessage(title, message, icon, milliseconds_timeout_hint);
}
