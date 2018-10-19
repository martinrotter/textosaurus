// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/miscellaneous/syntaxhighlighting.h"

#include "3rd-party/scintilla/include/SciLexer.h"
#include "definitions/definitions.h"
#include "saurus/miscellaneous/application.h"

#include <QDir>
#include <QRegularExpression>
#include <QSettings>

SyntaxHighlighting::SyntaxHighlighting(QObject* parent)
  : QObject(parent), m_currentColorThemeIndex(0) {}

QStringList SyntaxHighlighting::bareFileFilters() {
  if (m_bareFileFilters.isEmpty()) {
    for (const Lexer& lex : lexers()) {
      for (const QString& suffix : lex.m_suffices) {
        m_bareFileFilters << QSL("*") + suffix;
      }
    }
  }

  return m_bareFileFilters;
}

Lexer SyntaxHighlighting::lexerForFile(const QString& file_name) {
  QRegularExpression reg(QSL("(?:\\.?)(\\w+$)|($)"), QRegularExpression::PatternOption::CaseInsensitiveOption);
  QString suffix = reg.match(file_name).captured(1);

  return lexerForSuffix(suffix);
}

Lexer SyntaxHighlighting::lexerForSuffix(const QString& suffix) {
  for (const Lexer& lex : lexers()) {
    if (lex.m_suffices.contains(suffix)) {
      return lex;
    }
  }

  // We return first lexer, which is lexer for plain text files.
  return m_lexers.first();
}

Lexer SyntaxHighlighting::lexerForName(const QString& name) {
  for (const Lexer& lex : lexers()) {
    if (lex.m_name == name) {
      return lex;
    }
  }

  // We return first lexer, which is lexer for plain text files.
  return m_lexers.first();
}

void SyntaxHighlighting::loadColorThemes() {
  // Add predefined themes.

  // Plain color scheme, no syntax highlighting.
  m_colorThemes.append(SyntaxColorTheme(QSL("Textosaurus"), true, QMap<SyntaxColorTheme::StyleComponents, SyntaxColorThemeComponent> {
    {SyntaxColorTheme::StyleComponents::ScintillaPaper, SyntaxColorThemeComponent(QColor(), QColor(255, 255, 255))},
    {SyntaxColorTheme::StyleComponents::ScintillaControlChar, SyntaxColorThemeComponent(QColor(0, 0, 0), QColor(255, 255, 255))},
    {SyntaxColorTheme::StyleComponents::ScintillaMargin, SyntaxColorThemeComponent(QColor(96, 96, 96), QColor(238, 238, 238))},
    {SyntaxColorTheme::StyleComponents::Default, SyntaxColorThemeComponent(QColor(0, 0, 0))},
  }));

  // Solarized Light - http://ethanschoonover.com/solarized
  m_colorThemes.append(SyntaxColorTheme(QSL("Solarized Light"), true, QMap<SyntaxColorTheme::StyleComponents, SyntaxColorThemeComponent> {
    {SyntaxColorTheme::StyleComponents::ScintillaPaper, SyntaxColorThemeComponent(QColor(), QColor(SOLAR_LIGHT_BASE_3))},
    {SyntaxColorTheme::StyleComponents::ScintillaControlChar, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_CONTROL),
                                                                                        QColor(SOLAR_LIGHT_BASE_3))},
    {SyntaxColorTheme::StyleComponents::ScintillaMargin,
     SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_MAGENTA), QColor(SOLAR_LIGHT_BASE_2))},
    {SyntaxColorTheme::StyleComponents::ScintillaCurrentLine,
     SyntaxColorThemeComponent(QColor(), QColor(SOLAR_LIGHT_BASE_2))},
    {SyntaxColorTheme::StyleComponents::ScintillaUrlHighlight, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_BLUE))},
    {SyntaxColorTheme::StyleComponents::ScintillaOccurrencesHighlight, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_BLUE))},
    {SyntaxColorTheme::StyleComponents::ScintillaEdge, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_ORANGE))},
    {SyntaxColorTheme::StyleComponents::Default, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_BASE_00))},
    {SyntaxColorTheme::StyleComponents::Keyword, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_BLUE), QColor(), false, true)},
    {SyntaxColorTheme::StyleComponents::Comment, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_MAGENTA), QColor(), true, true)},
    {SyntaxColorTheme::StyleComponents::Error, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_ORANGE))},
    {SyntaxColorTheme::StyleComponents::Operator, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_ORANGE))},
    {SyntaxColorTheme::StyleComponents::Number, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_ORANGE))},
    {SyntaxColorTheme::StyleComponents::PlainData, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_BASE_00))},
    {SyntaxColorTheme::StyleComponents::String, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_VIOLET), QColor(), true)}
  }));

  // Solarized Dark - http://ethanschoonover.com/solarized
  m_colorThemes.append(SyntaxColorTheme(QSL("Solarized Dark"), true, QMap<SyntaxColorTheme::StyleComponents, SyntaxColorThemeComponent> {
    {SyntaxColorTheme::StyleComponents::ScintillaPaper, SyntaxColorThemeComponent(QColor(), QColor(SOLAR_BASE_3))},
    {SyntaxColorTheme::StyleComponents::ScintillaControlChar, SyntaxColorThemeComponent(QColor(SOLAR_CONTROL), QColor(SOLAR_BASE_3))},
    {SyntaxColorTheme::StyleComponents::ScintillaMargin, SyntaxColorThemeComponent(QColor(SOLAR_BASE_01), QColor(SOLAR_BASE_2))},
    {SyntaxColorTheme::StyleComponents::ScintillaCurrentLine, SyntaxColorThemeComponent(QColor(), QColor(SOLAR_BASE_2))},
    {SyntaxColorTheme::StyleComponents::ScintillaUrlHighlight, SyntaxColorThemeComponent(QColor(SOLAR_GREEN))},
    {SyntaxColorTheme::StyleComponents::ScintillaOccurrencesHighlight, SyntaxColorThemeComponent(QColor(SOLAR_GREEN))},
    {SyntaxColorTheme::StyleComponents::ScintillaEdge, SyntaxColorThemeComponent(QColor(SOLAR_VIOLET))},
    {SyntaxColorTheme::StyleComponents::Default, SyntaxColorThemeComponent(QColor(SOLAR_BASE_00))},
    {SyntaxColorTheme::StyleComponents::Keyword, SyntaxColorThemeComponent(QColor(SOLAR_GREEN), QColor(), false, true)},
    {SyntaxColorTheme::StyleComponents::Comment, SyntaxColorThemeComponent(QColor(SOLAR_ORANGE), QColor(), true, true)},
    {SyntaxColorTheme::StyleComponents::Error, SyntaxColorThemeComponent(QColor(SOLAR_VIOLET))},
    {SyntaxColorTheme::StyleComponents::Operator, SyntaxColorThemeComponent(QColor(SOLAR_ORANGE))},
    {SyntaxColorTheme::StyleComponents::Number, SyntaxColorThemeComponent(QColor(SOLAR_ORANGE))},
    {SyntaxColorTheme::StyleComponents::String, SyntaxColorThemeComponent(QColor(SOLAR_RED), QColor(), true)}
  }));

  // Monokai Neo - https://github.com/skylerlee/monokai-neo-syntax
  m_colorThemes.append(SyntaxColorTheme(QSL("Monokai Neo"), true, QMap<SyntaxColorTheme::StyleComponents, SyntaxColorThemeComponent> {
    {SyntaxColorTheme::StyleComponents::ScintillaPaper, SyntaxColorThemeComponent(QColor(), QColor(MONOKAI_DARK))},
    {SyntaxColorTheme::StyleComponents::ScintillaControlChar, SyntaxColorThemeComponent(QColor(MONOKAI_DARK_L), QColor(MONOKAI_DARK))},
    {SyntaxColorTheme::StyleComponents::ScintillaMargin, SyntaxColorThemeComponent(QColor(MONOKAI_DARK_L), QColor(MONOKAI_DARK))},
    {SyntaxColorTheme::StyleComponents::ScintillaUrlHighlight, SyntaxColorThemeComponent(QColor(MONOKAI_GREEN))},
    {SyntaxColorTheme::StyleComponents::ScintillaOccurrencesHighlight, SyntaxColorThemeComponent(QColor(MONOKAI_YELLOW))},
    {SyntaxColorTheme::StyleComponents::ScintillaEdge, SyntaxColorThemeComponent(QColor(MONOKAI_RED))},
    {SyntaxColorTheme::StyleComponents::Default, SyntaxColorThemeComponent(QColor(MONOKAI_LIGHT))},
    {SyntaxColorTheme::StyleComponents::Keyword, SyntaxColorThemeComponent(QColor(MONOKAI_CYAN))},
    {SyntaxColorTheme::StyleComponents::Comment, SyntaxColorThemeComponent(QColor(MONOKAI_DARK_L))},
    {SyntaxColorTheme::StyleComponents::Error, SyntaxColorThemeComponent(QColor(MONOKAI_RED))},
    {SyntaxColorTheme::StyleComponents::Operator, SyntaxColorThemeComponent(QColor(MONOKAI_RED))},
    {SyntaxColorTheme::StyleComponents::Number, SyntaxColorThemeComponent(QColor(MONOKAI_PURPLE))},
    {SyntaxColorTheme::StyleComponents::String, SyntaxColorThemeComponent(QColor(MONOKAI_YELLOW))}
  }));

  // Base16 Light - http://chriskempson.com/projects/base16
  m_colorThemes.append(SyntaxColorTheme(QSL("Base16 Light"), true, QMap<SyntaxColorTheme::StyleComponents, SyntaxColorThemeComponent> {
    {SyntaxColorTheme::StyleComponents::ScintillaPaper, SyntaxColorThemeComponent(QColor(), QColor(BASE16_L_PAPER))},
    {SyntaxColorTheme::StyleComponents::ScintillaControlChar, SyntaxColorThemeComponent(QColor(BASE16_L_WHITESP), QColor(BASE16_L_PAPER))},
    {SyntaxColorTheme::StyleComponents::ScintillaMargin, SyntaxColorThemeComponent(QColor(BASE16_L_LIN_NUM), QColor(BASE16_L_PAPER))},
    {SyntaxColorTheme::StyleComponents::ScintillaUrlHighlight, SyntaxColorThemeComponent(QColor(BASE16_L_STRINGS))},
    {SyntaxColorTheme::StyleComponents::ScintillaOccurrencesHighlight, SyntaxColorThemeComponent(QColor(BASE16_L_OCCURR))},
    {SyntaxColorTheme::StyleComponents::ScintillaCurrentLine, SyntaxColorThemeComponent(QColor(), QColor(BASE16_L_CUR_LINE))},
    {SyntaxColorTheme::StyleComponents::Default, SyntaxColorThemeComponent(QColor(BASE16_L_TEXT))},
    {SyntaxColorTheme::StyleComponents::Keyword, SyntaxColorThemeComponent(QColor(BASE16_L_KEYWORDS))},
    {SyntaxColorTheme::StyleComponents::Comment, SyntaxColorThemeComponent(QColor(BASE16_L_COMMENT))},
    {SyntaxColorTheme::StyleComponents::Operator, SyntaxColorThemeComponent(QColor(BASE16_L_OPERATORS))},
    {SyntaxColorTheme::StyleComponents::Number, SyntaxColorThemeComponent(QColor(BASE16_L_NUMBERS))},
    {SyntaxColorTheme::StyleComponents::String, SyntaxColorThemeComponent(QColor(BASE16_L_STRINGS))},
    {SyntaxColorTheme::StyleComponents::Preprocessor, SyntaxColorThemeComponent(QColor(BASE16_L_PREPROC))}
  }));

  // Tomorrow - https://github.com/chriskempson/tomorrow-theme
  m_colorThemes.append(SyntaxColorTheme(QSL("Tomorrow"), true, QMap<SyntaxColorTheme::StyleComponents, SyntaxColorThemeComponent> {
    {SyntaxColorTheme::StyleComponents::ScintillaPaper, SyntaxColorThemeComponent(QColor(), QColor(TOMORROW_PAPER))},
    {SyntaxColorTheme::StyleComponents::ScintillaControlChar, SyntaxColorThemeComponent(QColor(TOMORROW_WHITESP), QColor(TOMORROW_PAPER))},
    {SyntaxColorTheme::StyleComponents::ScintillaMargin, SyntaxColorThemeComponent(QColor(TOMORROW_LIN_NUM), QColor(TOMORROW_PAPER))},
    {SyntaxColorTheme::StyleComponents::ScintillaUrlHighlight, SyntaxColorThemeComponent(QColor(TOMORROW_STRINGS))},
    {SyntaxColorTheme::StyleComponents::ScintillaOccurrencesHighlight, SyntaxColorThemeComponent(QColor(TOMORROW_OCCURR))},
    {SyntaxColorTheme::StyleComponents::ScintillaCurrentLine, SyntaxColorThemeComponent(QColor(), QColor(TOMORROW_CUR_LINE))},
    {SyntaxColorTheme::StyleComponents::Default, SyntaxColorThemeComponent(QColor(TOMORROW_TEXT))},
    {SyntaxColorTheme::StyleComponents::Keyword, SyntaxColorThemeComponent(QColor(TOMORROW_KEYWORDS))},
    {SyntaxColorTheme::StyleComponents::Comment, SyntaxColorThemeComponent(QColor(TOMORROW_COMMENT))},
    {SyntaxColorTheme::StyleComponents::Operator, SyntaxColorThemeComponent(QColor(TOMORROW_OPERATORS))},
    {SyntaxColorTheme::StyleComponents::Number, SyntaxColorThemeComponent(QColor(TOMORROW_NUMBERS))},
    {SyntaxColorTheme::StyleComponents::String, SyntaxColorThemeComponent(QColor(TOMORROW_STRINGS))},
    {SyntaxColorTheme::StyleComponents::Preprocessor, SyntaxColorThemeComponent(QColor(TOMORROW_PREPROC))}
  }));

  const QString current_theme_name = qApp->settings()->value(GROUP(Editor), Editor::ColorTheme).toString();
  QSettings settings(qApp->userDataFolder() + QDir::separator() + QSL("colors.ini"), QSettings::Format::IniFormat);

  QList<SyntaxColorTheme> custom_themes = SyntaxColorTheme::fromSettings(settings);

  m_colorThemes.append(custom_themes);

  for (int i = 0; i < m_colorThemes.size(); i++) {
    if (m_colorThemes.at(i).name()  == current_theme_name) {
      m_currentColorThemeIndex = i;
      return;
    }
  }

  m_currentColorThemeIndex = 0;
}

QList<SyntaxColorTheme> SyntaxHighlighting::colorThemes() {
  if (m_colorThemes.isEmpty()) {
    loadColorThemes();
  }

  return m_colorThemes;
}

SyntaxColorTheme SyntaxHighlighting::currentColorTheme() {
  return colorThemes().at(m_currentColorThemeIndex);
}

void SyntaxHighlighting::setCurrentColorTheme(const QString& theme_name) {
  qApp->settings()->setValue(GROUP(Editor), Editor::ColorTheme, theme_name);
}

void SyntaxHighlighting::saveColorThemes(const QList<SyntaxColorTheme>& themes, int curr_theme_index) {
  m_colorThemes = themes;
  m_currentColorThemeIndex = curr_theme_index;

  QSettings settings(qApp->userDataFolder() + QDir::separator() + QSL("colors.ini"), QSettings::Format::IniFormat);

  for (const SyntaxColorTheme& theme : m_colorThemes) {
    if (!theme.predefined()) {
      theme.toSettings(settings);
    }
  }

  setCurrentColorTheme(m_colorThemes.at(m_currentColorThemeIndex).name());
}

QStringList SyntaxHighlighting::fileFilters() {
  if (m_fileFilters.isEmpty()) {
    for (const Lexer& lex : lexers()) {
      m_fileFilters << lex.m_name + tr(" files (*") + lex.m_suffices.join(QL1S(" *")) + QL1C(')');
    }
  }

  return m_fileFilters;
}

QList<Lexer> SyntaxHighlighting::lexers() {
  if (m_lexers.isEmpty()) {
    m_lexers
      << Lexer(tr("Plain text"), QStringList {
      QString(), QSL("txt")
    }, SCLEX_NULL)
      << Lexer(QSL("A68k"), QStringList {
      QSL("doxygen")
    }, SCLEX_A68K)
      << Lexer(QSL("Ada"), QStringList {
      QSL("ada")
    }, SCLEX_ADA)
      << Lexer(QSL("ASN.1"), QStringList {
      QSL("asn1"), QSL("asn")
    }, SCLEX_ASN1)
      << Lexer(QSL("Assembly"), QStringList {
      QSL("asm"), QSL("nasm")
    }, SCLEX_ASM)
      << Lexer(QSL("AVS"), QStringList {
      QSL("avs")
    }, SCLEX_AVS)
      << Lexer(QSL("AWK"), QStringList {
      QSL("awk")
    }, SCLEX_CPP, {
      {SCE_C_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_C_COMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_C_COMMENTDOC, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_C_COMMENTDOCKEYWORD, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_C_COMMENTDOCKEYWORDERROR, SyntaxColorTheme::StyleComponents::Error},
      {SCE_C_COMMENTLINE, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_C_COMMENTLINEDOC, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_C_NUMBER, SyntaxColorTheme::StyleComponents::Number},
      {SCE_C_WORD, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_C_WORD2, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_C_STRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_C_STRINGEOL, SyntaxColorTheme::StyleComponents::String},
      {SCE_C_STRINGRAW, SyntaxColorTheme::StyleComponents::String},
      {SCE_C_CHARACTER, SyntaxColorTheme::StyleComponents::String},
      {SCE_C_PREPROCESSOR, SyntaxColorTheme::StyleComponents::Preprocessor},
      {SCE_C_OPERATOR, SyntaxColorTheme::StyleComponents::Operator},
      {SCE_C_IDENTIFIER, SyntaxColorTheme::StyleComponents::Identifier},
      {SCE_C_VERBATIM, SyntaxColorTheme::StyleComponents::PlainData},
      {SCE_C_REGEX, SyntaxColorTheme::StyleComponents::Regex},
      {SCE_C_GLOBALCLASS, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_C_PREPROCESSORCOMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_C_PREPROCESSORCOMMENTDOC, SyntaxColorTheme::StyleComponents::Keyword}
    }, "break case continue do else exit function for if in next return switch while "
       "@include delete nextfile BEGIN END"
       "ARGC ARGIND ARGV FILENAME FNR FS NF NR OFMT OFS ORS RLENGTH RS RSTART SUBSEP "
       "BINMODE CONVFMT FIELDWIDTHS FPAT IGNORECASE LINT TEXTDOMAiN ENVIRON ERRNO PROCINFO RT")
      << Lexer(QSL("Bash"), QStringList {
      QSL("bash"), QSL("sh"), QSL("zsh"), QSL("bashrc"), QSL("zshrc"), QSL("_profile"), QSL("PKGBUILD")
    }, SCLEX_BASH, {
      {SCE_SH_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_SH_CHARACTER, SyntaxColorTheme::StyleComponents::String},
      {SCE_SH_STRING, SyntaxColorTheme::StyleComponents::String},

      {SCE_SH_ERROR, SyntaxColorTheme::StyleComponents::Error},
      {SCE_SH_COMMENTLINE, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_SH_WORD, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_SH_NUMBER, SyntaxColorTheme::StyleComponents::Number},
      {SCE_SH_OPERATOR, SyntaxColorTheme::StyleComponents::Operator},
      {SCE_SH_IDENTIFIER, SyntaxColorTheme::StyleComponents::Identifier},
      {SCE_SH_BACKTICKS, SyntaxColorTheme::StyleComponents::Operator},
      {SCE_SH_PARAM, SyntaxColorTheme::StyleComponents::Identifier}
    })
      << Lexer(QSL("Batch"), QStringList {
      QSL("bat"), QSL("cmd")
    }, SCLEX_BATCH)
      << Lexer(QSL("BibTeX"), QStringList {
      QSL("bib")
    }, SCLEX_BIBTEX)
      << Lexer(QSL("CMake"), QStringList {
      QSL("cmake"), QSL("txt")
    }, SCLEX_CMAKE)
      << Lexer(QSL("CoffeeScript"), QStringList {
      QSL("litcoffee")
    }, SCLEX_COFFEESCRIPT)
      << Lexer(QSL("Config"), QStringList {
      QSL("conf")
    }, SCLEX_CONF)
      << Lexer(QSL("C++"),
             QStringList {
      QSL("h"), QSL("hxx"), QSL("hpp"), QSL("c"), QSL("cpp"), QSL("cc"), QSL(
        "cxx")
    }, SCLEX_CPP, {
      {SCE_C_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_C_COMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_C_COMMENTDOC, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_C_COMMENTDOCKEYWORD, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_C_COMMENTDOCKEYWORDERROR, SyntaxColorTheme::StyleComponents::Error},
      {SCE_C_COMMENTLINE, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_C_COMMENTLINEDOC, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_C_NUMBER, SyntaxColorTheme::StyleComponents::Number},
      {SCE_C_WORD, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_C_WORD2, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_C_STRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_C_STRINGEOL, SyntaxColorTheme::StyleComponents::String},
      {SCE_C_STRINGRAW, SyntaxColorTheme::StyleComponents::String},
      {SCE_C_CHARACTER, SyntaxColorTheme::StyleComponents::String},
      {SCE_C_PREPROCESSOR, SyntaxColorTheme::StyleComponents::Preprocessor},
      {SCE_C_OPERATOR, SyntaxColorTheme::StyleComponents::Operator},
      {SCE_C_IDENTIFIER, SyntaxColorTheme::StyleComponents::Identifier},
      {SCE_C_VERBATIM, SyntaxColorTheme::StyleComponents::PlainData},
      {SCE_C_REGEX, SyntaxColorTheme::StyleComponents::Regex},
      {SCE_C_GLOBALCLASS, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_C_PREPROCESSORCOMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_C_PREPROCESSORCOMMENTDOC, SyntaxColorTheme::StyleComponents::Keyword}
    }, "alignas alignof and and_eq asm auto bitand bitor bool break case catch char "
       "char16_t char32_t class compl const constexpr const_cast continue decltype default "
       "delete do double dynamic_cast else enum explicit export extern false final float for "
       "friend goto if inline int long mutable namespace new noexcept not not_eq nullptr operator "
       "or or_eq override private protected public register reinterpret_cast return short signed "
       "sizeof static static_assert static_cast struct switch template this thread_local throw true "
       "try typedef typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq")
      << Lexer(QSL("CSS"), QStringList {
      QSL("css")
    }, SCLEX_CSS, {
      {SCE_CSS_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_CSS_COMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_CSS_CLASS, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_CSS_ATTRIBUTE, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_CSS_IDENTIFIER, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_CSS_DOUBLESTRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_CSS_SINGLESTRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_CSS_OPERATOR, SyntaxColorTheme::StyleComponents::Operator}
    }, "color background-color background-image background-repeat background-attachment background-position background "
       "font-family font-style font-variant font-weight font-size font "
       "word-spacing letter-spacing text-decoration vertical-align text-transform text-align text-indent line-height "
       "margin-top margin-right margin-bottom margin-left margin "
       "padding-top padding-right padding-bottom padding-left padding "
       "border-top-width border-right-width border-bottom-width border-left-width border-width "
       "border-top border-right border-bottom border-left border "
       "border-color border-style width height float clear "
       "display white-space list-style-type list-style-image list-style-position list-style")
      << Lexer(QSL("C#"), QStringList {
      QSL("cs")
    }, SCLEX_CPP)
      << Lexer(QSL("D"), QStringList {
      QSL("d")
    }, SCLEX_D)
      << Lexer(QSL("Diff"), QStringList {
      QSL("diff")
    }, SCLEX_DIFF)
      << Lexer(QSL("Fortran"), QStringList {
      QSL("for"), QSL("f90"), QSL("f95"), QSL("f2k"), QSL("f23")
    }, SCLEX_FORTRAN)
      << Lexer(QSL("Fortran77"), QStringList {
      QSL("f77")
    }, SCLEX_F77)
      << Lexer(QSL("HTML"), QStringList {
      QSL("html"), QSL("htm"), QSL("shtml"), QSL("shtm"), QSL("xhtml"), QSL("xht"), QSL("hta")
    }, SCLEX_HTML)
      << Lexer(QSL("Inno Setup"), QStringList {
      QSL("inno")
    }, SCLEX_INNOSETUP)
      << Lexer(QSL("Java"), QStringList {
      QSL("java")
    }, SCLEX_CPP)
      << Lexer(QSL("JavaScript"), QStringList {
      QSL("js")
    }, SCLEX_CPP)
      << Lexer(QSL("JSON"), QStringList {
      QSL("json")
    }, SCLEX_JSON)
      << Lexer(QSL("Lisp"), QStringList {
      QSL("lisp"), QSL("cl")
    }, SCLEX_LISP)
      << Lexer(QSL("Lua"), QStringList {
      QSL("lua")
    }, SCLEX_LUA)
      << Lexer(QSL("Make"), QStringList {
      QSL("mak"), QSL("Makefile")
    }, SCLEX_MAKEFILE)
      << Lexer(QSL("Markdown"), QStringList {
      QSL("markdown"), QSL("md")
    }, SCLEX_MARKDOWN, {
      {SCE_MARKDOWN_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_MARKDOWN_CODE, SyntaxColorTheme::StyleComponents::Script},
      {SCE_MARKDOWN_CODE2, SyntaxColorTheme::StyleComponents::Script},
      {SCE_MARKDOWN_CODEBK, SyntaxColorTheme::StyleComponents::Script},
      {SCE_MARKDOWN_LINK, SyntaxColorTheme::StyleComponents::Operator},
      {SCE_MARKDOWN_ULIST_ITEM, SyntaxColorTheme::StyleComponents::Operator},
      {SCE_MARKDOWN_OLIST_ITEM, SyntaxColorTheme::StyleComponents::Operator},
      {SCE_MARKDOWN_HEADER1, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_MARKDOWN_HEADER2, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_MARKDOWN_HEADER3, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_MARKDOWN_HEADER4, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_MARKDOWN_HEADER5, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_MARKDOWN_HEADER6, SyntaxColorTheme::StyleComponents::Comment},
    })
      << Lexer(QSL("Matlab"), QStringList {
      QSL("m")
    }, SCLEX_MATLAB)
      << Lexer(QSL("NSIS"), QStringList {
      QSL("nsis")
    }, SCLEX_NSIS)
      << Lexer(QSL("Octave"), QStringList {
      QSL("m")
    }, SCLEX_OCTAVE)
      << Lexer(QSL("Pascal"), QStringList {
      QSL("pas"), QSL("pp"), QSL("p"), QSL("inc")
    }, SCLEX_PASCAL, {
      {SCE_PAS_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_PAS_WORD, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_PAS_NUMBER, SyntaxColorTheme::StyleComponents::Number},
      {SCE_PAS_HEXNUMBER, SyntaxColorTheme::StyleComponents::Number},
      {SCE_PAS_CHARACTER, SyntaxColorTheme::StyleComponents::String},
      {SCE_PAS_STRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_PAS_STRINGEOL, SyntaxColorTheme::StyleComponents::String},
      {SCE_PAS_ASM, SyntaxColorTheme::StyleComponents::Preprocessor},
      {SCE_PAS_PREPROCESSOR, SyntaxColorTheme::StyleComponents::Preprocessor},
      {SCE_PAS_PREPROCESSOR2, SyntaxColorTheme::StyleComponents::Preprocessor},
      {SCE_PAS_COMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_PAS_COMMENT2, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_PAS_COMMENTLINE, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_PAS_IDENTIFIER, SyntaxColorTheme::StyleComponents::Identifier},
      {SCE_PAS_OPERATOR, SyntaxColorTheme::StyleComponents::Operator}
    }, "absolute abstract and array as asm assembler automated begin case "
       "cdecl class const constructor delayed deprecated destructor dispid dispinterface "
       "div do downto dynamic else end except experimental export exports external far "
       "file final finalization finally for forward function goto helper if "
       "implementation in inherited initialization inline interface is label library "
       "message mod near nil not object of on operator or out overload override packed "
       "pascal platform private procedure program property protected public published "
       "raise record reference register reintroduce repeat resourcestring safecall "
       "sealed set shl shr static stdcall strict string then threadvar to try type unit "
       "unsafe until uses var varargs virtual while winapi with xor "
       "add default implements index name nodefault read readonly remove stored write writeonly")
      << Lexer(QSL("Perl"), QStringList {
      QSL("pl"), QSL("pm"), QSL("plx")
    }, SCLEX_PERL)
      << Lexer(QSL("PO"), QStringList {
      QSL("po")
    }, SCLEX_PO)
      << Lexer(QSL("PostScript"), QStringList {
      QSL("ps")
    }, SCLEX_PS)
      << Lexer(QSL("POV"), QStringList {
      QSL("pov")
    }, SCLEX_POV)
      << Lexer(QSL("PowerShell"), QStringList {
      QSL("ps1")
    }, SCLEX_POWERSHELL, {
      {SCE_POWERSHELL_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_POWERSHELL_KEYWORD, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_POWERSHELL_NUMBER, SyntaxColorTheme::StyleComponents::Number},
      {SCE_POWERSHELL_CHARACTER, SyntaxColorTheme::StyleComponents::String},
      {SCE_POWERSHELL_STRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_POWERSHELL_HERE_STRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_POWERSHELL_ALIAS, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_POWERSHELL_COMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_POWERSHELL_COMMENTDOCKEYWORD, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_POWERSHELL_COMMENTSTREAM, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_POWERSHELL_IDENTIFIER, SyntaxColorTheme::StyleComponents::Identifier},
      {SCE_POWERSHELL_OPERATOR, SyntaxColorTheme::StyleComponents::Operator}
    }, "assembly exit process"
       "base filter public"
       "begin finally return"
       "break for sequence"
       "catch foreach static"
       "class from switch"
       "command function throw"
       "configuration hidden trap"
       "continue if try"
       "data in type"
       "define inlinescript until"
       "do interface using"
       "dynamicparam module  var"
       "else namespace while"
       "elseif parallel workflow"
       "end param"
       "enum private")
      << Lexer(QSL("Properties/INI"), QStringList {
      QSL("properties"), QSL("ini"), QSL("conf"), QSL("theme")
    }, SCLEX_PROPERTIES)
      << Lexer(QSL("Python"), QStringList {
      QSL("py"), QSL("pyw")
    }, SCLEX_PYTHON, {
      {SCE_P_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_P_WORD, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_P_NUMBER, SyntaxColorTheme::StyleComponents::Number},
      {SCE_P_CHARACTER, SyntaxColorTheme::StyleComponents::String},
      {SCE_P_STRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_P_STRINGEOL, SyntaxColorTheme::StyleComponents::String},
      {SCE_P_CLASSNAME, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_P_FCHARACTER, SyntaxColorTheme::StyleComponents::String},
      {SCE_P_FSTRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_P_COMMENTBLOCK, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_P_COMMENTLINE, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_P_IDENTIFIER, SyntaxColorTheme::StyleComponents::Identifier},
      {SCE_P_OPERATOR, SyntaxColorTheme::StyleComponents::Operator}
    }, "and del from not while "
       "as elif global or with "
       "assert else if pass yield "
       "break except import print "
       "class exec in raise "
       "continue finally is return "
       "def for lambda try")
      << Lexer(QSL("R"), QStringList {
      QSL("r")
    }, SCLEX_R)
      << Lexer(QSL("Registry"), QStringList {
      QSL("reg")
    }, SCLEX_REGISTRY)
      << Lexer(QSL("Ruby"), QStringList {
      QSL("rb"), QSL("rbw")
    }, SCLEX_RUBY)
      << Lexer(QSL("Rust"), QStringList {
      QSL("rs")
    }, SCLEX_RUST)
      << Lexer(QSL("SAS"), QStringList { QSL("sas") }, SCLEX_SAS, {
      {SCE_SAS_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_SAS_WORD, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_SAS_GLOBAL_MACRO, SyntaxColorTheme::StyleComponents::Preprocessor},
      {SCE_SAS_MACRO_FUNCTION, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_SAS_MACRO_KEYWORD, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_SAS_BLOCK_KEYWORD, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_SAS_TYPE, SyntaxColorTheme::StyleComponents::Keyword2},
      {SCE_SAS_NUMBER, SyntaxColorTheme::StyleComponents::Number},
      {SCE_SAS_STRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_SAS_COMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_SAS_COMMENTBLOCK, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_SAS_COMMENTLINE, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_SAS_IDENTIFIER, SyntaxColorTheme::StyleComponents::Identifier},
      {SCE_SAS_OPERATOR, SyntaxColorTheme::StyleComponents::Operator}
    }, "% %abort %bquote %cmpres %compstor %copy %datatyp %display %do %else %end %eval %global %goto "
       "%if %index %input %left %length %let %local %lowcase %macro %mend %nrbquote %nrquote %nrstr %put "
       "%qcmpres %qleft %qlowcase %qscan %qsubstr %qsysfunc %qtrim %quote %qupcase %return %scan %str "
       "%substr %superq %symdel %symexist %symglobl %symlocal %syscall %sysevalf %sysexec %sysfunc %sysget "
       "%syslput %sysprod %sysrc %sysrput %then %trim %unquote %until %upcase %verify %while %window & cmdmac "
       "implmac macro mautolocdisplay mautosource mcompilenote merror mfile mindelimiter mlogic mlogicnest "
       "mprint mprintnest mrecall mstored msymtabmax mvarsize nocenter option sasautos sasmstore serror symbolgen "
       "symdel symgetn symputn sysbuffr syscc syscharwidth syscmd sysdate sysdate9 sysday sysdevic sysdmg sysdsn "
       "sysenv syserr sysfilrc sysindex sysinfo sysjobid syslast syslckrc syslibrc sysmacroname sysmenv sysncpu "
       "syspbuff sysprocname sysscp sysscpl syssite sysstartid sysstartname systime sysuserid sysver sysvlong "
       "abs addr addrlong airy allperm anyalnum anyalpha anycntrl anydigit anyfirst anygraph anylower anyname "
       "anyprint anypunct anyspace anyupper anyxdigit appcort apply arcos armacov armalik armasim arsin atan "
       "atan2 attrc attrn band beta betainv block blshift bnot bor branks brshift btran bxor byte cat cats catt "
       "catx cdf ceil ceilz cexist change char choose choosec choosen cinv cnonct coalesce coalescec collate "
       "colpctn colpctsum comb compbl compcost compged complev comport compound compress concat constant "
       "convexit convx convxp cos cosh count countc covlag cshape css curobs cv cvexhull daccdb daccdbsl "
       "daccsl daccsyd dacctab dairy datdif date datejul datepart datetime day dclose dcreate depdb depdbsl "
       "depsl depsyd deptab dequote design designf det deviance dhms diag digamma dim dinfo dnum dopen "
       "doptname doptnum dread dropnote dsname dur duration durp echelon eigen eigval eigvec erf erfc "
       "eurocurr exist exp fact fappend farmacov farmafit farmalik farmasim fclose fcol fdelete fdif "
       "fetch fetchobs fexist fft fget fileexist fileref findc finfo finv fipname fipnamel fipstate "
       "floor floorz fnonct fnote fopen foptname foptnum forward fpoint fpos fput fread frewind frlen "
       "fsep fuzz fwrite gaend gagetmem gagetval gainit gaminv gamma gareeval garegen gasetcro gasetmut "
       "gasetobj gasetsel gasetup gblkvp gblkvpd gclose gdelete gdraw gdrawl geneig geomean geomeanz "
       "getoption getvarc getvarn ggrid ginclude ginv gopen gpie gpiexy gpoint gpoly gport gportpop "
       "gportstk gscale gscript gset gshow gsorth gstart gstop gstrlen gtext gvtext gwindow gxaxis "
       "gyaxis half hankel harmean harmeanz hbound hdir hermite hms homogen hour htmldecode htmlencode "
       "ibessel ifc ifft ifn indexc indexw inputc inputn insert int intck intnx intrr intz inv invupdt "
       "iorcmsg ipf iqr irr itbicg itcgrad itminres jbessel jroot juldate juldate7 kalcvf kalcvs kaldff "
       "kaldfs kcompare kcompress kcount kcvt kindex kindexc kleft klength klowcase kreverse kright kscan "
       "kstrcat ksubstr ksubstrb ktranslate ktrim ktruncate kupcase kupdate kupdateb kurtosis kverify lag "
       "largest lav lbound lcp lengthc lengthm lengthn lgamma libref lms loc log log10 log2 logbeta logcdf "
       "logpdf logsdf lowcase lts lupdt mad marg max maxqform mcd mdy mean median min minute mod module "
       "modulec modulei moduleic modulein modulen modz month mopen mort mve name ncol netpv nldate nldatm "
       "nleng nliteral nlpcg nlpdd nlpfdd nlpfea nlphqn nlplm nlpnms nlpnra nlpnrr nlpqn nlpqua nlptr "
       "nltime nmiss normal notalnum notalpha notcntrl notdigit note notfirst notgraph notlower notname "
       "notprint notpunct notspace notupper notxdigit npv nrow num nvalid ode open opscal ordinal orpol "
       "ortvec pagepctn pagepctsum pathname pctl pctn pctsum peek peekc peekclong peeklong perm pgraf "
       "point poisson poke pokelong polyroot probbeta probbnml probbnrm probchi probf probgam probhypr "
       "probmc probnegb probnorm probt product propcase prxchange prxdebug prxfree prxmatch prxnext "
       "prxparen prxparse prxposn prxsubstr ptrlongadd push put putc putn pv pvp qtr quad quantile "
       "queue quote ranbin rancau rand randgen randseed ranexp rangam range ranktie rannor ranperk "
       "ranperm ranpoi rantbl rantri ranuni rates ratio rdodt repeat reppctn reppctsum resolve reverse "
       "rewind rms root round rounde roundz rowcat rowcatc rowpctn rowpctsum rupdt rxchange rxfree "
       "rxmatch rxparse rxsubstr rzlind saving scan scanq sdf second seq seqscale seqshift setdif shape "
       "sign sin sinh skewness sleep smallest softmax solve solvelin sortndx sound soundex spedis spline "
       "splinec splinev spot sqrsym sqrt ssq std stderr stfips stname stnamel storage streaminit strip "
       "subpad substr substrn sum svd sweep symexist symget symglobl symlocal symput symputx symsqr sysget "
       "sysmsg sysparm sysprocessid sysprocessname sysprod sysrc system tan tanh teigen teigval teigvec "
       "time timepart tinv tnonct today toeplitz tpsplnev translate tranwrd trigamma trim trimn trisolv "
       "trunc tsbaysea tsdecomp tsmlocar tsmlomar tsmulmar tspears tspred tsroot tstvcar tsunimar type "
       "uniform union unique uniqueby upcase urldecode urlencode uss uuidgen valset value var varfmt "
       "varinfmt varlabel varlen varmacov varmalik varmasim varname varnum varray varrayx vartranscode "
       "vartype vecdiag vformat vformatd vformatdx vformatn vformatnx vformatw vformatwx vformatx "
       "vinarray vinarrayx vinformat vinformatd vinformatdx vinformatn vinformatnx vinformatw vinformatwx "
       "vinformatx vlabel vlabelx vlength vlengthx vname vnamex vnext vnormal vtranscode vtranscodex "
       "vtsroot vtype vtypex vvalue vvaluex wavft wavget wavift wavprint wavthrsh week weekday xmult "
       "xsect year yield yieldp yrdif yyq zipcity zipfips zipname zipnamel zipstate abort array as "
       "attrib by call cards cards4 catname chtml class close closefile continue csvall datalines "
       "datalines4 decimal_align declare delete describe dm do docbook edit else end endsas exclude "
       "execute file filename find finish first. footnote for force format free from go goto html "
       "html3 htmlcss id if imode include index infile informat input join keep label last. leave "
       "length libname link list listing load lock lostcard markup mattrib merge missing modify "
       "ods on order page pageby path pause pcl pdf phtml printer proclabel proctitle purge putlog "
       "read redirect remove rename reset results resume return rtf sasfile save select separated "
       "set setin setout show skip start stop store table then title to trace until use usegopt "
       "verify when where while window with wml")
      << Lexer(QSL("Spice"), QStringList {
      QSL("scp"), QSL("out")
    }, SCLEX_SPICE)
      << Lexer(QSL("SQL"), QStringList {
      QSL("sql")
    }, SCLEX_SQL, {
      {SCE_SQL_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_SQL_WORD, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_SQL_WORD2, SyntaxColorTheme::StyleComponents::Keyword2},
      {SCE_SQL_NUMBER, SyntaxColorTheme::StyleComponents::Number},
      {SCE_SQL_CHARACTER, SyntaxColorTheme::StyleComponents::String},
      {SCE_SQL_STRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_SQL_COMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_SQL_COMMENTDOC, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_SQL_COMMENTDOCKEYWORD, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_SQL_COMMENTDOCKEYWORDERROR, SyntaxColorTheme::StyleComponents::Error},
      {SCE_SQL_COMMENTLINEDOC, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_SQL_COMMENTLINE, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_SQL_IDENTIFIER, SyntaxColorTheme::StyleComponents::Identifier},
      {SCE_SQL_OPERATOR, SyntaxColorTheme::StyleComponents::Operator}
    }, "absolute action add admin after aggregate "
       "alias all allocate alter and any are array as asc "
       "assertion at authorization "
       "before begin binary bit blob body boolean both breadth by "
       "call cascade cascaded case cast catalog char character "
       "check class clob close collate collation column commit "
       "completion connect connection constraint constraints "
       "constructor continue corresponding create cross cube current "
       "current_date current_path current_role current_time current_timestamp "
       "current_user cursor cycle "
       "data date day deallocate dec decimal declare default "
       "deferrable deferred delete depth deref desc describe descriptor "
       "destroy destructor deterministic dictionary diagnostics disconnect "
       "distinct domain double drop dynamic "
       "each else end end-exec equals escape every except "
       "exception exec execute exists exit external "
       "false fetch first float for foreign found from free full "
       "function "
       "general get global go goto grant group grouping "
       "having host hour "
       "identity if ignore immediate in indicator initialize initially "
       "inner inout input insert int integer intersect interval "
       "into is isolation iterate "
       "join "
       "key "
       "language large last lateral leading left less level like "
       "limit local localtime localtimestamp locator "
       "map match merge minute modifies modify module month "
       "names national natural nchar nclob new next no none "
       "not null numeric "
       "object of off old on only open operation option "
       "or order ordinality out outer output "
       "package pad parameter parameters partial path postfix precision prefix "
       "preorder prepare preserve primary "
       "prior privileges procedure public "
       "read reads real recursive ref references referencing relative "
       "restrict result return returns revoke right "
       "role rollback rollup routine row rows "
       "savepoint schema scroll scope search second section select "
       "sequence session session_user set sets size smallint some| space "
       "specific specifictype sql sqlexception sqlstate sqlwarning start "
       "state statement static structure system_user "
       "table temporary terminate than then time timestamp "
       "timezone_hour timezone_minute to trailing transaction translation "
       "treat trigger true "
       "under union unique unknown "
       "unnest update usage user using "
       "value values varchar variable varying view "
       "when whenever where with without work write "
       "year "
       "zone")
      << Lexer(QSL("Stata"), QStringList { QSL("stata") }, SCLEX_STATA, {
      {SCE_STATA_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_STATA_WORD, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_STATA_GLOBAL_MACRO, SyntaxColorTheme::StyleComponents::Preprocessor},
      {SCE_STATA_MACRO, SyntaxColorTheme::StyleComponents::Preprocessor},
      {SCE_STATA_TYPE, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_STATA_NUMBER, SyntaxColorTheme::StyleComponents::Number},
      {SCE_STATA_STRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_STATA_COMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_STATA_COMMENTBLOCK, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_STATA_COMMENTLINE, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_STATA_IDENTIFIER, SyntaxColorTheme::StyleComponents::Identifier},
      {SCE_STATA_OPERATOR, SyntaxColorTheme::StyleComponents::Operator}
    })
      << Lexer(QSL("TCL"), QStringList {
      QSL("tcl")
    }, SCLEX_TCL)
      << Lexer(QSL("TeX"), QStringList {
      QSL("tex"), QSL("latex"), QSL("sty"), QSL("cls")
    }, SCLEX_TEX)
      << Lexer(QSL("VeriLog"), QStringList {
      QSL("v"), QSL("sv"), QSL("vh"), QSL("svh")
    }, SCLEX_VERILOG)
      << Lexer(QSL("VHDL"), QStringList {
      QSL("vhd"), QSL("vhdl")
    }, SCLEX_VHDL)
      << Lexer(QSL("Visual Basic"), QStringList {
      QSL("vb"),
    }, SCLEX_VB)
      << Lexer(QSL("XML"), {
      QSL("xml"), QSL("xaml"), QSL("xsl"), QSL("xslt"), QSL("xml"),
      QSL("xsd"), QSL("xul"), QSL("kml"), QSL("svg"), QSL("mxml"), QSL("xsml"),
      QSL("wsdl"), QSL("xlf"), QSL("xliff"), QSL("xbl"), QSL("xml"), QSL("sxbl"),
      QSL("sitemap"), QSL("gml"), QSL("gpx"), QSL("plist"), QSL("ts"), QSL("qrc"), QSL("vcxproj"),
      QSL("opml"), QSL("rss"), QSL("atom"), QSL("feed"), QSL("vbox"), QSL("vbox-prev")
    }, SCLEX_XML, {
      {SCE_H_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_H_TAG, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_H_TAGUNKNOWN, SyntaxColorTheme::StyleComponents::Error},
      {SCE_H_ATTRIBUTE, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_H_ATTRIBUTEUNKNOWN, SyntaxColorTheme::StyleComponents::Error},
      {SCE_H_NUMBER, SyntaxColorTheme::StyleComponents::Number},
      {SCE_H_DOUBLESTRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_H_SINGLESTRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_H_COMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_H_TAGEND, SyntaxColorTheme::StyleComponents::Identifier},
      {SCE_H_XMLSTART, SyntaxColorTheme::StyleComponents::Operator},
      {SCE_H_XMLEND, SyntaxColorTheme::StyleComponents::Operator},
      {SCE_H_SCRIPT, SyntaxColorTheme::StyleComponents::Script},
      {SCE_H_CDATA, SyntaxColorTheme::StyleComponents::PlainData}
    })
      << Lexer(QSL("YAML"), QStringList {
      QSL("yml"), QSL("yaml")
    }, SCLEX_YAML);
  }

  return m_lexers;
}

Lexer SyntaxHighlighting::defaultLexer() {
  return lexers().first();
}

Lexer::Lexer() : m_name(QString()), m_code(SCLEX_NULL) {}

Lexer::Lexer(const QString& name, const QStringList& suffices, int code,
             const QMap<int, SyntaxColorTheme::StyleComponents>& style_mappings,
             const QString& keywords)
  : m_name(name), m_code(code), m_suffices(suffices), m_keywords(keywords), m_styleMappings(style_mappings) {}

bool Lexer::isEmpty() const {
  return m_name.isEmpty() && m_suffices.isEmpty();
}
