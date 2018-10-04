// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef WIDGETWITHSTATUS_H
#define WIDGETWITHSTATUS_H

#include <QIcon>
#include <QTimer>
#include <QWidget>

class PlainToolButton;
class QHBoxLayout;

class WidgetWithStatus : public QWidget {
  Q_OBJECT

  public:
    enum class StatusType {
      Information,
      Warning,
      Error,
      Ok,
      Progress
    };

    // Constructors and destructors.
    explicit WidgetWithStatus(QWidget* parent);
    virtual ~WidgetWithStatus() = default;

    // Sets custom status for this control.
    void setStatus(StatusType status, const QString& tooltip_text);
    StatusType status() const;

  protected:
    StatusType m_status;
    QTimer m_blinkTimer;
    QWidget* m_wdgInput;
    PlainToolButton* m_btnStatus;
    QHBoxLayout* m_layout;
    QIcon m_iconProgress;
    QIcon m_iconInformation;
    QIcon m_iconWarning;
    QIcon m_iconError;
    QIcon m_iconOk;
    QIcon m_iconVisible;
};

inline WidgetWithStatus::StatusType WidgetWithStatus::status() const {
  return m_status;
}

#endif // WIDGETWITHSTATUS_H
