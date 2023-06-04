
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

// The function for a Producer in the broadcasting system.
// Each producer creates news stories of different types and adds them to its private queue.
void producerFunction(ProducerFunctionParams *params) {
    int sportsCounter = 0;
    int newsCounter = 0;
    int weatherCounter = 0;

    // Loop for producing the specified number of news stories.
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
        // Add the news story to the Producer's private queue.
        boundedBufferAdd(params->boundedBuffer, print);
    }

    // Notify the Dispatcher that this Producer has finished producing.
    boundedBufferAdd(params->boundedBuffer, strdup("DONE"));
}

// The function for the Dispatcher in the broadcasting system.
// The Dispatcher continuously scans the Producers' private queues and sorts the news stories.
void dispatcherFunction(void) {
    // Keep checking the Producers' private queues until all are empty.
    while (!listIsEmpty(producerBuffers)) {
        ListNode *node = producerBuffers->head;
        // Scan each Producer's private queue.
        while (node != NULL) {
            BoundedBuffer *producerBuffer = node->data;
            char *value = boundedBufferRemove(producerBuffer);
            // Check if the Producer has finished producing.
            if (strcmp("DONE", value) == 0) {
                ListNode *nextNode = node->next;
                boundedBufferFree(producerBuffer);
                listRemoveNode(producerBuffers, node);
                free(value);
                node = nextNode;
            } else {
                // Sort the news story by type and add it to the appropriate queue.
                if (strstr(value, "SPORTS") != NULL) {
                    boundedBufferAdd(sportsEditorBuffer, value);
                } else if (strstr(value, "NEWS") != NULL) {
                    boundedBufferAdd(newsEditorBuffer, value);
                } else if (strstr(value, "WEATHER") != NULL) {
                    boundedBufferAdd(weatherEditorBuffer, value);
                }
                node = node->next;
            }
        }
    }
    
    // Notify the Co-Editors that the Dispatcher has finished sorting.
    boundedBufferAdd(sportsEditorBuffer, strdup("DONE"));
    boundedBufferAdd(newsEditorBuffer, strdup("DONE"));
    boundedBufferAdd(weatherEditorBuffer, strdup("DONE"));
}

// The function for the Screen Manager in the broadcasting system.
// The Screen Manager displays the sorted news stories received from the Co-Editors.
void screenManagerFunction(void) {
    // Number of Co-Editors the Screen Manager expects to receive "DONE" messages from.
    int coEditorsCount = 3;
    // Keep displaying news stories until all Co-Editors have finished editing.
    while (coEditorsCount > 0) {
        char *str = unboundedBufferRemove(screenManagerBuffer);
        // Check if a Co-Editor has finished editing.
        if (strcmp(str, "DONE") == 0) {
            coEditorsCount--;
        } else {
            // Print the edited news story to the screen.
            printf("%s\n", str);
        }
        free(str);
    }
    // Notify that the Screen Manager has finished displaying all news stories.
    printf("DONE\n");
}


// The function for a Co-Editor in the broadcasting system.
// Each Co-Editor receives news stories of a certain type from the Dispatcher, edits them, and passes them to the Screen Manager
void coEditorFunction(BoundedBuffer *boundedBuffer) {
    // Keep track of whether this Co-Editor has received a "DONE" message from the Dispatcher.
    int receivedDone = FALSE;
    // Keep editing news stories until the Dispatcher has finished sorting.
    while (!receivedDone) {
        // Simulate the editing process by blocking for one tenth of a second.
        sleep(0.1);

        char *str = boundedBufferRemove(boundedBuffer);
        if (strcmp(str, "DONE") == 0) {
            receivedDone = TRUE;
        }
        // Add the edited news story to the Screen Manager's queue.
        unboundedBufferAdd(screenManagerBuffer, str);
    }
}

// This function reads the configuration of the broadcasting system from a file.
// It constructs the Producers based on this configuration, and determines the size of the Co-Editors' queue.
int readConf(LinkedList *producers, int *coEditorQueueSize, char *filePath) {
    int configFd = open(filePath, O_RDONLY);
    if (configFd == ERROR) {
        return ERROR;
    }

    // Keep track of whether we have finished reading the file.
    int finishedFile = FALSE;
    while (!finishedFile) {
        int lines[3];
        int linesRead = 0;
        // Read until we encounter an empty line.
        while (TRUE) {
            int number = 0;
            int finishedLine = FALSE;
            int isEmptyLine = TRUE;
            // Continue to read the line until we encounter a newline character.
            while (!finishedLine) {
                char ch;
                int readBytes = read(configFd, &ch, 1);
                // If we've reached the end of the file, mark this line and the file as finished.
                if (readBytes == 0) {
                    finishedFile = finishedLine = TRUE;
                } else if (ch == '\n') {
                    // If we've reached the end of the line, mark this line as finished.
                    finishedLine = TRUE;
                } else {
                    // Convert the character to a number and add it to the current number.
                    int digit = ch - '0';
                    number = 10 * number + digit;
                    isEmptyLine = FALSE;
                }
            }

            if (isEmptyLine) {
                break;
            } 

            // Store the number in the array and increment the count of lines read.
            lines[linesRead++] = number;
        }

        // If we've read three lines, create a new Producer with the numbers as parameters.
        if (linesRead == 3) {
            Producer *producer = malloc(sizeof(Producer));
            if (producer == NULL) {
                return ERROR;
            }
            producer->id = lines[0];
            producer->numberOfProducts = lines[1];
            producer->queueSize = lines[2];
            listAddLast(producers, producer);
        } else if (linesRead == 1) {
            // If we've only read one line, set it as the size of the Co-Editors' queue.
            *coEditorQueueSize = lines[0];
        }
    }
    
    // Return a success code if we've successfully read the entire file.
    return SUCCESS;
}

// The main function of the program, where execution begins.
// It expects one command line argument: the path to the configuration file.
int main(int argc, char *argv[]) {
    if (argc != 2) {
        return ERROR;
    }

    int coEditorQueueSize = 0;
    LinkedList *producers = listCreate();
    if (readConf(producers, &coEditorQueueSize, argv[1]) == ERROR) {
        return ERROR;
    }

    // create the relevant buffers
    screenManagerBuffer = unboundedBufferCreate();
    if (screenManagerBuffer == NULL) return ERROR;
    sportsEditorBuffer = boundedBufferCreate(coEditorQueueSize);
    if (sportsEditorBuffer == NULL) return ERROR;
    newsEditorBuffer = boundedBufferCreate(coEditorQueueSize);
    if (newsEditorBuffer == NULL) return ERROR;
    weatherEditorBuffer = boundedBufferCreate(coEditorQueueSize);
    if (weatherEditorBuffer == NULL) return ERROR;

    producerBuffers = listCreate();
    // Loop through the Producers and set up their parameters and threads.
    ListNode *producerConf = producers->head;
    ProducerFunctionParams producerParams[producers->size];
    pthread_t producerThreads[producers->size];
    for (int i = 0; i < producers->size; i++) {
        Producer *producer = producerConf->data;
        producerParams[i].id = producer->id;
        producerParams[i].numberOfProducts = producer->numberOfProducts;
        BoundedBuffer *buffer = boundedBufferCreate(producer->queueSize);
        producerParams[i].boundedBuffer = buffer;
        listAddLast(producerBuffers, buffer);
        // Create a new thread for the Producer function.
        pthread_create(&producerThreads[i], NULL, (void *(*)(void *))producerFunction, &producerParams[i]);

        producerConf = producerConf->next;
    }   
    
    // Create new threads for the Screen Manager and Dispatcher functions.
    pthread_t screenThread, dispatcherThread;
    pthread_create(&screenThread, NULL, (void *(*)(void *))screenManagerFunction, NULL);
    pthread_create(&dispatcherThread, NULL, (void *(*)(void *))dispatcherFunction, NULL);

    // Create new threads for the Co-Editor functions.
    BoundedBuffer *coEditorBuffers[3] = { sportsEditorBuffer, newsEditorBuffer, weatherEditorBuffer };
    pthread_t coEditorThreads[3];
    for (int i = 0; i < 3; i++) {
        pthread_create(&coEditorThreads[i], NULL, (void *(*)(void *))coEditorFunction, coEditorBuffers[i]);
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

    // Free memory allocated for the bounded and unbounded buffers.
    boundedBufferFree(sportsEditorBuffer);
    boundedBufferFree(newsEditorBuffer);
    boundedBufferFree(weatherEditorBuffer);
    unboundedBufferFree(screenManagerBuffer);
    ListNode *producerBuffer = producerBuffers->head;
    while (producerBuffer != NULL) {
        boundedBufferFree((BoundedBuffer *)producerBuffer->data);
        producerBuffer = producerBuffer->next;
    }
    // Free memory allocated for the linked lists.
    listFree(producerBuffers);
    listFree(producers);

    return 0;
}
