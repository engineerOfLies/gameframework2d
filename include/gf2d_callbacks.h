#ifndef __GF2D_CALLBACKS_H__
#define __GF2D_CALLBACKS_H__

typedef struct
{
    void *data;
    void (*callback)(void *data);
}Callback;

Callback *gf2d_callback_new(void (*callback)(void *data),void *data);
void gf2d_callback_free(Callback *callback);
void gf2d_callback_call(Callback *callback);

#endif
