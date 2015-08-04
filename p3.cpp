#include <iostream>
#include <sys/shm.h>
#include <unistd.h>
#include <csignal>
#include "bufor.h"

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        std::cout << "Process 3" << std::endl;
        return 0;
    }

    //get key from parent
    key_t shmkey = std::atoi(argv[1]);

    //connect with shared memory
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

    buf->p3 = getpid();

    char tmp[1024];

    while (true)
    {
        //read process2
        read(buf->pipefd2[0], tmp, 1024);

        std::cout << "P3" << tmp << std::endl;

        sleep(3);
    }

    return EXIT_SUCCESS;
}
