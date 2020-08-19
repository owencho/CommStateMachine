#ifndef FAKEIRQ_H
#define FAKEIRQ_H
#include "unity.h"


void fake_enableIRQ();
void fake_disableIRQ();
void fakeCheckIRQ(const UNITY_LINE_TYPE lineNumber);


#endif // FAKE_IRQ_H
