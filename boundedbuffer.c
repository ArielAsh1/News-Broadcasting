#include "boundedbuffer.h"
#include <stdlib.h>

BoundedBuffer *boundedBufferCreate(int size) {
    BoundedBuffer *newBoundedBuffer = malloc(sizeof(BoundedBuffer));
    if (newBoundedBuffer  == NULL) {
        return NULL;
    }
    newBoundedBuffer ->buffer = unboundedBufferCreate();
    if (newBoundedBuffer ->buffer == NULL) {
        free(newBoundedBuffer );
        return NULL;
    }
    sem_init(&newBoundedBuffer ->semFull, 0, size);
    return newBoundedBuffer;
}

void boundedBufferFree(BoundedBuffer *boundedBuffer) {
    sem_destroy(&boundedBuffer->semFull);
    free(boundedBuffer->buffer);
    free(boundedBuffer);
}

void boundedBufferAdd(BoundedBuffer *boundedBuffer, char *s) {
    sem_wait(&boundedBuffer->semFull);
    unboundedBufferAdd(boundedBuffer->buffer, s);
}

char *boundedBufferRemove(BoundedBuffer *boundedBuffer) {
    char *value = unboundedBufferRemove(boundedBuffer->buffer);
    sem_post(&boundedBuffer->semFull);
    return value;
}