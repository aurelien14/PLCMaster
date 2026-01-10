/* PLC scheduler implementation. */

#include "core/plc/plc_scheduler.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "core/platform/platform_time.h"

static void *plc_scheduler_thread(void *arg);
static int plc_scheduler_add_task_internal(PlcScheduler_t* s, const PlcTask_t* task);
static void plc_scheduler_drain_pending(PlcScheduler_t* s);
static int plc_scheduler_queue_task(PlcScheduler_t* s, const PlcTask_t* task);

int plc_scheduler_init(PlcScheduler_t* s, uint32_t base_cycle_ms)
{
	if (s == NULL || base_cycle_ms == 0)
	{
		return -1;
	}

	s->base_cycle_ms = base_cycle_ms;
	s->task_count = 0;
	plat_atomic_store_u32(&s->pending_write, 0U);
	plat_atomic_store_u32(&s->pending_read, 0U);
	memset(s->pending_ready, 0, sizeof(s->pending_ready));
	plat_atomic_store_bool(&s->running, false);
	s->backend_cycle_begin = NULL;
	s->backend_cycle_commit = NULL;
	s->backend_user = NULL;
	s->on_cycle_begin = NULL;
	s->on_cycle_end = NULL;
	s->user = NULL;
	memset(&s->thread, 0, sizeof(s->thread));
	return 0;
}

int plc_scheduler_add_task(PlcScheduler_t* s, const PlcTask_t* task)
{
	if (plat_atomic_load_bool(&s->running))
	{
		return -1;
	}

	return plc_scheduler_add_task_internal(s, task);
}

int plc_scheduler_add_task_runtime(PlcScheduler_t* s, const PlcTask_t* task)
{
	if (s == NULL || task == NULL || task->fn == NULL || task->period_ms == 0)
	{
		return -1;
	}

	if (!plat_atomic_load_bool(&s->running))
	{
		return plc_scheduler_add_task(s, task);
	}

	return plc_scheduler_queue_task(s, task);
}

int plc_scheduler_set_backend_hooks(PlcScheduler_t* s, void (*cycle_begin)(void *user), void (*cycle_commit)(void *user), void *user)
{
	if (s == NULL || plat_atomic_load_bool(&s->running))
	{
		return -1;
	}

	s->backend_cycle_begin = cycle_begin;
	s->backend_cycle_commit = cycle_commit;
	s->backend_user = user;
	return 0;
}

int plc_scheduler_set_callbacks(PlcScheduler_t* s, void (*on_cycle_begin)(void *user), void (*on_cycle_end)(void *user), void *user)
{
	if (s == NULL || plat_atomic_load_bool(&s->running))
	{
		return -1;
	}

	s->on_cycle_begin = on_cycle_begin;
	s->on_cycle_end = on_cycle_end;
	s->user = user;
	if (s->backend_cycle_begin == NULL)
	{
		s->backend_cycle_begin = on_cycle_begin;
		s->backend_user = user;
	}
	if (s->backend_cycle_commit == NULL)
	{
		s->backend_cycle_commit = on_cycle_end;
		s->backend_user = user;
	}
	return 0;
}

int plc_scheduler_start(PlcScheduler_t* s)
{
	if (s == NULL)
	{
		return -1;
	}

	if (plat_atomic_exchange_bool(&s->running, true))
	{
		return -1;
	}

	if (plat_thread_create(&s->thread, PLAT_THREAD_NORMAL, NULL, plc_scheduler_thread, s) != 0)
	{
		plat_atomic_store_bool(&s->running, false);
		return -1;
	}

	return 0;
}

int plc_scheduler_stop(PlcScheduler_t* s)
{
	if (s == NULL)
	{
		return -1;
	}

	if (!plat_atomic_exchange_bool(&s->running, false))
	{
		return 0;
	}

	return plat_thread_join(&s->thread);
}

bool plc_scheduler_is_running(const PlcScheduler_t* s)
{
	if (s == NULL)
	{
		return false;
	}

	return plat_atomic_load_bool(&s->running);
}

static void *plc_scheduler_thread(void *arg)
{
	uint32_t i;
	uint64_t start_ms;
	uint64_t next_tick_ms;
	PlcScheduler_t* s = (PlcScheduler_t*)arg;

	if (s == NULL)
	{
		return NULL;
	}

	start_ms = plat_time_monotonic_ms();
	next_tick_ms = start_ms + s->base_cycle_ms;

	for (i = 0; i < s->task_count; ++i)
	{
		uint64_t phase_start = start_ms + s->tasks[i].phase_ms;

		if (s->tasks[i].period_ms > 0)
		{
			if (phase_start >= s->tasks[i].period_ms)
			{
				phase_start -= s->tasks[i].period_ms;
			}
			else
			{
				phase_start = 0;
			}
		}

		s->tasks[i].last_run_ms = (uint32_t)phase_start;
	}

	while (plat_atomic_load_bool(&s->running))
	{
		uint64_t now_ms = plat_time_monotonic_ms();

		if (now_ms < next_tick_ms)
		{
			uint64_t sleep_ms = next_tick_ms - now_ms;
			plat_thread_sleep_ms((uint32_t)sleep_ms);
			continue;
		}

		if (now_ms > next_tick_ms && (now_ms - next_tick_ms) > s->base_cycle_ms)
		{
			printf("[PLC] overrun %" PRIu64 "ms\n", now_ms - next_tick_ms);
		}

		plc_scheduler_drain_pending(s);

		if (s->backend_cycle_begin != NULL)
		{
			s->backend_cycle_begin(s->backend_user);
		}

		if (s->on_cycle_begin != NULL && s->on_cycle_begin != s->backend_cycle_begin)
		{
			s->on_cycle_begin(s->user);
		}

		for (i = 0; i < s->task_count; ++i)
		{
			PlcTask_t* task = &s->tasks[i];
			uint64_t task_start_ms = start_ms + task->phase_ms;

			if (now_ms >= task_start_ms && (now_ms - (uint64_t)task->last_run_ms) >= task->period_ms)
			{
				int rc = 0;

				while ((now_ms - (uint64_t)task->last_run_ms) >= task->period_ms)
				{
					task->last_run_ms += task->period_ms;
				}

				rc = task->fn(task->ctx);
				if (rc != 0)
				{
					printf("[PLC] task %s rc=%d\n", task->name ? task->name : "(null)", rc);
				}
			}
		}

		if (s->on_cycle_end != NULL && s->on_cycle_end != s->backend_cycle_commit)
		{
			s->on_cycle_end(s->user);
		}

		if (s->backend_cycle_commit != NULL)
		{
			s->backend_cycle_commit(s->backend_user);
		}

		next_tick_ms += s->base_cycle_ms;

		if (now_ms > next_tick_ms && (now_ms - next_tick_ms) > (5 * s->base_cycle_ms))
		{
			next_tick_ms = now_ms + s->base_cycle_ms;
		}
	}

	return NULL;
}

static int plc_scheduler_add_task_internal(PlcScheduler_t* s, const PlcTask_t* task)
{
	if (s == NULL || task == NULL || task->fn == NULL || task->period_ms == 0)
	{
		return -1;
	}

	if (s->task_count >= sizeof(s->tasks) / sizeof(s->tasks[0]))
	{
		return -1;
	}

	s->tasks[s->task_count] = *task;
	s->tasks[s->task_count].last_run_ms = 0;
	s->task_count++;
	return 0;
}

static void plc_scheduler_drain_pending(PlcScheduler_t* s)
{
	uint32_t read_index;
	uint32_t write_index;
	uint32_t capacity;

	if (s == NULL)
	{
		return;
	}

	capacity = (uint32_t)(sizeof(s->pending_tasks) / sizeof(s->pending_tasks[0]));
	read_index = plat_atomic_load_u32(&s->pending_read);
	write_index = plat_atomic_load_u32(&s->pending_write);

	while (read_index < write_index)
	{
		uint32_t slot = read_index % capacity;

		if (!plat_atomic_load_bool(&s->pending_ready[slot]))
		{
			break;
		}

		PlcTask_t task = s->pending_tasks[slot];
		plat_atomic_store_bool(&s->pending_ready[slot], false);
		plat_atomic_store_u32(&s->pending_read, read_index + 1U);
		read_index++;

		if (plc_scheduler_add_task_internal(s, &task) != 0)
		{
			printf("[PLC] drop pending task %s\n", task.name ? task.name : "(null)");
		}
	}
}

static int plc_scheduler_queue_task(PlcScheduler_t* s, const PlcTask_t* task)
{
	uint32_t capacity;

	if (s == NULL || task == NULL)
	{
		return -1;
	}

	capacity = (uint32_t)(sizeof(s->pending_tasks) / sizeof(s->pending_tasks[0]));

	for (;;)
	{
		uint32_t read_index = plat_atomic_load_u32(&s->pending_read);
		uint32_t write_index = plat_atomic_load_u32(&s->pending_write);

		if ((write_index - read_index) >= capacity)
		{
			return -1;
		}

		if (plat_atomic_cas_u32(&s->pending_write, write_index, write_index + 1U))
		{
			uint32_t slot = write_index % capacity;
			s->pending_tasks[slot] = *task;
			plat_atomic_store_bool(&s->pending_ready[slot], true);
			return 0;
		}
	}
}
