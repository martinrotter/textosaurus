#ifndef FAVORITESLISTWIDGET_H
#define FAVORITESLISTWIDGET_H

#include <QListWidget>

class FavoritesListWidget : public QListWidget {
  Q_OBJECT

  public:
    explicit FavoritesListWidget(QWidget* parent = nullptr);
    virtual ~FavoritesListWidget() = default;

  public slots:
    void loadFileItem(const QString& file_path);

  protected:
    virtual void keyPressEvent(QKeyEvent* event) override;
};

#endif // FAVORITESLISTWIDGET_H
