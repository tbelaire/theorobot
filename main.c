#include "assert.h"
#include "ring.h"
#include "types.h"


#define QUEUE_SIZE 32

char inbuf[QUEUE_SIZE];
char outbuf[QUEUE_SIZE];

struct ringbuff in_ring;
struct ringbuff out_ring;

void setup() {
    ringbuff_init(&in_ring, inbuf, QUEUE_SIZE);
    ringbuff_init(&out_ring, outbuf, QUEUE_SIZE);
}
void loop() {

}

int main() {
    setup();
    while(1) {
        loop();
    }
}
