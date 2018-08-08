// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef IOEXCEPTION_H
#define IOEXCEPTION_H

#include "common/exceptions/applicationexception.h"

class IOException : public ApplicationException {
  public:
    explicit IOException(QString message = QString());
    virtual ~IOException() = default;
};

#endif // IOEXCEPTION_H
