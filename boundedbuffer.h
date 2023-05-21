#ifndef BOUNDEDBUFFER_H
#define BOUNDEDBUFFER_H

#include "linkedlist.h"

typedef struct {
    LinkedList *list;
    int size;
} BoundedBuffer;

BoundedBuffer *boundedBufferCreate(int size);
void boundedBufferFree(BoundedBuffer *boundedBuffer);
void boundedBufferAdd(BoundedBuffer *boundedBuffer, char *s);
char *boundedBufferRemove(BoundedBuffer *boundedBuffer);

#endif