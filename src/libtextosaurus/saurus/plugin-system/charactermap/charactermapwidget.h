// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef CHARACTERMAPWIDGET_H
#define CHARACTERMAPWIDGET_H

#include <QWidget>

class CharacterMap;
class QComboBox;
class QLineEdit;

class CharacterMapWidget : public QWidget {
  Q_OBJECT

  public:
    explicit CharacterMapWidget(QWidget* parent = nullptr);
    virtual ~CharacterMapWidget() = default;

    CharacterMap* map() const;

  private:
    void setupUi();

  private:
    CharacterMap* m_map;
    QComboBox* m_cmbPlane;
    QLineEdit* m_txtSearch;
};

#endif // CHARACTERMAPWIDGET_H
