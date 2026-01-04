/* System builder declarations. TODO: describe system assembly interfaces. */

#ifndef SYSTEM_BUILDER_H
#define SYSTEM_BUILDER_H

#include "core/runtime/runtime.h"
#include "app/config/config_static.h"

int system_build(Runtime_t *rt, const SystemConfig_t *config);

#endif /* SYSTEM_BUILDER_H */
