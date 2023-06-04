
#ifndef UNBOUNDEDBUFFER_H
#define UNBOUNDEDBUFFER_H

#include "linkedlist.h"
#include <pthread.h>
#include <semaphore.h>

typedef struct {
    LinkedList *list;
    pthread_mutex_t mutex;
    sem_t semEmpty;
} UnboundedBuffer;

UnboundedBuffer *unboundedBufferCreate();
void unboundedBufferPrint(UnboundedBuffer *unboundedBuffer);
void unboundedBufferFree(UnboundedBuffer *unboundedBuffer);
void unboundedBufferAdd(UnboundedBuffer *unboundedBuffer, char *s);
char *unboundedBufferRemove(UnboundedBuffer *unboundedBuffer);

#endif