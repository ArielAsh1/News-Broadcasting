
#include "unboundedbuffer.h"
#include <stdlib.h>

// This function creates a new unbounded buffer and initializes its fields.
UnboundedBuffer *unboundedBufferCreate() {
    UnboundedBuffer *newUnboundedBuffer = malloc(sizeof(UnboundedBuffer));
    if (newUnboundedBuffer == NULL) {
        return NULL;
    }
    newUnboundedBuffer->list = listCreate();
    pthread_mutex_init(&newUnboundedBuffer->mutex, NULL);
    sem_init(&newUnboundedBuffer->semEmpty, 0, 0);
    return newUnboundedBuffer;
}

// This function frees an unbounded buffer and its associated resources.
void unboundedBufferFree(UnboundedBuffer *unboundedBuffer) {
    pthread_mutex_destroy(&unboundedBuffer->mutex);
    sem_destroy(&unboundedBuffer->semEmpty);
    listFree(unboundedBuffer->list);
    free(unboundedBuffer);
}

// This function adds a string to an unbounded buffer.
void unboundedBufferAdd(UnboundedBuffer *unboundedBuffer, char *s) {
    pthread_mutex_lock(&unboundedBuffer->mutex);
    listAddHead(unboundedBuffer->list, s);
    pthread_mutex_unlock(&unboundedBuffer->mutex);
    sem_post(&unboundedBuffer->semEmpty);
}

// This function removes and returns a string from an unbounded buffer.
char *unboundedBufferRemove(UnboundedBuffer *unboundedBuffer) {
    sem_wait(&unboundedBuffer->semEmpty);
    pthread_mutex_lock(&unboundedBuffer->mutex);
    char *value = listRemoveLast(unboundedBuffer->list);
    pthread_mutex_unlock(&unboundedBuffer->mutex);
    return value;
}

// This function prints the contents of an unbounded buffer.
void unboundedBufferPrint(UnboundedBuffer *unboundedBuffer) {
    pthread_mutex_lock(&unboundedBuffer->mutex);
    listPrint(unboundedBuffer->list);
    pthread_mutex_unlock(&unboundedBuffer->mutex);
}
