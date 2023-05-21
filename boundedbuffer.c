#include "boundedbuffer.h"
#include <stdlib.h>

BoundedBuffer *boundedBufferCreate(int size) {
    BoundedBuffer *newBoundedBuffer = malloc(sizeof(BoundedBuffer));
    if (newBoundedBuffer == NULL) {
        return NULL;
    }
    newBoundedBuffer->size = size;
    newBoundedBuffer->list = listCreate();
    return newBoundedBuffer;
}

void boundedBufferFree(BoundedBuffer *boundedBuffer) {
    listFree(boundedBuffer->list);
    free(boundedBuffer);
}

void boundedBufferAdd(BoundedBuffer *boundedBuffer, char *s) {
    listAddHead(boundedBuffer->list, s);
}

char *boundedBufferRemove(BoundedBuffer *boundedBuffer) {
    return listRemoveLast(boundedBuffer->list);
}