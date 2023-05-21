// Ariel Ashkenazy 208465096
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "linkedlist.h"

#define FALSE (0)
#define TRUE  (1)

#define ERROR           (-1)
#define MAX_LINE_LENGTH (100)

typedef struct {
    int id;
    int numberOfProducts;
    int queueSize;
} Producer;

// TODO: can we assume that the file is valid?
// TODO: can we assume that lines don't contain unnecessary spaces?

// for testing
void listPrint(LinkedList *list) {
    printf("LIST:\n");
    int index = 0;
    ListNode *curr = list->head;
    while (curr != NULL) {
        printf("\t%s\n", curr->data);
        curr = curr->next;
        index++;
    }
}

// for testing
void listTest() {
    LinkedList *list = listCreate();
    listPrint(list);
    listRemoveLast(list);
    listPrint(list);
    listAddHead(list, "0");
    listPrint(list);
    listAddHead(list, "1");
    listPrint(list);
    listAddHead(list, "2");
    listPrint(list);
    listRemoveLast(list);
    listPrint(list);
    listRemoveLast(list);
    listPrint(list);
    listRemoveLast(list);
    listPrint(list);
    listRemoveLast(list);
    listPrint(list);
    listFree(list);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return ERROR;
    }

    int coEditorQueueSize = 0;
    LinkedList *producers = listCreate();
    
    int configFd = open(argv[1], O_RDONLY);
    int finishedFile = FALSE;
    while (!finishedFile) {
        int lines[3];
        int linesRead = 0;
        while (TRUE) {
            int number = 0;
            int finishedLine = FALSE;
            int isEmptyLine = TRUE;
            while (!finishedLine) {
                char ch;
                int readBytes = read(configFd, &ch, 1);
                if (readBytes == 0) {
                    finishedFile = finishedLine = TRUE;
                } else if (ch == '\n') {
                    finishedLine = TRUE;
                } else {
                    int digit = ch - '0';
                    number = 10 * number + digit;
                    isEmptyLine = FALSE;
                }
            }

            if (isEmptyLine) {
                break;
            } 

            lines[linesRead++] = number;
        }

        if (linesRead == 3) {
            Producer *producer = malloc(sizeof(Producer));
            if (producer == NULL) {
                return ERROR;
            }
            producer->id = lines[0];
            producer->numberOfProducts = lines[1];
            producer->queueSize = lines[2];
            listAddHead(producers, producer);
        } else if (linesRead == 1) {
            coEditorQueueSize = lines[0];
        }
    }

    

    listFree(producers);
    return 0;
}