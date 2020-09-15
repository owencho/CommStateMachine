#include "unity.h"
#include "CmdNode.h"
#include "CmdCompareForAVL.h"
#include "unity.h"
#include "Avl.h"
#include "IntNode.h"
#include "Compare.h"
#include "Balance.h"
#include "Rotate.h"
#include "Node.h"
#include "AvlError.h"
#include "Exception.h"
#include "CException.h"

CEXCEPTION_T ex;
Node * root ;
CmdNode nodeAdd;
CmdNode node1, node5, node10, node15,node20,node25,node30,node35,node40,node45,node50,node55;
CmdNode node60,node65,node70,node75,node80,node85,node90,node95,node99;
void setUp(void){
    node1.command =1;   node5.command =5;   node15.command =15;
    node20.command =20; node25.command =25; node30.command =30;
    node35.command =35; node40.command =40; node45.command =45;
    node50.command =50; node55.command =55; node60.command =60;
    node65.command =65; node70.command =70; node75.command =75;
    node80.command =80; node85.command =85; node90.command =90;
    node95.command =95; node99.command =99; node10.command =10;
}
void tearDown(void){}
void initCmdNode(CmdNode * node,  CmdNode * left ,CmdNode * right,int balanceFactor){
    node->left = left;
    node->right = right;
    node->bFactor = balanceFactor;
}




//This function is used to compare the int node inside the AVL tree
// to determine the node to be added on the left or right depends on the size
// of the int command
// IntCompare return 1 when when root > nodeAdd
// IntCompare return -1 when when root < nodeAdd
// IntCompare return 0 when when root == nodeAdd
void test_cmdCompareForAVL_smaller_root_return_neg1(void){
    Compare compare = (Compare)cmdCompareForAVL;
    int i = 56;
    TEST_ASSERT_EQUAL(-1,(compare((Node*)&node5,(void *)&i)));
}

void test_cmdCompareForAVL_larger_root_return_1(void){
    Compare compare = (Compare)cmdCompareForAVL;
    int i = 56;
    TEST_ASSERT_EQUAL(1,(compare((Node*)&node99,(void *)&i)));
}

void test_cmdCompareForAVL_same(void){
    Compare compare = (Compare)cmdCompareForAVL;
    int i = 99;
    TEST_ASSERT_EQUAL(0,(compare((Node*)&node99,(void *)&i)));
}
