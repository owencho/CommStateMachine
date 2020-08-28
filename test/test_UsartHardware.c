#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include "unity.h"
#include "unity_internals.h"
#include "UsartHardware.h"
#include "UsartDriver.h"
#include "mock_Irq.h"
#include "mock_EventQueue.h"
#include "mock_Rcc.h"
#include "mock_Gpio.h"
#include "mock_Nvic.h"
#include "Event.h"
#include "EventCompare.h"
#include "List.h"
#include "ListItemCompare.h"
#include "UsartEvent.h"
#include "TimerEventQueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SM_Common.h"
#include "FakeIRQ.h"
#include "CommStateMachine.h"
#include "CommEventQueue.h"
#include "Common.h"
#include "mock_Usart.h"
void setUp(void){}

void tearDown(void){}

void test_UsartHardware_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement UsartHardware");
}
