#include "FakeIRQ.h"

int32_t disableIrqCount = 0;
char msg[1024];
void fake_enableIRQ(){
     disableIrqCount--;
}

void fake_disableIRQ(){
     disableIrqCount++;
}

void fakeCheckIRQ(const UNITY_LINE_TYPE lineNumber){
    if(disableIrqCount<0){
        sprintf(msg,"enableIRQ() called more than disableIRQ() for %d time",abs(disableIrqCount));
        UNITY_TEST_FAIL(lineNumber,msg);
    }
    else if (disableIrqCount>0){
        sprintf(msg,"disableIRQ() called more than enableIRQ() for %d time",abs(disableIrqCount));
        UNITY_TEST_FAIL(lineNumber,msg);
    }
}
