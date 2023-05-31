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
#include "unboundedbuffer.h"
#include "linkedlist.h"

#define SPORTS  (0)
#define NEWS    (1)
#define WEATHER (2)

#define FALSE (0)
#define TRUE  (1)

#define ERROR   (-1)
#define SUCCESS (0)

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
UnboundedBuffer *screenManagerBuffer;

// TODO:
//  dispatcher
//  main (create threads/buffers and destroy/free)
//  unboundedbuffer (using semaphores/mutex)
//  boundedbuffer (using unbounded and semaphores/mutex)

// QTODO: can we assume that the file is valid?
// QTODO: can we assume that lines don't contain unnecessary spaces?

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

    boundedBufferAdd(params->boundedBuffer, strdup("DONE"));
}

void dispatcherFunction(void) {
    while (!listIsEmpty(producerBuffers)) {
        ListNode *node = producerBuffers->head;
        while (node != NULL) {
            BoundedBuffer *producerBuffer = node->data;
            char *value = boundedBufferRemove(producerBuffer);
            if (strcmp("DONE", value) == 0) {
                ListNode *nextNode = node->next;
                listRemoveNode(producerBuffers, node);
                free(value);
                node = nextNode;
            } else {
                if (strstr("SPORTS", value) != NULL) {
                    boundedBufferAdd(sportsEditorBuffer, value);
                } else if (strstr("NEWS", value) != NULL) {
                    boundedBufferAdd(newsEditorBuffer, value);
                } else if (strstr("WEATHER", value) != NULL) {
                    boundedBufferAdd(weatherEditorBuffer, value);
                }
                node = node->next;
            }
        }
    }

    boundedBufferAdd(sportsEditorBuffer, strdup("DONE"));
    boundedBufferAdd(newsEditorBuffer, strdup("DONE"));
    boundedBufferAdd(weatherEditorBuffer, strdup("DONE"));
}

void screenManagerFunction(void) {
    int coEditorsCount = 3;
    while (coEditorsCount > 0) {
        char *str = unboundedBufferRemove(screenManagerBuffer);
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
        unboundedBufferAdd(screenManagerBuffer, str);
    }
}

int readConf(LinkedList *producers, int *coEditorQueueSize, char *filePath) {
    int configFd = open(filePath, O_RDONLY);
    if (configFd == ERROR) {
        return ERROR;
    }

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
            *coEditorQueueSize = lines[0];
        }
    }

    return SUCCESS;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return ERROR;
    }

    int coEditorQueueSize = 0;
    LinkedList *producers = listCreate();
    if (readConf(producers, &coEditorQueueSize, argv[1]) == ERROR) {
        return ERROR;
    }

    screenManagerBuffer = unboundedBufferCreate();
    if (screenManagerBuffer == NULL) return ERROR;
    sportsEditorBuffer = boundedBufferCreate(coEditorQueueSize);
    if (sportsEditorBuffer == NULL) return ERROR;
    newsEditorBuffer = boundedBufferCreate(coEditorQueueSize);
    if (newsEditorBuffer == NULL) return ERROR;
    weatherEditorBuffer = boundedBufferCreate(coEditorQueueSize);
    if (weatherEditorBuffer == NULL) return ERROR;

    pthread_t screenThread, dispatcherThread;
    pthread_create(&screenThread, NULL, (void *(*)(void *))screenManagerFunction, NULL);
    pthread_create(&dispatcherThread, NULL, (void *(*)(void *))dispatcherFunction, NULL);

    BoundedBuffer *coEditorBuffers[3] = { sportsEditorBuffer, newsEditorBuffer, weatherEditorBuffer };
    pthread_t coEditorThreads[3];
    for (int i = 0; i < 3; i++) {
        pthread_create(&coEditorThreads[i], NULL, (void *(*)(void *))coEditorFunction, coEditorBuffers[i]);
    }

    producerBuffers = listCreate();
    ListNode *producerConf = producers->head;
    ProducerFunctionParams producerParams[producers->size];
    pthread_t producerThreads[producers->size];
    for (int i = 0; i < producers->size; i++) {
    // int id;
    // int numberOfProducts;
    // BoundedBuffer *boundedBuffer;
        Producer *producer = producerConf->data;
        producerParams[i].id = producer->id;
        producerParams[i].numberOfProducts = producer->numberOfProducts;
        BoundedBuffer *buffer = boundedBufferCreate(producer->queueSize);
        producerParams[i].boundedBuffer = buffer;
        listAddLast(producerBuffers, buffer);
        pthread_create(&producerThreads[i], NULL, (void *(*)(void *))producerFunction, &producerParams[i]);
    }   

    // after this loop all the threads finished running and can be freed
    for (int i = 0; i < producers->size; i++) {
        pthread_join(producerThreads[i], NULL);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(coEditorThreads[i], NULL);
    }

    pthread_join(screenThread, NULL);
    pthread_join(dispatcherThread, NULL);

    boundedBufferFree(sportsEditorBuffer);
    boundedBufferFree(newsEditorBuffer);
    boundedBufferFree(weatherEditorBuffer);
    unboundedBufferFree(screenManagerBuffer);
    listFree(producerBuffers);
    listFree(producers);

    return 0;
}
