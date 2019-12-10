// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef CHARACTERMAP_H
#define CHARACTERMAP_H

#include <QWidget>

struct CharacterInfo {
  QChar m_character;
  QString m_description;

  CharacterInfo(QChar chr, const QString& description) : m_character(chr), m_description(description) {}

};

class CharacterMap : public QWidget {
  Q_OBJECT

  public:
    explicit CharacterMap(QWidget* parent = nullptr);
    virtual ~CharacterMap() = default;

    QSize sizeHint() const override;

  signals:
    void characterSelected(const QChar& character);

  public slots:
    void loadCharacters(const QList<CharacterInfo>& list);

  protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

  private:
    bool isSelectedValidCharacter() const;
    void calculateSquareSize();

    // Returns index of character based on point.
    int indexFromPoint(const QPoint& pt) const;

    int m_columns;
    int m_squareSize;
    QFont m_font;

    QList<CharacterInfo> m_characters;
    int m_selectedCharacter;
};

#endif // CHARACTERMAP_H
