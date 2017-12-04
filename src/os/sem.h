#ifndef EDUOS_OS_SEM_H
#define EDUOS_OS_SEM_H

#include "os/sched.h"
#include "third-party/queue.h"

struct wq {
	TAILQ_HEAD(wq_listhead, sched_task) head;
};

extern int sem_init(int cnt);

extern int sem_down(int semid);

extern int sem_up(int semid);

extern int sem_free(int semid);

#endif /* EDUOS_OS_SEM_H */

