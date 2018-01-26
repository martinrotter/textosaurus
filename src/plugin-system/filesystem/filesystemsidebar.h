// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FILESYSTEMSIDEBAR_H
#define FILESYSTEMSIDEBAR_H

#include "src/gui/sidebars/basesidebar.h"

#include <QFileSystemModel>
#include <QListWidget>

class FileSystemSidebarModel : public QFileSystemModel {
  Q_OBJECT

  public:
    explicit FileSystemSidebarModel(QObject* parent = nullptr);
    virtual ~FileSystemSidebarModel() = default;

    virtual QVariant data(const QModelIndex& index, int role) const override;
};

class FilesystemView : public QListView {
  Q_OBJECT

  public:
    explicit FilesystemView(QWidget* parent = nullptr);
    virtual ~FilesystemView() = default;

    virtual void setRootIndex(const QModelIndex& index) override;

  signals:
    void rootIndexChanged(const QModelIndex& index);
};

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

class TextApplication;
class BaseLineEdit;

class FilesystemSidebar : public BaseSidebar {
  Q_OBJECT

  public:
    explicit FilesystemSidebar(TextApplication* text_app, QWidget* parent = nullptr);
    virtual ~FilesystemSidebar() = default;

    virtual Qt::DockWidgetArea initialArea() const override;
    virtual bool initiallyVisible() const override;
    virtual int initialWidth() const override;

  private slots:
    void addToFavorites();
    void openFavoriteItem(const QModelIndex& idx);
    void openFileFolder(const QModelIndex& idx);
    void goToParentFolder();
    void saveCurrentFolder(const QModelIndex& idx);
    void saveFavorites() const;

  signals:
    void openFileRequested(const QString& file_path);

  private:
    virtual void load() override;

  private:
    BaseLineEdit* m_txtPath;
    FileSystemSidebarModel* m_fsModel;
    FilesystemView* m_fsView;
    FavoritesListWidget* m_lvFavorites;
};

#endif // FILESYSTEMSIDEBAR_H
