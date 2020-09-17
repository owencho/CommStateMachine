#ifndef EXCEPTIONTESTSUPPORT_H
#define EXCEPTIONTESTSUPPORT_H
#include "Exception.h"
#include "unity.h"
void dumpExceptionString(Exception *ex,UNITY_LINE_TYPE lineNumber);
#define dumpException(ex)        \
                      dumpExceptionString(ex,__LINE__)

#endif // EXCEPTIONTESTSUPPORT_H
