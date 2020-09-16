#include "ExceptionTestSupport.h"

void dumpExceptionString(Exception *ex,UNITY_LINE_TYPE lineNumber){
    printf("ERROR on line %d : %s \n",lineNumber,ex->errorMessage);
}
