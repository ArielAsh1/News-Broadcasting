
#include "boundedbuffer.h"
#include <stdlib.h>

// This function creates a new bounded buffer with a specified size.
BoundedBuffer *boundedBufferCreate(int size) {
    BoundedBuffer *newBoundedBuffer = malloc(sizeof(BoundedBuffer));
    if (newBoundedBuffer  == NULL) {
        return NULL;
    }
    newBoundedBuffer ->buffer = unboundedBufferCreate();
    if (newBoundedBuffer ->buffer == NULL) {
        // If unable to create the unbounded buffer, free the memory allocated for the bounded buffer.
        free(newBoundedBuffer );
        return NULL;
    }
    // Initialize the semaphore for the buffer with the maximum size.
    sem_init(&newBoundedBuffer ->semFull, 0, size);
    return newBoundedBuffer;
}

// This function frees a bounded buffer and its associated resources.
void boundedBufferFree(BoundedBuffer *boundedBuffer) {
    sem_destroy(&boundedBuffer->semFull);
    unboundedBufferFree(boundedBuffer->buffer);
    free(boundedBuffer);
}

// This function adds a string to a bounded buffer if there is space.
void boundedBufferAdd(BoundedBuffer *boundedBuffer, char *s) {
    // Wait for the semaphore to ensure that the buffer is not full.
    sem_wait(&boundedBuffer->semFull);
    unboundedBufferAdd(boundedBuffer->buffer, s);
}

// This function removes and returns a string from a bounded buffer.
char *boundedBufferRemove(BoundedBuffer *boundedBuffer) {
    char *value = unboundedBufferRemove(boundedBuffer->buffer);
    sem_post(&boundedBuffer->semFull);
    return value;
}

// This function prints the contents of a bounded buffer.
void boundedBufferPrint(BoundedBuffer *buffer) {
    unboundedBufferPrint(buffer->buffer);
}
