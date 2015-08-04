#include <iostream>
#include <algorithm>
#include <string>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include "bufor.h"

int main(int /*argc*/, const char** argv)
{
    //parent pid
    pid_t myPid = getpid();

    key_t shmkey = ftok(argv[0], 128);

    //init shared memory
    int shmid = shmget(shmkey, sizeof(Bufor), IPC_CREAT | 0666);

    if (shmid == -1)
        return EXIT_FAILURE;

    //get pointer shared memory
    Bufor* buf = static_cast<Bufor*>(shmat(shmid, nullptr, 0));

    if (!buf)
    {
        //delete shared memory
        shmctl(shmid, IPC_RMID, 0);
        return EXIT_FAILURE;
    }

    buf->p0 = myPid;

    int pipefd1[2];

    //init pipe1
    if (pipe(pipefd1))
    {
        //delete shared memory
        shmdt(buf);
        shmctl(shmid, IPC_RMID, 0);
        return EXIT_FAILURE;
    }

    std::copy(pipefd1, pipefd1 + 2, buf->pipefd1);

    int pipefd2[2];

    //init pipe2
    if (pipe(pipefd2))
    {
        //close pipe1
        close(pipefd1[0]);
        close(pipefd1[1]);
        //delete shared memory
        shmdt(buf);
        shmctl(shmid, IPC_RMID, 0);
        return EXIT_FAILURE;
    }
	
    std::copy(pipefd2, pipefd2 + 2, buf->pipefd2);

    //create process 1
    pid_t p1 = fork();

    if (p1 == 0)
    {
        execl("p1", "p1", std::to_string(shmkey).c_str(), nullptr);
    }
    else if (p1 < 0)
    {
        //close all pipe
        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);
        //delete shared memory
        shmdt(buf);
        shmctl(shmid, IPC_RMID, 0);
        return EXIT_FAILURE;
    }

    //create process 2
    pid_t p2 = fork();

    if (p2 == 0)
    {
        execl("p2", "p2", std::to_string(shmkey).c_str(), nullptr);
    }
    else if (p2 < 0)
    {
        //close all pipe
        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);
        //delete shared memory
        shmdt(buf);
        shmctl(shmid, IPC_RMID, 0);
        //kill process 1
        kill(SIGKILL, buf->p1);
        return EXIT_FAILURE;
    }

    //create process 1
    pid_t p3 = fork();

    if (p3 == 0)
    {
        execl("p3", "p3", std::to_string(shmkey).c_str(), nullptr);
    }
    else if (p3 < 0)
    {
        //close all pipe
        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);
        //delete shared memory
        shmdt(buf);
        shmctl(shmid, IPC_RMID, 0);
        //kill process
        kill(SIGKILL, buf->p1);
        kill(SIGKILL, buf->p2);
        return EXIT_FAILURE;
    }

    int status = 0;

    waitpid(p1, &status, 0);

    //kill process
    kill(SIGKILL, buf->p2);
    kill(SIGKILL, buf->p3);

    //close pipes
    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);

    //close shared memory
    shmdt(buf);
    shmctl(shmid, IPC_RMID, 0);

    return EXIT_SUCCESS;
}
