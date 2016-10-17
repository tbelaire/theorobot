#ifndef RING_H
#define RING_H

#include "types.h"

struct ringbuff {
    char* data;
    u16 cap;
    u16 start;
    u16 size;
};

void ringbuff_init( struct ringbuff* rb, char* data, u16 cap );

void ringbuff_push( struct ringbuff* rb, char data );
char ringbuff_pop( struct ringbuff* rb );
void ringbuff_append( struct ringbuff* rb, char* data );

#endif // RING_H
