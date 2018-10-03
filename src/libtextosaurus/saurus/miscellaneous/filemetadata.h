// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef FILEMETADATA_H
#define FILEMETADATA_H

#include "definitions/definitions.h"
#include "saurus/miscellaneous/syntaxhighlighting.h"

#include <QString>

struct TEXTOSAURUS_DLLSPEC FileMetadata {
  public:
    static FileMetadata getInitialMetadata(const QString& file_path, const QString& explicit_encoding = QString());
    QString m_encoding;
    Lexer m_lexer;
    int m_eolMode;
};

#endif // FILEMETADATA_H
