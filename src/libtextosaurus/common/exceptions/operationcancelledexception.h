// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef OPERATIONCANCELLEDEXCEPTION_H
#define OPERATIONCANCELLEDEXCEPTION_H

#include "common/exceptions/applicationexception.h"

class OperationCancelledException : public ApplicationException {
  public:
    explicit OperationCancelledException(QString message = QString());
    virtual ~OperationCancelledException() = default;
};

#endif // OPERATIONCANCELLEDEXCEPTION_H
