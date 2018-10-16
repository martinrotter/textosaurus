// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef SYNTAXCOLORTHEMEEDITOR_H
#define SYNTAXCOLORTHEMEEDITOR_H

#include <QWidget>

#include "saurus/miscellaneous/syntaxcolortheme.h"

#include "ui_syntaxcolorthemeeditor.h"

class SyntaxColorThemeEditor : public QWidget {
  Q_OBJECT

  public:
    explicit SyntaxColorThemeEditor(QWidget* parent = nullptr);
    virtual ~SyntaxColorThemeEditor() = default;

    void loadColorThemes(const QList<SyntaxColorTheme>& themes, const QString& current_theme_name);

    QList<SyntaxColorTheme> colorThemes() const;
    SyntaxColorTheme& currentColorTheme();

    int currentColorThemeIndex() const;

  signals:
    void colorThemesEdited();

  private slots:
    void deleteSelectedTheme();
    void editForegroundColor();
    void clearForegroundColor();
    void editBackgroundColor();
    void clearBackgroundColor();
    void updateCurrentComponent();
    void copyExistingTheme();
    void onThemeSwitched(int row);
    void displayComponentDetails(int row);

  private:
    SyntaxColorThemeComponent generateNewComponent() const;
    SyntaxColorTheme::StyleComponents itemComponent(int row) const;

    void loadColorTheme(const SyntaxColorTheme& theme);

  private:
    Ui::SyntaxColorThemeEditor m_ui = {};

    QList<SyntaxColorTheme> m_colorThemes;
    QMap<SyntaxColorTheme::StyleComponents, int> m_indexMapping;
};

#endif // SYNTAXCOLORTHEMEEDITOR_H
