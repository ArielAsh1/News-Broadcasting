#include "linkedlist.h"
#include <stdlib.h>

LinkedList *listCreate(void) {
    LinkedList *newList = malloc(sizeof(LinkedList));
    if (newList == NULL) {
        return NULL;
    }

    newList->head = newList->last = NULL;
    newList->size = 0;

    return newList;
}

// free the entire linked list
void listRecursiveFree(ListNode *head) {
    if (head != NULL) {
        listRecursiveFree(head->next);
        free(head->data);
        free(head);
    }
}

void listFree(LinkedList *list) {
    listRecursiveFree(list->head);
    free(list);
}

ListNode *listNodeCreate(void *data) {
    ListNode *newNode = malloc(sizeof(ListNode));
    if (newNode == NULL) {
        return NULL;
    }

    newNode->data = data;
    newNode->next = NULL;
    
    return newNode;
}

void listAddHead(LinkedList *list, void *data) {
    ListNode *newNode = listNodeCreate(data);
    if (list->head == NULL) {
        list->head = list->last = newNode;
    } else {
        newNode->next = list->head;
        list->head = newNode;
    }

    list->size++;
}

void *listRemoveLast(LinkedList *list) {
    // list is empty
    if (list->last == NULL) {
        return NULL;
    }

    ListNode *lastNode = list->last;
    void *lastNodeData = lastNode->data;

    if (list->head == list->last) {
        // list has only one node
        list->head = list->last = NULL;
    } else {
        // list has more than one node
        ListNode *beforeLast = list->head;
        while (beforeLast->next != list->last) {
            beforeLast = beforeLast->next;
        }
        list->last = beforeLast;
        beforeLast->next = NULL;
    }

    free(lastNode);
    list->size--;
    return lastNodeData;
}