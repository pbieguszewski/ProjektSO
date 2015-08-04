#include <iostream>
#include <algorithm>
#include <sys/shm.h>
#include <unistd.h>
#include <csignal>
#include "bufor.h"

std::string caesarCipher(const std::string& text, int key);

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        std::cout << "Process 2" << std::endl;
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

    buf->p2 = getpid();

    char tmp[1024];

    while (true)
    {
        //read process1
        read(buf->pipefd1[0], tmp, 1024);

        auto str = caesarCipher(tmp, 7);

        //write process3
        write(buf->pipefd2[1], static_cast<const void*>(str.c_str()), str.length() + 1);

        sleep(3);
    }

    return EXIT_SUCCESS;
}

std::string caesarCipher(const std::string& text, int key)
{
    int num = 26;
    key %= num;

    std::string tmp = text;

    for (auto &i : tmp)
    {
        i += key;

        unsigned char j = static_cast<unsigned char>(i);

        if(j > 'z' )
            i -= num;

        if(j < 'a' ) 
            i += num;
    }

    return tmp;
}
