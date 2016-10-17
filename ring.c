#include "assert.h"
#include "ring.h"
#include "types.h"

void ringbuff_init( struct ringbuff* rb, char* data, u16 cap ) {
    rb->data = data;
    rb->cap = cap;
    rb->start = 0;
    rb->size = 0;
}

void ringbuff_push( struct ringbuff* rb, char data ) {
    assert(rb->size + 1 < rb->cap);

    u16 end_ix = (rb->start + rb->size) % rb->cap;
    rb->data[end_ix] = data;
    rb->size += 1;
}
char ringbuff_pop( struct ringbuff* rb ) {
    assert(rb->size > 0);
    char c = rb->data[rb->start];
    rb->start = (rb->start + 1) % rb->cap;
    rb->size -= 1;
    return c;
}

void ringbuff_append( struct ringbuff* rb, char* data ) {
    while(*data) {
        ringbuff_push( rb, *data);
        data++;
    }
}
