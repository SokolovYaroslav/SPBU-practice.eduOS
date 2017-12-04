
#include <errno.h>

#include <stdbool.h>

#include <stdlib.h>

#include "os/sem.h"

#include "os/sched.h"

#include "os/irq.h"


struct sem {
	bool used;
	int semid;
	int cnt;
	int cnt_max;
	struct wq wq;
};

struct sem g_sem;

int sem_init(int cnt) {
	if (cnt < 1) {
		return -EINVAL;
	}
	struct sem *sem = NULL;
	irqmask_t irq = irq_disable();
	if (!g_sem.used) {
		sem = &g_sem;
		sem->used = true;
		TAILQ_INIT(&sem->wq.head);
	}
	irq_enable(irq);
	
	if (!sem) {
		return -ENOMEM;
	}

	sem->cnt_max = cnt;
	sem->cnt = cnt;

	return 0;
}

int sem_down(int semid) {
	irqmask_t irq = irq_disable();
	if (semid != 0 || !g_sem.used) {
		irq_enable(irq);
		return -EINVAL;
	}

	while (true) {
		if (g_sem.cnt > 0) {
			g_sem.cnt--;
			break;
		}
		else {
			sched_wait(&g_sem.wq);
			sched();
		}
	}
	irq_enable(irq);

	return 0;
}

int sem_up(int semid) {
	irqmask_t irq = irq_disable();
	if (semid != 0 || !g_sem.used) {
		irq_enable(irq);
		return -EINVAL;
	}
	
	if (g_sem.cnt < g_sem.cnt_max) {
		g_sem.cnt++;
	}
	if (!TAILQ_EMPTY(&g_sem.wq.head)) {
		sched_notify(TAILQ_FIRST(&g_sem.wq.head));
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

