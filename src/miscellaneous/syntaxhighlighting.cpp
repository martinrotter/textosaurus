// For license of this file, see <project-root-folder>/LICENSE.md.

#include "definitions/definitions.h"
#include "miscellaneous/syntaxhighlighting.h"

#include "3rd-party/scintilla/include/SciLexer.h"

#include <QRegularExpression>
#include <QSettings>

SyntaxHighlighting::SyntaxHighlighting(QObject* parent)
  : QObject(parent), m_bareFileFilters(QStringList()), m_fileFilters(QStringList()), m_lexers(QList<Lexer>()),
  m_colorThemes(QList<SyntaxColorTheme>()) {}

QStringList SyntaxHighlighting::bareFileFilters() {
  if (m_bareFileFilters.isEmpty()) {
    for (const Lexer& lex : lexers()) {
      for (const QString& suffix : lex.m_suffices) {
        if (!suffix.isEmpty()) {
          m_bareFileFilters << QSL("*") + suffix;
        }
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
  m_colorThemes.append(SyntaxColorTheme(QSL("Textilosaurus"), true, QMap<SyntaxColorTheme::StyleComponents, SyntaxColorThemeComponent> {
    {SyntaxColorTheme::StyleComponents::ScintillaPaper, SyntaxColorThemeComponent(QColor(), QColor(255, 255, 255))},
    {SyntaxColorTheme::StyleComponents::ScintillaControlChar, SyntaxColorThemeComponent(QColor(0, 0, 0), QColor(255, 255, 255))},
    {SyntaxColorTheme::StyleComponents::ScintillaMargin, SyntaxColorThemeComponent(QColor(96, 96, 96), QColor(238, 238, 238))},
    {SyntaxColorTheme::StyleComponents::Default, SyntaxColorThemeComponent(QColor(0, 0, 0))},
  }));

  // Solarized Light - http://ethanschoonover.com/solarized
  m_colorThemes.append(SyntaxColorTheme(QSL("Solarized Light"), true, QMap<SyntaxColorTheme::StyleComponents, SyntaxColorThemeComponent> {
    {SyntaxColorTheme::StyleComponents::ScintillaPaper, SyntaxColorThemeComponent(QColor(), QColor(SOLAR_LIGHT_BASE_3))},
    {SyntaxColorTheme::StyleComponents::ScintillaControlChar, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_CONTROL), QColor(SOLAR_LIGHT_BASE_3))},
    {SyntaxColorTheme::StyleComponents::ScintillaMargin, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_BASE_1), QColor(SOLAR_LIGHT_BASE_2))},
    {SyntaxColorTheme::StyleComponents::Default, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_BASE_00))},
    {SyntaxColorTheme::StyleComponents::Keyword, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_BLUE), QColor(), false, true)},
    {SyntaxColorTheme::StyleComponents::Comment, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_MAGENTA), QColor(), true, true)},
    {SyntaxColorTheme::StyleComponents::Error, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_ORANGE))},
    {SyntaxColorTheme::StyleComponents::Operator, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_ORANGE))},
    {SyntaxColorTheme::StyleComponents::Number, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_ORANGE))},
    {SyntaxColorTheme::StyleComponents::PlainData, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_BASE_00))},
    {SyntaxColorTheme::StyleComponents::String, SyntaxColorThemeComponent(QColor(SOLAR_LIGHT_VIOLET), QColor(), true)}
  }));

  // Solarized - http://ethanschoonover.com/solarized
  m_colorThemes.append(SyntaxColorTheme(QSL("Solarized"), true, QMap<SyntaxColorTheme::StyleComponents, SyntaxColorThemeComponent> {
    {SyntaxColorTheme::StyleComponents::ScintillaPaper, SyntaxColorThemeComponent(QColor(), QColor(SOLAR_BASE_3))},
    {SyntaxColorTheme::StyleComponents::ScintillaControlChar, SyntaxColorThemeComponent(QColor(SOLAR_CONTROL), QColor(SOLAR_BASE_3))},
    {SyntaxColorTheme::StyleComponents::ScintillaMargin, SyntaxColorThemeComponent(QColor(SOLAR_BASE_1), QColor(SOLAR_BASE_2))},
    {SyntaxColorTheme::StyleComponents::Default, SyntaxColorThemeComponent(QColor(SOLAR_BASE_00))},
    {SyntaxColorTheme::StyleComponents::Keyword, SyntaxColorThemeComponent(QColor(SOLAR_GREEN), QColor(), false, true)},
    {SyntaxColorTheme::StyleComponents::Comment, SyntaxColorThemeComponent(QColor(SOLAR_ORANGE), QColor(), true, true)},
    {SyntaxColorTheme::StyleComponents::Error, SyntaxColorThemeComponent(QColor(SOLAR_VIOLET))},
    {SyntaxColorTheme::StyleComponents::Operator, SyntaxColorThemeComponent(QColor(SOLAR_ORANGE))},
    {SyntaxColorTheme::StyleComponents::Number, SyntaxColorThemeComponent(QColor(SOLAR_ORANGE))},
    {SyntaxColorTheme::StyleComponents::String, SyntaxColorThemeComponent(QColor(SOLAR_RED), QColor(), true)}
  }));

  // TODO: Load custom themes.
}

QList<SyntaxColorTheme> SyntaxHighlighting::colorThemes() {
  if (m_colorThemes.isEmpty()) {
    loadColorThemes();
  }

  return m_colorThemes;
}

SyntaxColorTheme SyntaxHighlighting::defaultTheme() {
  return colorThemes().at(2);
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
      QSL("txt"), QString()
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
      << Lexer(QSL("C++"), QStringList {
      QSL("h"), QSL("hxx"), QSL("hpp"), QSL("c"), QSL("cpp"), QSL("cc"), QSL("cxx")
    }, SCLEX_CPP, {
      {SCE_C_DEFAULT, SyntaxColorTheme::StyleComponents::Default},
      {SCE_C_COMMENT, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_C_COMMENTDOC, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_C_COMMENTDOCKEYWORD, SyntaxColorTheme::StyleComponents::Keyword},
      {SCE_C_COMMENTDOCKEYWORDERROR, SyntaxColorTheme::StyleComponents::Error},
      {SCE_C_COMMENTLINE, SyntaxColorTheme::StyleComponents::Comment},
      {SCE_C_COMMENTLINEDOC, SyntaxColorTheme::StyleComponents::Keyword},
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
    })
      << Lexer(QSL("CSS"), QStringList {
      QSL("css")
    }, SCLEX_CSS)
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
    }, SCLEX_PASCAL)
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
    }, SCLEX_POWERSHELL)
      << Lexer(QSL("Properties/INI"), QStringList {
      QSL("properties"), QSL("ini"), QSL("conf"), QSL("theme")
    }, SCLEX_PROPERTIES)
      << Lexer(QSL("Python"), QStringList {
      QSL("py"), QSL("pyw")
    }, SCLEX_PYTHON)
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
      << Lexer(QSL("Spice"), QStringList {
      QSL("scp"), QSL("out")
    }, SCLEX_SPICE)
      << Lexer(QSL("SQL"), QStringList {
      QSL("sql")
    }, SCLEX_SQL)
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
      {SCE_H_TAG, SyntaxColorTheme::StyleComponents::Identifier},
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

Lexer::Lexer() : m_name(QString()), m_code(SCLEX_NULL), m_suffices(QStringList()) {}

Lexer::Lexer(const QString& name, const QStringList& suffices, int code, const QMap<int, SyntaxColorTheme::StyleComponents>& style_mappings)
  : m_name(name), m_code(code), m_suffices(suffices), m_styleMappings(style_mappings) {}

bool Lexer::isEmpty() const {
  return m_name.isEmpty() && m_suffices.isEmpty();
}
