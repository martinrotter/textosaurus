// For license of this file, see <project-root-folder>/LICENSE.md.

#include "common/exceptions/ioexception.h"

IOException::IOException(const QString& message) : ApplicationException(message) {}
