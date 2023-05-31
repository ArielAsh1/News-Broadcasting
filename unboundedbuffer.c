#include "unboundedbuffer.h"
#include <stdlib.h>

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

void unboundedBufferFree(UnboundedBuffer *unboundedBuffer) {
    pthread_mutex_destroy(&unboundedBuffer->mutex);
    sem_destroy(&unboundedBuffer->semEmpty);
    listFree(unboundedBuffer->list);
    free(unboundedBuffer);
}

void unboundedBufferAdd(UnboundedBuffer *unboundedBuffer, char *s) {
    pthread_mutex_lock(&unboundedBuffer->mutex);
    listAddLast(unboundedBuffer->list, s);
    pthread_mutex_unlock(&unboundedBuffer->mutex);
    sem_post(&unboundedBuffer->semEmpty);
}

char *unboundedBufferRemove(UnboundedBuffer *unboundedBuffer) {
    sem_wait(&unboundedBuffer->semEmpty);
    pthread_mutex_lock(&unboundedBuffer->mutex);
    char *value = listRemoveLast(unboundedBuffer->list);
    pthread_mutex_unlock(&unboundedBuffer->mutex);
    return value;
}