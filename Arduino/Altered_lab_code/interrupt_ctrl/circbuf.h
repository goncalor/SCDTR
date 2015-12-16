#ifndef _CIRCBUF_H
#define _CIRCBUF_H

struct circbuf_t {
    float *const buf;
    unsigned head;
    unsigned tail;
    const unsigned maxlen;
};
typedef struct circbuf_t circbuf_t;

short circbuf_add(circbuf_t *cb, float data);
short circbuf_remove(circbuf_t *cb, float *data);

#endif
