
#include "linkedlist.h"
#include <stdlib.h>

// Creates a new linked list.
LinkedList *listCreate(void) {
    LinkedList *newList = malloc(sizeof(LinkedList));
    if (newList == NULL) {
        return NULL;
    }

    newList->head = newList->last = NULL;
    newList->size = 0;

    return newList;
}

// free the entire linked list nodes recursively
void listRecursiveFree(ListNode *head) {
    // If the node isn't NULL, free its next nodes, then the node itself.
    if (head != NULL) {
        listRecursiveFree(head->next);
        free(head->data);
        free(head);
    }
}

// Frees a linked list.
void listFree(LinkedList *list) {
    listRecursiveFree(list->head);
    free(list);
}

// Creates a new list node with the given data.
ListNode *listNodeCreate(void *data) {
    ListNode *newNode = malloc(sizeof(ListNode));
    if (newNode == NULL) {
        return NULL;
    }
    newNode->data = data;
    newNode->next = NULL;
    
    return newNode;
}

// Adds a new node with the given data to the head of the list.
void listAddHead(LinkedList *list, void *data) {
    ListNode *newNode = listNodeCreate(data);
    if (list->head == NULL) {
        // If the list is empty, the new node is both the head and the last.
        list->head = list->last = newNode;
    } else {
        // Otherwise, the new node becomes the head and its next node is the old head.
        newNode->next = list->head;
        list->head = newNode;
    }

    list->size++;
}

// Adds a new node with the given data to the end of the list.
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

// Removes the last node from the list and returns its data.
void *listRemoveLast(LinkedList *list) {
    return listRemoveNode(list, list->last);
}

// Removes a specific node from the list and returns its data.
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
        // the node to remove is the head, so the head becomes the next node.
        list->head = list->head->next;
    } else {
        // Otherwise, find the node before the node to remove and set its next to the node to remove's next.
        ListNode *beforeNode = list->head;
        while (beforeNode->next != node) {
            beforeNode = beforeNode->next;
        }
        beforeNode->next = node->next;
        if (beforeNode->next == NULL) {
            list->last = beforeNode;
        }
    }
    // Free the node and decrease the size of the list.
    free(node);
    list->size--;
    return nodeData;
}

// Returns whether the list is empty.
int listIsEmpty(LinkedList *list) {
    return list->size == 0;
}

// Prints the contents of the list FOR TESTING
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