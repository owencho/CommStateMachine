#include <stdio.h>
#include "unity.h"
#include "CustomAssert.h"
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <stdarg.h>

#define _STRINGIFY(x)     #x
#define STRINGIFY(x)     _STRINGIFY(x)

void testReportFailure (char* message ,...){
    int actualLength;
    char* buffer;
    va_list arg;
    va_start(arg, message);
    actualLength = vsnprintf(NULL,0, message, arg);   //trick system to take actualLength
    buffer =malloc(actualLength + 1);               // allocate value to buffer
    vsnprintf(buffer,actualLength + 1, message, arg);
    va_end(arg);
    UNITY_TEST_FAIL(__LINE__, buffer);
}
