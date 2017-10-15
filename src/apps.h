#ifndef EDUOS_APPS_H
#define EDUOS_APPS_H

struct args {
    int res;
    char *argv;
};

extern void shell(void *args);

#endif /* EDUOS_APPS_H */

