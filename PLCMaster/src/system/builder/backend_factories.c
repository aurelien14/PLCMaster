/* Backend registry static factory list. */

#include "core/backend/backend_registry.h"

#include "backends/ethercat/ec_backend.h"

#define BACKEND_ENTRY(type, name, create, destroy) \
	{ \
		(type), \
		(name), \
		(create), \
		(destroy) \
	},

const BackendFactoryEntry_t g_backend_factories[] = {
#include "system/builder/backend_list.def"
};

const size_t g_backend_factories_count =
	sizeof(g_backend_factories) / sizeof(g_backend_factories[0]);

#undef BACKEND_ENTRY
