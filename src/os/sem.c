
#include <errno.h>

#include <stdbool.h>

#include <stdlib.h>

#include "os/sem.h"

#include "os/sched.h"

#include "os/irq.h"

struct sem {
	bool used;
	int cnt;
	int cnt_max;
	TAILQ_HEAD(listhead, sched_task) head;
};

struct sem g_sem;

int sem_init(int cnt) {
	if (g_sem.used) {
		return -ENOMEM;
	}

	g_sem.used = true;
	g_sem.cnt_max = cnt;
	g_sem.cnt = cnt;

	return 0;
}

int sem_down(int semid) {
	if (semid != 0 || !g_sem.used) {
		return -EINVAL;
	}

	irqmask_t irq = irq_disable();
	while (true) {
		if (g_sem.cnt > 0) {
			g_sem.cnt--;
			return 0;
		}
		else {
			TAILQ_INSERT_TAIL(&g_sem.head, sched_current(), link);
			sched_wait();
			sched();
		}
	}
	irq_enable(irq);

	return 0;
}

int sem_up(int semid) {
	if (semid != 0 || !g_sem.used) {
		return -EINVAL;
	}
	
	irqmask_t irq = irq_disable();
	g_sem.cnt++;
	struct sched_task *other = TAILQ_FIRST(&g_sem.head);
	if (other) {
		TAILQ_REMOVE(&g_sem.head, other, link);
		sched_notify(other);
	}
	irq_enable(irq);

	return 0;
}

int sem_free(int semid) {
	if (semid != 0 || !g_sem.used) {
		return -EINVAL;
	}
	g_sem.used = false;
	return 0;
}

