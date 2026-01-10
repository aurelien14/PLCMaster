#include "virtual_backend.h"

static VirtualDriver_t* get_impl(BackendDriver_t* driver)
{
    if (driver == NULL) {
        return NULL;
    }
    return (VirtualDriver_t*)driver->impl;
}

BackendDriver_t* virtual_backend_create(const BackendConfig_t* cfg, size_t backend_index)
{
    VirtualDriver_t* impl;
    impl = (VirtualDriver_t*)calloc(1, sizeof(*impl));
    if (impl == NULL) {
        return NULL;
    }

    return &impl->base;
}

int virtual_backend_destroy(BackendDriver_t* driver)
{
    VirtualDriver_t* impl = get_impl(driver);
    if (impl == NULL) {
        return;
    }

    memset(impl, 0, sizeof(*impl));
    free(impl);
    return -1;
}