// Ariel Ashkenazy 208465096
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "boundedbuffer.h"
#include "linkedlist.h"

#define SPORTS  (0)
#define NEWS    (1)
#define WEATHER (2)

#define FALSE (0)
#define TRUE  (1)

#define ERROR (-1)

#define MAX_LINE_LENGTH  (100)
#define MAX_PRINT_LENGTH (250)

typedef struct {
    int id;
    int numberOfProducts;
    int queueSize;
} Producer;

typedef struct {
    int id;
    int numberOfProducts;
    BoundedBuffer *boundedBuffer;
} ProducerFunctionParams;

LinkedList *producerBuffers;
BoundedBuffer *sportsEditorBuffer;
BoundedBuffer *newsEditorBuffer;
BoundedBuffer *weatherEditorBuffer;
BoundedBuffer *screenManagerBuffer;

// TODO:
//  dispatcher
//  main (create threads/buffers and destroy/free)
//  unboundedbuffer (using semaphores/mutex)
//  boundedbuffer (using unbounded and semaphores/mutex)

// QTODO: can we assume that the file is valid?
// QTODO: can we assume that lines don't contain unnecessary spaces?

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

void producerFunction(ProducerFunctionParams *params) {
    int sportsCounter = 0;
    int newsCounter = 0;
    int weatherCounter = 0;

    for (int i = 0; i < params->numberOfProducts; i++) {
        int type = rand() % 3;
        char *typeStr;
        int typeCount;
        switch (type) {
            case SPORTS:
                typeStr = "SPORTS";
                typeCount = sportsCounter++;
                break;
            case NEWS:
                typeStr = "NEWS";
                typeCount = newsCounter++;
                break;
            case WEATHER:
                typeStr = "WEATHER";
                typeCount = weatherCounter++;
                break;
        }

        char *print = malloc(sizeof(char) * MAX_PRINT_LENGTH);
        sprintf(print, "Producer %d %s %d", params->id, typeStr, typeCount);
        boundedBufferAdd(params->boundedBuffer, print);
    }

    char *doneBuffer = malloc(sizeof(char) * (strlen("DONE") + 1));
    strcpy(doneBuffer, "DONE");
    boundedBufferAdd(params->boundedBuffer, doneBuffer);
}

void dispatcherFunction(void) {

}

void screenManagerFunction(void) {
    int coEditorsCount = 3;
    while (coEditorsCount > 0) {
        char *str = boundedBufferRemove(screenManagerBuffer);
        if (strcmp(str, "DONE") == 0) {
            coEditorsCount--;
        } else {
            printf("%s\n", str);
        }
        free(str);
    }
    printf("DONE\n");
}

void coEditorFunction(BoundedBuffer *boundedBuffer) {
    int receivedDone = FALSE;
    while (!receivedDone) {
        sleep(0.1);

        char *str = boundedBufferRemove(boundedBuffer);
        if (strcmp(str, "DONE") == 0) {
            receivedDone = TRUE;
        }
        boundedBufferAdd(screenManagerBuffer, str);
    }
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

    pthread_t producerThreads[producers->size];
    for (int i = 0; i < producers->size; i++) {
        pthread_create(&producerThreads[i], NULL, (void *(*)(void *))producerFunction, NULL);
    }

    // after this loop all the threads finished running and can be freed
    for (int i = 0; i < producers->size; i++) {
        pthread_join(producerThreads[i], NULL);
    }

    listFree(producers);
    return 0;
}
