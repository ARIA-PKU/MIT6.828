#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void next_proc(int p[2])
{
    close(p[1]);
    int prime, n;
    if (read(p[0], &prime, 4) != 4)
    {
        fprintf(2, "child fail to read");
        exit(1);
    }
    fprintf(2, "prime %d\n", prime);

    if (read(p[0], &n, 4) != 0)
    {
        int nextp[2];
        pipe(nextp);
        if (fork() == 0)
        {
            next_proc(nextp);
        }
        else
        {
            close(nextp[0]);
            if (n % prime != 0)
            {
                write(nextp[1], &n, 4);
            }

            while (read(p[0], &n, 4))
            {
                if (n % prime)
                {
                    write(nextp[1], &n, 4);
                }
            }
            close(p[0]);
            close(nextp[1]);
            wait(0);
        }
    }
    exit(0);
}

int main(int argc, char const *argv[])
{
    int p[2];
    pipe(p);

    if (fork() == 0)
    {
        next_proc(p);
    }
    else
    {
        close(p[0]);
        for (int i = 2; i <= 35; i++)
        {
            if (write(p[1], &i, 4) != 4)
            {
                fprintf(2, "fail to write in first process");
                exit(1);
            }
        }
        close(p[1]);
        wait(0);
        exit(0);
    }
    exit(0);
}