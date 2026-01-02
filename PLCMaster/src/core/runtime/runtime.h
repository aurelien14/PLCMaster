/* Runtime lifecycle interface. TODO: expand runtime structure. */

#ifndef RUNTIME_H
#define RUNTIME_H

typedef struct Runtime
{
    void* tag_table;
    void* io_view;
    void* status_view;
    void* backends;
} Runtime_t;

int runtime_init(Runtime_t* rt);
void runtime_deinit(Runtime_t* rt);

#endif /* RUNTIME_H */
