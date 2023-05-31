#ifndef BOUNDEDBUFFER_H
#define BOUNDEDBUFFER_H

#include "unboundedbuffer.h"
#include <semaphore.h>

typedef struct {
    UnboundedBuffer *buffer;
    sem_t semFull;
} BoundedBuffer;

BoundedBuffer *boundedBufferCreate(int size);
void boundedBufferFree(BoundedBuffer *boundedBuffer);
void boundedBufferAdd(BoundedBuffer *boundedBuffer, char *s);
char *boundedBufferRemove(BoundedBuffer *boundedBuffer);

#endif