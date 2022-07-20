#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
    char buf = 'A';

    int c2p[2]; // child -> parent
    int p2c[2]; // parent -> child
    pipe(c2p);
    pipe(p2c);

    int pid = fork();

    if (pid < 0)
    {
        fprintf(2, "fork() failed\n");
        close(c2p[0]);
        close(c2p[1]);
        close(p2c[0]);
        close(p2c[1]);
        exit(1);
    }
    else if (pid == 0)
    { // children
        close(c2p[0]);
        close(p2c[1]);

        if (read(p2c[0], &buf, sizeof(buf)) != 1)
        {
            fprintf(2, "fail to read in child");
            close(c2p[1]);
            close(p2c[0]);
            exit(1);
        }
        else
        {
            fprintf(1, "%d: received ping\n", getpid());
            close(c2p[1]);
            close(p2c[0]);
            exit(0);
        }
    }
    else
    { // parent
        close(c2p[1]);
        close(p2c[0]);
        if (write(p2c[1], &buf, 1) != 1)
        {
            fprintf(2, "fail to write in parent");
            close(c2p[0]);
            close(p2c[1]);
            exit(1);
        }
        else
        {
            fprintf(1, "%d: received pong\n", getpid());
            close(c2p[0]);
            close(p2c[1]);
            exit(0);
        }
    }
}