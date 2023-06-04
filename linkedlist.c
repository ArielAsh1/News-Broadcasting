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

void listAddLast(LinkedList *list, void *data) {
    ListNode *newNode = listNodeCreate(data);
    if (listIsEmpty(list)) {
        list->head = list->last = newNode;
    } else {
        list->last->next = newNode;
        list->last = newNode;
    }

    list->size++;
}

void *listRemoveLast(LinkedList *list) {
    return listRemoveNode(list, list->last);
}

void *listRemoveNode(LinkedList *list, ListNode *node) {
    // list is empty
    if (listIsEmpty(list)) {
        return NULL;
    }

    void *nodeData = node->data;

    if (list->size == 1) {
        // list has only one node
        list->head = list->last = NULL;
    } else if (list->head == node) {
        list->head = list->head->next;
    } else {
        // list has more than one node
        ListNode *beforeNode = list->head;
        while (beforeNode->next != node) {
            beforeNode = beforeNode->next;
        }
        beforeNode->next = node->next;
        if (beforeNode->next == NULL) {
            list->last = beforeNode;
        }
    }

    free(node);
    list->size--;
    return nodeData;
}

int listIsEmpty(LinkedList *list) {
    return list->size == 0;
}

void listPrint(LinkedList *list) {
    printf("LIST: %d\n", list->size);
    int index = 0;
    ListNode *curr = list->head;
    while (curr != NULL) {
        printf("\t%d. %s\n", index, curr->data);
        curr = curr->next;
        index++;
    }
}