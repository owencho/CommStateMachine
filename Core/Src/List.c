#include "List.h"
#include "ListItemCompare.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

List *createList(void){
    List * list = (List *)malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    list->count = 0;
    list->previous = NULL;
    return list;
}

ListItem *createListItem(void* data){
    ListItem * listItem = (ListItem *)malloc(sizeof(ListItem));
    listItem->next = NULL;
    listItem->data = data;
    return listItem;
}

int getLinkedListCount(List * linkedList){
    return linkedList->count;
}

void resetCurrentListItem(List * linkedList){
    if(linkedList == NULL)
        return ;
    linkedList->current = linkedList->head;
    linkedList->previous = NULL;
}

ListItem * getCurrentListItem(List * linkedList){
    if(linkedList == NULL)
        return NULL;
    return linkedList->current;
}

ListItem * getNextListItem(List * linkedList){
    ListItem * returnPtr;
    if(linkedList == NULL)
        return NULL;
    else if(linkedList->current == NULL)
        return NULL;

    returnPtr = linkedList->current;
    linkedList->current = linkedList->current->next;
    linkedList->previous = returnPtr;
    return linkedList->current;

}

List* listAddItemToTail(List * linkedList, ListItem * listItem ){
    //ListItem * listItem ;
    if(linkedList == NULL || listItem == NULL)
        return linkedList;
    //listItem = createListItem(data);
    listItem->next = NULL;
    if(linkedList->head == NULL){
        linkedList->head = listItem;
        linkedList->current = listItem;
        linkedList->previous = NULL;
    }
    else{
        linkedList->tail->next = listItem;
    }
    linkedList->tail = listItem;
    linkedList->count++;
    resetCurrentListItem(linkedList);
    return linkedList;
}

List* listAddItemToHead(List * linkedList,  ListItem * listItem){
  //  ListItem * listItem ;
    if(linkedList == NULL || listItem == NULL)
        return linkedList;
    //listItem = createListItem(data);
    listItem->next = NULL;
    if(linkedList->head == NULL){
        linkedList->head = listItem;
        linkedList->current = listItem;
        linkedList->tail = listItem;
        linkedList->previous = NULL;
    }
    else{
        if(linkedList->current == linkedList->head)
            linkedList->previous = NULL;
        listItem->next = linkedList->head;
        linkedList->head = listItem;
    }
    linkedList->count++;
    resetCurrentListItem(linkedList);
    return linkedList;
}

List* listAddItemToNext(List * linkedList, ListItem * currentlistItem,ListItem * newListItem){
    ListItem * expectedListItem ;
    ListItem * checkListItem ;
    if(linkedList == NULL || currentlistItem == NULL || newListItem == NULL)
        return linkedList;
    //newListItem = createListItem(data);
    newListItem->next = NULL;
    expectedListItem = findListItem(linkedList,currentlistItem,(LinkedListCompare)ListItemCompare) ;
    checkListItem = findListItem(linkedList,newListItem,(LinkedListCompare)ListItemCompare);
    if(currentlistItem != expectedListItem || checkListItem != NULL)
        return linkedList;

    newListItem->next = currentlistItem->next;
    currentlistItem->next = newListItem;
    if(currentlistItem == linkedList->tail)
        linkedList->tail = newListItem;
    linkedList->count++;
    resetCurrentListItem(linkedList);
    return linkedList;
}

void * deleteHeadListItem(List * linkedList){
    ListItem * nextListItem;
    ListItem * deletedListItem;
    //void * data;
    if(linkedList == NULL)         //if linkedList input is NULL
        return NULL;
    if(linkedList->head ==NULL)    // if the linkedList has no listItem
        return NULL;
    deletedListItem = linkedList->head;
    //data = deletedListItem->data;
    nextListItem = linkedList->head->next;
    if(linkedList->current == linkedList->head)  //if current is point to head
        linkedList->current = nextListItem;   // also change current as the head is deleted
    if(nextListItem == NULL)
        linkedList->tail = NULL;             // if last item deleted , the tail also points to NULL
    linkedList->head = nextListItem;
    linkedList->count--;
    //freeListItem(deletedListItem);
    return deletedListItem;
}



void* deleteSelectedListItem(List * linkedList,void  * listItemData,LinkedListCompare compare){
    ListItem * listItem;
    ListItem * deletedListItem;

    if(linkedList==NULL || listItemData == NULL || compare == NULL)
        return NULL;
    if(linkedList->head ==NULL)
        return NULL;

    listItem=findListItem(linkedList,listItemData,compare);
    if(listItem !=NULL)
        deletedListItem = checkAndDeleteListItem(linkedList,listItem);
    resetCurrentListItem(linkedList);
    return deletedListItem;
}

void* checkAndDeleteListItem(List * linkedList,ListItem * listItem){
    //void * data;
    ListItem * deletedListItem;
    if(linkedList == NULL || listItem == NULL){
        return NULL;
    }

    if(listItem == linkedList->head){
        deletedListItem = deleteHeadListItem(linkedList);
    }
    else if(listItem == linkedList->tail){
        linkedList->tail = linkedList->previous;
        linkedList->tail->next = NULL;
        linkedList->count--;
        deletedListItem = listItem;
        //data = listItem->data;
        //freeListItem(listItem);
    }
    else{
        linkedList->count--;
        linkedList->previous->next = listItem->next;
        deletedListItem = listItem;
        //data = listItem->data;
        //freeListItem(listItem);
    }
    resetCurrentListItem(linkedList);
    return deletedListItem;
  }

ListItem * findListItem(List * linkedList,void * listItemData,LinkedListCompare compare ){
    int size;
    ListItem * nextListItem;
    if(linkedList==NULL || listItemData == NULL || compare == NULL)
        return NULL;
    resetCurrentListItem(linkedList);
    nextListItem = getCurrentListItem(linkedList);
    while(nextListItem != NULL){
        size = compare(nextListItem,listItemData);
        if(size)
            return nextListItem;
        nextListItem = getNextListItem(linkedList);
    }
    return NULL;
}

void listForEach(List * linkedList,LinkedListProcessor processor){
    ListItem * listItem;
    listItem = getCurrentListItem(linkedList);
    if(processor == NULL) return;
    while(listItem != NULL){
        processor(listItem);
        listItem = getNextListItem(linkedList);
    }
}
