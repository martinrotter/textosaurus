// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FILESYSTEMSIDEBAR_H
#define FILESYSTEMSIDEBAR_H

#include "saurus/gui/sidebars/basesidebar.h"

class TextApplication;
class BaseTextEdit;
class FilesystemModel;
class FilesystemView;
class FavoritesListWidget;
class QTabWidget;
class QComboBox;

class FilesystemSidebar : public BaseSidebar {
  Q_OBJECT

  public:
    explicit FilesystemSidebar(TextApplication* text_app, QWidget* parent = nullptr);
    virtual ~FilesystemSidebar() = default;

    virtual Qt::DockWidgetArea initialArea() const override;
    virtual bool initiallyVisible() const override;
    virtual int initialWidth() const override;

  private slots:
    void openDrive(int index);
    void openFavoriteItem(const QModelIndex& idx);
    void openFileFolder(const QModelIndex& idx);
    void openFolder(const QString& path);
    void openFolder(const QModelIndex& idx);

    void goToParentFolder();
    void saveCurrentFolder(const QModelIndex& idx);

    void addToFavorites();
    void saveFavorites() const;

  signals:
    void openFileRequested(const QString& file_path);

  private:
    void makeExplorerVisible() const;
    void makeFavoritesVisible() const;
    virtual void load() override;

  private:
    const QString m_settingsSection = "filesystem";
    BaseTextEdit* m_txtPath;
    QComboBox* m_cmbDrives;
    FilesystemModel* m_fsModel;
    FilesystemView* m_fsView;
    FavoritesListWidget* m_lvFavorites;
    QTabWidget* m_tabWidget;
    QComboBox* m_cmbFilters;
};

#endif // FILESYSTEMSIDEBAR_H
