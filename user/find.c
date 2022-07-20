#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void do_find(char const *path, char const *target)
{
    char buf[512], *p;
    struct dirent de;
    struct stat st;
    int fd = open(path, 0);
    if (fd < 0)
    {
        fprintf(2, "fail to open: %s\n", path);
        exit(1);
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "fail to stat: %s\n", path);
        exit(1);
    }

    switch (st.type)
    {
    case T_FILE:
        fprintf(2, "not a dir\n");
        exit(1);
    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0)
            {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            if (st.type == T_DIR)
            {
                do_find(buf, target);
            }
            else if (st.type == T_FILE)
            {
                if (strcmp(de.name, target) == 0)
                {
                    printf("%s\n", buf);
                }
            }
        }
        break;
    }
    close(fd);
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "parameter is wrong");
        exit(1);
    }

    char const *path = argv[1];
    char const *target = argv[2];

    do_find(path, target);
    exit(0);
}