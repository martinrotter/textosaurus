// For license of this file, see <project-root-folder>/LICENSE.md.

#include "definitions/definitions.h"
#include "miscellaneous/syntaxhighlighting.h"

#include "scintilla/include/SciLexer.h"

#include <QRegularExpression>

SyntaxHighlighting::SyntaxHighlighting(QObject* parent) : QObject(parent), m_fileFilters(QStringList()),
  m_lexers(Lexers()) {}

Lexer SyntaxHighlighting::lexerForFile(const QString& file_name) {
  QRegularExpression reg("(?:\\.)(\\w+$)|($)");
  QString suffix = reg.match(file_name).captured(1);

  return lexerForSuffix(suffix);
}

Lexer SyntaxHighlighting::lexerForSuffix(const QString& suffix) {
  foreach (const Lexer& lex, lexers()) {
    if (lex.m_suffices.contains(suffix)) {
      return lex;
    }
  }

  // We return first lexer, which is lexer for plain text files.
  return m_lexers.first();
}

Lexer SyntaxHighlighting::lexerForName(const QString& name) {
  foreach (const Lexer& lex, lexers()) {
    if (lex.m_name == name) {
      return lex;
    }
  }

  // We return first lexer, which is lexer for plain text files.
  return m_lexers.first();
}

QStringList SyntaxHighlighting::fileFilters() {
  if (m_fileFilters.isEmpty()) {
    foreach (const Lexer& lex, lexers()) {
      m_fileFilters << lex.m_name + tr(" files (*") + lex.m_suffices.join(QL1S(" *")) + QL1C(')');
    }
  }

  return m_fileFilters;
}

Lexers SyntaxHighlighting::lexers() {
  if (m_lexers.isEmpty()) {
    m_lexers
      << Lexer(tr("Plain text"), QStringList {
      QSL("txt"), QString()
    }, SCLEX_NULL)
      << Lexer(QSL("A68k"), QStringList {
      QSL("doxygen")
    }, SCLEX_A68K)
      << Lexer(QSL("Assembly"), QStringList {
      QSL("asm"), QSL("nasm")
    }, SCLEX_ASM)
      << Lexer(QSL("AVS"), QStringList {
      QSL("avs")
    }, SCLEX_AVS)
      << Lexer(QSL("Bash"), QStringList {
      QSL("bash"), QSL("sh"), QSL("zsh")
    }, SCLEX_BASH)
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
      QSL("conf"), QSL("ini")
    }, SCLEX_CONF)
      << Lexer(QSL("C++"), QStringList {
      QSL("h"), QSL("hxx"), QSL("hpp"), QSL("c"), QSL("cpp"), QSL("cc"), QSL("cxx")
    }, SCLEX_CPP)
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
      << Lexer(QSL("Java"), QStringList {
      QSL("java")
    }, SCLEX_CPP)
      << Lexer(QSL("JavaScript"), QStringList {
      QSL("js")
    }, SCLEX_CPP)
      << Lexer(QSL("JSON"), QStringList {
      QSL("json")
    }, SCLEX_JSON)
      << Lexer(QSL("Lua"), QStringList {
      QSL("lua")
    }, SCLEX_LUA)
      << Lexer(QSL("Make"), QStringList {
      QSL("mak"), QSL("makefile")
    }, SCLEX_MAKEFILE)
      << Lexer(QSL("Markdown"), QStringList {
      QSL("markdown"), QSL("md")
    }, SCLEX_MARKDOWN)
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
      << Lexer(QSL("Properties"), QStringList {
      QSL("properties"), QSL("ini"), QSL("conf"), QSL("theme")
    }, SCLEX_PROPERTIES)
      << Lexer(QSL("Python"), QStringList {
      QSL("py"), QSL("pyw")
    }, SCLEX_PYTHON)
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
      << Lexer(QSL("XML"), QStringList {
      QSL("xml"), QSL("xaml"), QSL("xsl"), QSL("xslt"), QSL("xml"),
      QSL("xsd"), QSL("xul"), QSL("kml"), QSL("svg"), QSL("mxml"), QSL("xsml"),
      QSL("wsdl"), QSL("xlf"), QSL("xliff"), QSL("xbl"), QSL("xml"), QSL("sxbl"),
      QSL("sitemap"), QSL("gml"), QSL("gpx"), QSL("plist"), QSL("ts"), QSL("qrc"), QSL("vcxproj")
    }, SCLEX_XML)
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

Lexer::Lexer(const QString& name, const QStringList& suffices, int code)
  : m_name(name), m_code(code), m_suffices(suffices) {}

bool Lexer::isEmpty() const {
  return m_name.isEmpty() && m_suffices.isEmpty();
}
