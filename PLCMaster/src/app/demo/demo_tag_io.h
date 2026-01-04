/* Optional demo for exercising tag read/write flows in development builds. */

#ifndef DEMO_TAG_IO_H
#define DEMO_TAG_IO_H

#include "core/runtime/runtime.h"

#ifdef DEV
int demo_tag_io_run(Runtime_t *rt);
#endif /* DEV */

#endif /* DEMO_TAG_IO_H */
