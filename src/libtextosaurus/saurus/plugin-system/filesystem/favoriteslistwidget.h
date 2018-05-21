// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef FAVORITESLISTWIDGET_H
#define FAVORITESLISTWIDGET_H

#include <QListWidget>

class FilesystemPlugin;

class FavoritesListWidget : public QListWidget {
  Q_OBJECT

  public:
    explicit FavoritesListWidget(FilesystemPlugin* plugin, QWidget* parent = nullptr);
    virtual ~FavoritesListWidget() = default;

  public slots:
    void loadFileItem(const QString& file_path);

  protected:
    virtual void keyPressEvent(QKeyEvent* event) override;

  private:
    FilesystemPlugin* m_plugin;
};

#endif // FAVORITESLISTWIDGET_H
