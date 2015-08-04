#ifndef BUFOR_H
#define BUFOR_H

#include <sys/types.h>

struct Bufor final
{
    pid_t p0;
    pid_t p1;
    pid_t p2;
    pid_t p3;
    int pipefd1[2];
    int pipefd2[2];
};

#endif // BUFOR_H
