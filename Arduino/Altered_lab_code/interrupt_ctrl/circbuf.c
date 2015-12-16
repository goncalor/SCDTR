#include "circbuf.h"

//struct circbuf_t {
//    float *const buf;
//    unsigned head;
//    unsigned tail;
//    const unsigned maxlen;
//};
//typedef struct circbuf_t circbuf_t;

short circbuf_add(circbuf_t *cb, float data)
{
    unsigned next = cb->head + 1;

    // wrap around
    if (next >= cb->maxlen)
        next = 0;

    // buffer is full
    if (next == cb->tail)
        return 0;

    cb->buf[cb->head] = data;
    cb->head = next;
    return 1;
}


short circbuf_remove(circbuf_t *cb, float *data)
{
    // if the head isn't ahead of the tail, we don't have any characters
    if (cb->head == cb->tail)
        return 0;  // quit with an error

    *data = cb->buf[cb->tail];
    cb->buf[cb->tail] = 0;  // clear the data (optional)

    unsigned next = cb->tail + 1;
    if(next >= cb->maxlen)
        next = 0;

    cb->tail = next;

    return 1;
}
