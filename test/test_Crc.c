#include "unity.h"
#include "Crc.h"

void setUp(void){}

void tearDown(void){}

void test_Crc(void){
    char array[4] ={0x12,0x23,0x24,0x25};
    TEST_ASSERT_EQUAL(generateCrc16(array, 4),0xD8AA);
    TEST_ASSERT_EQUAL(generateCrc16(array, 4),0xD8AA);
}
