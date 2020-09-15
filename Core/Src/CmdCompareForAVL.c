#include "CmdCompareForAVL.h"

int cmdCompareForAVL (CmdNode *node, int * cmd){
    if(node->command < *cmd)
        return -1;
    else if (node->command > *cmd)
        return 1;
    return 0;
}
