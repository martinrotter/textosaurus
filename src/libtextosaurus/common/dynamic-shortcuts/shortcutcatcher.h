// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef SHORTCUTCATCHER_H
#define SHORTCUTCATCHER_H

#include <QWidget>

class QHBoxLayout;
class QToolButton;
class QKeySequenceEdit;

class ShortcutCatcher : public QWidget {
  Q_OBJECT

  public:
    explicit ShortcutCatcher(QWidget* parent = nullptr);
    virtual ~ShortcutCatcher() = default;

    QKeySequence shortcut() const;
    void setDefaultShortcut(const QKeySequence& key);
    void setShortcut(const QKeySequence& key);

  public slots:
    void hideResetButton();
    void resetShortcut();
    void clearShortcut();

  signals:
    void shortcutChanged(QKeySequence seguence);

  private:
    QToolButton* m_btnReset;
    QToolButton* m_btnClear;
    QKeySequenceEdit* m_btnChange;
    QHBoxLayout* m_layout;
    QKeySequence m_defaultSequence;
};

#endif // KEYSEQUENCECATCHER_H
