#include "platform_mem.h"

#if PLAT_WINDOWS
int plat_mem_lock_all(void)
{
	return 0;
}
#elif PLAT_LINUX
#include <sys/mman.h>

int plat_mem_lock_all(void)
{
	if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0)
	{
		return -1;
	}

	return 0;
}
#else
int plat_mem_lock_all(void)
{
	return -1;
}
#endif
