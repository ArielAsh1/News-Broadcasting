#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>

typedef struct _ListNode {
    void *data;
    struct _ListNode *next;
} ListNode;

typedef struct {
    ListNode *head;
    ListNode *last;
    size_t size;
} LinkedList;


LinkedList *listCreate(void);
void listFree(LinkedList *list);
void listPrint(LinkedList *list);

void listAddHead(LinkedList *list, void *data);
void listAddLast(LinkedList *list, void *data);
void *listRemoveLast(LinkedList *list);

void *listRemoveNode(LinkedList *list, ListNode *node);
int listIsEmpty(LinkedList *list);

#endif