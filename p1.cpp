#include <iostream>
#include <sys/shm.h>
#include <unistd.h>
#include "bufor.h"

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        std::cout << "Process 1" << std::endl;
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

    buf->p1 = getpid();

    std::string tmp;

    std::cout << "Enter the text: ";

    while (std::cin >> tmp)
    {
        std::cout << "Enter the text: ";

        //write process2
        write(buf->pipefd1[1], static_cast<const void*>(tmp.c_str()), tmp.length() + 1);

        sleep(3);
    }

    return EXIT_SUCCESS;
}
