#include "ListItemCompare.h"

int ListItemCompare(ListItem *listItem, ListItem *compareListItem){
    if(listItem == compareListItem)
        return 1;
    return 0;
}
