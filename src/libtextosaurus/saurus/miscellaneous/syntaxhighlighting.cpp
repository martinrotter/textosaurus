// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/miscellaneous/syntaxhighlighting.h"

#include "definitions/definitions.h"

// *INDENT-OFF*
#include PATH(SCINTILLA_DIR,include/SciLexer.h)

// *INDENT-ON*

#include "common/exceptions/applicationexception.h"
#include "definitions/definitions.h"
#include "saurus/miscellaneous/application.h"

#include <QDir>
#include <QProcess>
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
  QProcess proc_file(this);
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  // Force English locale with UTF-8 arguments parsing.
  env.remove(QSL("LOCALE"));
  env.remove(QSL("LC_ALL"));
  env.remove(QSL("LC_CTYPE"));

  env.insert(QSL("LOCALE"), QSL("en_US.UTF-8"));
  env.insert(QSL("LC_ALL"), QSL("en_US.UTF-8"));
  env.insert(QSL("LC_CTYPE"), QSL("en_US.UTF-8"));

  proc_file.setProcessEnvironment(env);
  proc_file.start(QSL("file"), { QSL("-b"), QSL("-n"), QSL("--mime-type"), QDir::toNativeSeparators(file_name)});

  if (!proc_file.waitForStarted(50)) {
    qWarningNN <<
      QSL("File format recognition via 'file' utility failed, 'file' was not launched due to error '%1'").arg(proc_file.error());
    proc_file.kill();
  }
  else {
    if (!proc_file.waitForFinished(500)) {
      qWarningNN <<
        QSL("File format recognition via 'file' utility failed, the execution of 'file' took too long.").arg(proc_file.error());
      proc_file.kill();
    }
    else {
      QString output_o = proc_file.readAllStandardOutput(); output_o = output_o.remove('\n');
      QString error_o = proc_file.readAllStandardError();

      if (!error_o.isEmpty()) {
        qWarningNN << QSL("Utility 'file' reported some error via error output: %1").arg(error_o);
      }

      if (!output_o.isEmpty()) {
        try {
          auto lexer = lexerForMime(output_o);

          if (lexer.m_code != defaultLexer().m_code) {
            qDebugNN << QSL("Lexer '%2' for MIME type '%1' was found using utility 'file'.").arg(output_o, lexer.m_name);
            return lexer;
          }
        }
        catch (ApplicationException& ex) {
          // Not found.
          qWarningNN << QString("Failed to find lexer using 'file' utility: %1").arg(ex.message());
        }
      }
    }
  }

  QRegularExpression reg(QSL("(?:\\.?)(\\w+$)|($)"), QRegularExpression::PatternOption::CaseInsensitiveOption);
  QString suffix = reg.match(file_name).captured(1);

  try {
    return lexerForSuffix(suffix);
  }
  catch (ApplicationException& ex) {
    qWarningNN << QString("Failed to find lexer via filename suffix: %1").arg(ex.message());
    return defaultLexer();
  }
}

Lexer SyntaxHighlighting::lexerForSuffix(const QString& suffix) {
  for (const Lexer& lex : lexers()) {
    if (lex.m_suffices.contains(suffix)) {
      return lex;
    }
  }

  throw ApplicationException(QString("lexer for suffix '%1' was not found").arg(suffix));
}

Lexer SyntaxHighlighting::lexerForMime(const QString& mime_type) {
  for (const Lexer& lex : lexers()) {
    if (lex.m_mimeTypes.contains(mime_type)) {
      return lex;
    }
  }

  throw ApplicationException(QString("lexer for MIME type '%1' was not found").arg(mime_type));
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

Lexer SyntaxHighlighting::lexerForFilter(const QString& filter) {
  int idx_filter = fileFilters().indexOf(filter);

  // We do want to ignore "plain text" filter here.
  if (idx_filter >= 1) {
    return lexers().at(idx_filter);
  }
  else {
    throw ApplicationException(QSL("lexer for filter '%1' not found").arg(filter));
  }
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
    {SyntaxColorTheme::StyleComponents::Keyword2, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_CYAN), QColor(), false, true)},
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
    {SyntaxColorTheme::StyleComponents::Keyword2, SyntaxColorThemeComponent(QColor(SOLAR_CYAN), QColor(), false, true)},
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
    {SyntaxColorTheme::StyleComponents::Keyword2, SyntaxColorThemeComponent(QColor(MONOKAI_ORANGE))},
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
    {SyntaxColorTheme::StyleComponents::Keyword2, SyntaxColorThemeComponent(QColor(BASE16_L_KEYWORDS))},
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
    {SyntaxColorTheme::StyleComponents::Keyword2, SyntaxColorThemeComponent(QColor(TOMORROW_KEYWORDS))},
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

  // NOTE: Solarized Dark is the default color scheme.
  m_currentColorThemeIndex = 2;
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
      m_fileFilters << lex.m_name + tr(" (*") + lex.m_suffices.join(QL1S(" *")) + QL1C(')');
    }
  }

  return m_fileFilters;
}

QList<Lexer> SyntaxHighlighting::lexers() {
  if (m_lexers.isEmpty()) {
    m_lexers
      << Lexer(tr("Plain text"), QStringList {
      QString(), QSL("txt")
    }, SCLEX_NULL, {}, {QSL("text/plain")})
      << Lexer(QSL("A68k"), QStringList {
      QSL("doxygen")
    }, SCLEX_A68K, {}, {QSL("text/x-Algol68")})
      << Lexer(QSL("Ada"), QStringList {
      QSL("ada")
    }, SCLEX_ADA)
      << Lexer(QSL("ASN.1"), QStringList {
      QSL("asn1"), QSL("asn")
    }, SCLEX_ASN1)
      << Lexer(QSL("Assembly"), QStringList {
      QSL("asm"), QSL("nasm")
    }, SCLEX_ASM, {}, {QSL("text/x-asm")})
      << Lexer(QSL("AVS"), QStringList {
      QSL("avs")
    }, SCLEX_AVS)
      << Lexer(QSL("AWK"), QStringList {
      QSL("awk"), QSL("gawk"), QSL("nawk")
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
    }, {QSL("text/x-awk"), QSL("text/x-gawk"), QSL("text/x-nawk")}, {
      {0, "break case continue do else exit function for if in next return switch while "
          "@include delete nextfile BEGIN END "
          "ARGC ARGIND ARGV FILENAME FNR FS NF NR OFMT OFS ORS RLENGTH RS RSTART SUBSEP "
          "BINMODE CONVFMT FIELDWIDTHS FPAT IGNORECASE LINT TEXTDOMAiN ENVIRON ERRNO PROCINFO RT"}
    })
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
    }, {QSL("text/x-shellscript")})
      << Lexer(QSL("Batch"), QStringList {
      QSL("bat"), QSL("cmd")
    }, SCLEX_BATCH, {}, {QSL("text/x-msdos-batch")})
      << Lexer(QSL("BibTeX"), QStringList { QSL("bib") }, SCLEX_BIBTEX, {}, {QSL("BibTeX text file")}, {}, true)
      << Lexer(QSL("CMake"), QStringList {
      QSL("cmake"), QSL("txt")
    }, SCLEX_CMAKE, {}, {}, {}, true)
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
    }, {QSL("text/x-c++"), QSL("text/x-c")}, {
      {0, "alignas alignof and and_eq asm auto bitand bitor bool break case catch char "
          "char16_t char32_t class compl const constexpr const_cast continue decltype default "
          "delete do double dynamic_cast else enum explicit export extern false final float for "
          "friend goto if inline int long mutable namespace new noexcept not not_eq nullptr operator "
          "or or_eq override private protected public register reinterpret_cast return short signed "
          "sizeof static static_assert static_cast struct switch template this thread_local throw true "
          "try typedef typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq"}
    })
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
    }, {}, {
      {0, "color background-color background-image background-repeat background-attachment background-position background "
          "font-family font-style font-variant font-weight font-size font "
          "word-spacing letter-spacing text-decoration vertical-align text-transform text-align text-indent line-height "
          "margin-top margin-right margin-bottom margin-left margin "
          "padding-top padding-right padding-bottom padding-left padding "
          "border-top-width border-right-width border-bottom-width border-left-width border-width "
          "border-top border-right border-bottom border-left border "
          "border-color border-style width height float clear "
          "display white-space list-style-type list-style-image list-style-position list-style"}
    }, true)
      << Lexer(QSL("C#"), QStringList {
      QSL("cs")
    }, SCLEX_CPP)
      << Lexer(QSL("D"), QStringList {
      QSL("d")
    }, SCLEX_D)
      << Lexer(QSL("Diff"), QStringList {
      QSL("diff")
    }, SCLEX_DIFF, {
      {SCE_DIFF_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_DIFF_COMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_DIFF_COMMAND, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_DIFF_HEADER, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_DIFF_POSITION, SyntaxColorTheme::StyleComponents::Operator},
      {SCE_DIFF_DELETED, SyntaxColorTheme::StyleComponents::Error},
      {SCE_DIFF_ADDED, SyntaxColorTheme::StyleComponents::Number}
    }, {QSL("text/x-diff")})
      << Lexer(QSL("Fortran"), QStringList {
      QSL("for"), QSL("f90"), QSL("f95"), QSL("f2k"), QSL("f23")
    }, SCLEX_FORTRAN, {}, {QSL("text/x-fortran")})
      << Lexer(QSL("Fortran77"), QStringList {
      QSL("f77")
    }, SCLEX_F77)
      << Lexer(QSL("HTML"), QStringList {
      QSL("html"), QSL("htm"), QSL("shtml"), QSL("shtm"), QSL("xhtml"), QSL("xht"), QSL("hta")
    }, SCLEX_HTML, {
      {SCE_H_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_H_COMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_H_TAG, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_H_TAGEND, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_H_ATTRIBUTE, SyntaxColorTheme::StyleComponents::Keyword2},
      {SCE_H_XMLSTART, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_H_XMLEND, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_H_NUMBER, SyntaxColorTheme::StyleComponents::Number},
      {SCE_H_TAGUNKNOWN, SyntaxColorTheme::StyleComponents::Error},
      {SCE_H_ATTRIBUTEUNKNOWN, SyntaxColorTheme::StyleComponents::Error},
      {SCE_H_CDATA, SyntaxColorTheme::StyleComponents::PlainData},
      {SCE_H_DOUBLESTRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_H_SINGLESTRING, SyntaxColorTheme::StyleComponents::String}
    }, {QSL("text/html"), QSL("XHTML document text")}, {
      {0, "a abbr acronym address applet area b base basefont "
          "bdo bgsound big blink blockquote body br button caption center "
          "cite code col colgroup dd del dfn dir div dl dt em "
          "fieldset font form frame frameset h1 h2 h3 h4 h5 h6 "
          "head hr html i iframe img input ins isindex kbd label "
          "legend li link listing map marquee menu menuitem meta multicol nobr noembed noframes noscript "
          "object ol optgroup option p param plaintext pre q s samp "
          "script select small spacer span strike strong style sub sup "
          "table tbody td textarea tfoot th thead title tr tt u ul "
          "var xml xmlns xmp address article aside audio base bdi canvas command details datalist dialog embed "
          "figure figcaption footer header hgroup keygen main mark menu meter nav output "
          "picture progress ruby rb rt rtc rp section source template track time video wbr"}
    })
      << Lexer(QSL("Inno Setup"), QStringList {
      QSL("inno")
    }, SCLEX_INNOSETUP)
      << Lexer(QSL("Java"), QStringList {
      QSL("java")
    }, SCLEX_CPP, {}, {QSL("text/x-java")})
      << Lexer(QSL("JavaScript"), QStringList {
      QSL("js")
    }, SCLEX_CPP)
      << Lexer(QSL("JSON"), QStringList {
      QSL("json")
    }, SCLEX_JSON, {}, {}, {}, true)
      << Lexer(QSL("Lisp"), QStringList {
      QSL("lisp"), QSL("cl")
    }, SCLEX_LISP, {}, {QSL("text/x-lisp")})
      << Lexer(QSL("Lua"), QStringList {
      QSL("lua")
    }, SCLEX_LUA, {}, {QSL("text/x-lua")})
      << Lexer(QSL("Make"), QStringList {
      QSL("mak"), QSL("Makefile")
    }, SCLEX_MAKEFILE, {}, {QSL("text/x-makefile")})
      << Lexer(QSL("Markdown"), QStringList {
      QSL("markdown"), QSL("md")
    }, SCLEX_MARKDOWN, {
      {SCE_MARKDOWN_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_MARKDOWN_CODE, SyntaxColorTheme::StyleComponents::Script},
      {SCE_MARKDOWN_CODE2, SyntaxColorTheme::StyleComponents::Script},
      {SCE_MARKDOWN_CODEBK, SyntaxColorTheme::StyleComponents::Script},
      {SCE_MARKDOWN_EM1, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_MARKDOWN_EM2, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_MARKDOWN_STRONG1, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_MARKDOWN_STRONG2, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_MARKDOWN_STRIKEOUT, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_MARKDOWN_BLOCKQUOTE, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_MARKDOWN_HRULE, SyntaxColorTheme::StyleComponents::Keyword2},
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
      QSL("m"), QSL("mat")
    }, SCLEX_MATLAB, {}, {QSL("Matlab v5 mat-file")}, {}, true)
      << Lexer(QSL("NSIS"), QStringList {
      QSL("nsis"), QSL("ns")
    }, SCLEX_NSIS, {}, {}, {}, true)
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
    }, {QSL("text/x-pascal")}, {
      {0, "absolute abstract and array as asm assembler automated begin case "
          "cdecl class const constructor delayed deprecated destructor dispid dispinterface "
          "div do downto dynamic else end except experimental export exports external far "
          "file final finalization finally for forward function goto helper if "
          "implementation in inherited initialization inline interface is label library "
          "message mod near nil not object of on operator or out overload override packed "
          "pascal platform private procedure program property protected public published "
          "raise record reference register reintroduce repeat resourcestring safecall "
          "sealed set shl shr static stdcall strict string then threadvar to try type unit "
          "unsafe until uses var varargs virtual while winapi with xor "
          "add default implements index name nodefault read readonly remove stored write writeonly"}
    })
      << Lexer(QSL("Perl"), QStringList {
      QSL("pl"), QSL("pm"), QSL("plx")
    }, SCLEX_PERL, {}, {QSL("text/x-perl")})
      << Lexer(QSL("PO"), QStringList {
      QSL("po")
    }, SCLEX_PO)
      << Lexer(QSL("PHP"), QStringList {
      QSL("php")
    }, SCLEX_PHPSCRIPT, {
      {SCE_H_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_H_COMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_H_TAG, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_H_TAGEND, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_H_ATTRIBUTE, SyntaxColorTheme::StyleComponents::Keyword2},
      {SCE_H_XMLSTART, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_H_XMLEND, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_H_NUMBER, SyntaxColorTheme::StyleComponents::Number},
      {SCE_H_TAGUNKNOWN, SyntaxColorTheme::StyleComponents::Error},
      {SCE_H_ATTRIBUTEUNKNOWN, SyntaxColorTheme::StyleComponents::Error},
      {SCE_H_CDATA, SyntaxColorTheme::StyleComponents::PlainData},
      {SCE_H_DOUBLESTRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_H_SINGLESTRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_H_QUESTION, SyntaxColorTheme::StyleComponents::Preprocessor},

      // PHP-specific bindings.
      {SCE_HPHP_COMPLEX_VARIABLE, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_HPHP_SIMPLESTRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_HPHP_HSTRING, SyntaxColorTheme::StyleComponents::String},
      {SCE_HPHP_HSTRING_VARIABLE, SyntaxColorTheme::StyleComponents::String},
      {SCE_HPHP_NUMBER, SyntaxColorTheme::StyleComponents::Number},
      {SCE_HPHP_VARIABLE, SyntaxColorTheme::StyleComponents::Identifier},
      {SCE_HPHP_COMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_HPHP_COMMENTLINE, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_HPHP_OPERATOR, SyntaxColorTheme::StyleComponents::Operator},
      {SCE_HPHP_WORD, SyntaxColorTheme::StyleComponents::Keyword}
    }, {QSL("text/x-php"), QSL("PHP script")}, {
      {0, "a abbr acronym address applet area b base basefont "
          "bdo bgsound big blink blockquote body br button caption center "
          "cite code col colgroup dd del dfn dir div dl dt em "
          "fieldset font form frame frameset h1 h2 h3 h4 h5 h6 "
          "head hr html i iframe img input ins isindex kbd label "
          "legend li link listing map marquee menu menuitem meta multicol nobr noembed noframes noscript "
          "object ol optgroup option p param plaintext pre q s samp "
          "script select small spacer span strike strong style sub sup "
          "table tbody td textarea tfoot th thead title tr tt u ul "
          "var xml xmlns xmp"},
      {4, "__halt_compiler() abstract and array() as "
          "break callable case catch class clone const continue declare default "
          "die() do echo else elseif empty() enddeclare endfor endforeach endif "
          "endswitch endwhile eval() exit() extends final finally for foreach function "
          "global goto if implements include include_once instanceof "
          "insteadof interface isset() list() namespace new or print "
          "private protected public require require_once "
          "return static switch throw trait try unset() use var while xor yield "}
    })
      << Lexer(QSL("PostScript"), QStringList {
      QSL("ps")
    }, SCLEX_PS, {}, {QSL("application/postscript"), QSL("PostScript document")})
      << Lexer(QSL("POV"), QStringList {
      QSL("pov")
    }, SCLEX_POV)
      << Lexer(QSL("Powershell"), QStringList {
      QSL("ps1")
    }, SCLEX_POWERSHELL, {
      {SCE_POWERSHELL_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_POWERSHELL_KEYWORD, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_POWERSHELL_FUNCTION, SyntaxColorTheme::StyleComponents::Keyword2},
      {SCE_POWERSHELL_CMDLET, SyntaxColorTheme::StyleComponents::Keyword2},
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
    }, {}, {
      {0, "begin break catch continue data do dynamicparam else elseif end exit filter "
          "finally for foreach from function if in local param private process return "
          "switch throw trap try until where while"},
      {1, "add-computer add-content add-history add-member add-pssnapin add-type "
          "checkpoint-computer clear-content clear-eventlog clear-history clear-item "
          "clear-itemproperty clear-variable compare-object complete-transaction "
          "connect-wsman convert-path convertfrom-csv convertfrom-securestring "
          "convertfrom-stringdata convertto-csv convertto-html convertto-securestring "
          "convertto-xml copy-item copy-itemproperty debug-process disable-computerrestore "
          "disable-psbreakpoint disable-pssessionconfiguration disable-wsmancredssp "
          "disconnect-wsman enable-computerrestore enable-psbreakpoint enable-psremoting "
          "enable-pssessionconfiguration enable-wsmancredssp enter-pssession exit-pssession "
          "export-alias export-clixml export-console export-counter export-csv "
          "export-formatdata export-modulemember export-pssession foreach-object "
          "format-custom format-list format-table format-wide get-acl get-alias "
          "get-authenticodesignature get-childitem get-command get-computerrestorepoint "
          "get-content get-counter get-credential get-culture get-date get-event "
          "get-eventlog get-eventsubscriber get-executionpolicy get-formatdata get-help "
          "get-history get-host get-hotfix get-item get-itemproperty get-job get-location "
          "get-member get-module get-psbreakpoint get-pscallstack get-psdrive "
          "get-psprovider get-pssession get-pssessionconfiguration get-pssnapin "
          "get-pfxcertificate get-process get-random get-service get-tracesource "
          "get-transaction get-uiculture get-unique get-variable get-wsmancredssp "
          "get-wsmaninstance get-winevent get-wmiobject group-object import-alias "
          "import-clixml import-counter import-csv import-localizeddata import-module "
          "import-pssession invoke-command invoke-expression invoke-history invoke-item "
          "invoke-wsmanaction invoke-wmimethod join-path limit-eventlog measure-command "
          "measure-object move-item move-itemproperty new-alias new-event new-eventlog "
          "new-item new-itemproperty new-module new-modulemanifest new-object new-psdrive "
          "new-pssession new-pssessionoption new-service new-timespan new-variable "
          "new-wsmaninstance new-wsmansessionoption new-webserviceproxy out-default "
          "out-file out-gridview out-host out-null out-printer out-string pop-location "
          "push-location read-host receive-job register-engineevent register-objectevent "
          "register-pssessionconfiguration register-wmievent remove-computer remove-event "
          "remove-eventlog remove-item remove-itemproperty remove-job remove-module "
          "remove-psbreakpoint remove-psdrive remove-pssession remove-pssnapin "
          "remove-variable remove-wsmaninstance remove-wmiobject rename-item "
          "rename-itemproperty reset-computermachinepassword resolve-path restart-computer "
          "restart-service restore-computer resume-service select-object select-string "
          "select-xml send-mailmessage set-acl set-alias set-authenticodesignature "
          "set-content set-date set-executionpolicy set-item set-itemproperty set-location "
          "set-psbreakpoint set-psdebug set-pssessionconfiguration set-service "
          "set-strictmode set-tracesource set-variable set-wsmaninstance "
          "set-wsmanquickconfig set-wmiinstance show-eventlog sort-object split-path "
          "start-job start-process start-service start-sleep start-transaction "
          "start-transcript stop-computer stop-job stop-process stop-service "
          "stop-transcript suspend-service tee-object test-computersecurechannel "
          "test-connection test-modulemanifest test-path test-wsman trace-command "
          "undo-transaction unregister-event unregister-pssessionconfiguration "
          "update-formatdata update-list update-typedata use-transaction wait-event "
          "wait-job wait-process where-object write-debug write-error write-eventlog "
          "write-host write-output write-progress write-verbose write-warning"},
      {2, "ac asnp cat cd chdir clc clear clhy cli clp cls clv compare copy cp cpi cpp cvpa "
          "dbp del diff dir ebp echo epal epcsv epsn erase etsn exsn fc fl foreach ft fw "
          "gal gbp gc gci gcm gcs gdr ghy gi gjb gl gm gmo gp gps group gsn gsnp gsv gu gv "
          "gwmi h history icm iex ihy ii ipal ipcsv ipmo ipsn ise iwmi kill lp ls man md "
          "measure mi mount move mp mv nal ndr ni nmo nsn nv ogv oh popd ps pushd pwd r rbp "
          "rcjb rd rdr ren ri rjb rm rmdir rmo rni rnp rp rsn rsnp rv rvpa rwmi sajb sal "
          "saps sasv sbp sc select set si sl sleep sort sp spjb spps spsv start sv swmi tee "
          "type where wjb write"},
      {3, "clear-host disable-psremoting enable-psremoting get-verb help importsystemmodules "
          "mkdir more prompt psedit tabexpansion"}
    }, true)
      << Lexer(QSL("Properties/INI"), QStringList {
      QSL("properties"), QSL("ini"), QSL("conf"), QSL("theme")
    }, SCLEX_PROPERTIES, {}, {QSL("OS/2 INI")})
      << Lexer(QSL("Python"), QStringList {
      QSL("py"), QSL("pyw")
    }, SCLEX_PYTHON, {
      {SCE_P_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_P_WORD, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_P_WORD2, SyntaxColorTheme::StyleComponents::Keyword2},
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
    }, {QSL("text/x-python"), QSL("Python script text executable"), QSL("Blender3D BPython script")}, {
      {0, "and del from not while "
          "as elif global or with "
          "assert else if pass yield "
          "break except import print "
          "class exec in raise "
          "continue finally is return "
          "def for lambda try"}
    })
      << Lexer(QSL("R"), QStringList {
      QSL("r")
    }, SCLEX_R, {}, {}, {}, true)
      << Lexer(QSL("Registry"), QStringList {
      QSL("reg")
    }, SCLEX_REGISTRY)
      << Lexer(QSL("Ruby"), QStringList {
      QSL("rb"), QSL("rbw")
    }, SCLEX_RUBY, {}, {QSL("text/x-ruby")})
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
    })
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
    }, {}, {
      {0, "absolute action add admin after aggregate "
          "alias all allocate alter and any are array as asc "
          "assertion at authorization "
          "before begin binary bit blob body boolean both breadth by "
          "call cascade cascaded case cast catalog char character "
          "check class clob close collate collation column commit "
          "completion connect connection constraint constraints "
          "constructor continue corresponding create cross cube current "
          "current_date current_path current_role current_time current_timestamp "
          "current_user cursor cycle data date day deallocate dec decimal declare default "
          "deferrable deferred delete depth deref desc describe descriptor "
          "destroy destructor deterministic dictionary diagnostics disconnect "
          "distinct domain double drop dynamic "
          "each else end end-exec equals escape every except "
          "exception exec execute exists exit external "
          "false fetch first float for foreign found from free full "
          "function general get global go goto grant group grouping "
          "having host hour identity if ignore immediate in indicator initialize initially "
          "inner inout input insert int integer intersect interval "
          "into is isolation iterate join key "
          "language large last lateral leading left less level like "
          "limit local localtime localtimestamp locator "
          "map match merge minute modifies modify module month "
          "names national natural nchar nclob new next no none "
          "not null numeric object of off old on only open operation option "
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
          "treat trigger true under union unique unknown "
          "unnest update usage user using "
          "value values varchar variable varying view "
          "when whenever where with without work write year zone"}
    })
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
    }, SCLEX_TCL, {}, {QSL("text/x-tcl"), QSL("Tcl script")})
      << Lexer(QSL("TeX"), QStringList {
      QSL("tex"), QSL("latex"), QSL("sty"), QSL("cls")
    }, SCLEX_TEX, {}, {QSL("text/x-tex"), QSL("TeX document text")}, {}, true)
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
    }, {QSL("text/xml"), QSL("image/svg+xml"),
        QSL("application/xml-sitemap"), QSL("application/vnd.google-earth.kml+xml")})
      << Lexer(QSL("YAML"), QStringList {
      QSL("yml"), QSL("yaml")
    }, SCLEX_YAML);
  }

  return m_lexers;
}

Lexer SyntaxHighlighting::defaultLexer() {
  return lexers().first();
}

Lexer::Lexer() : m_name(QString()), m_code(SCLEX_NULL), m_mimeTypes(QStringList()) {}

Lexer::Lexer(QString name, QStringList suffices, int code,
             QMap<int, SyntaxColorTheme::StyleComponents> style_mappings, QStringList mime_types,
             QMap<int, QString> keywords, bool supports_old_style_folding)
  : m_name(std::move(name)), m_code(code), m_suffices(std::move(suffices)), m_keywords(std::move(keywords)),
  m_supportsOldStyleFolding(supports_old_style_folding),
  m_mimeTypes(mime_types), m_styleMappings(std::move(style_mappings)) {}

bool Lexer::isEmpty() const {
  return m_name.isEmpty() && m_suffices.isEmpty();
}
