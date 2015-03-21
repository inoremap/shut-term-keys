#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <termios.h>

int getch(void)
{
    int ch;
    struct termios oldt;
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt); /*store old settings */
    newt = oldt; /* copy old settings to new settings */
    newt.c_lflag &= ~(ICANON | ECHO); /* make one change to old settings in new settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); /*apply the new settings immediatly */
    char chr;
    if (read(STDIN_FILENO, &chr, 1) == 1)
        ch = chr;
    else
        ch = -1;
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); /*reapply the old settings */
    return ch; /*return received char */
}

int g_child_fin = 0;

/* SIGCHLD handler. */
static void sigchld_hdl(int sig)
{
    (void)sig;
    g_child_fin = 1;
    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
    }
}

int main (int argc, char *argv[])
{
    struct sigaction act;

    memset(&act, 0, sizeof(act));
    act.sa_handler = sigchld_hdl;

    if (sigaction(SIGCHLD, &act, 0))
    {
        perror("sigaction");
        return 1;
    }

    pid_t chld_pid = fork();
    switch (chld_pid)
    {
        case -1:
            perror("fork");
            return 1;
        case 0:
        {
            char* largv[argc];
            for (int arg = 1; arg < argc; ++arg)
            {
                largv[arg - 1] = argv[arg];
            }
            largv[argc - 1] = 0;
            execvp(largv[0], largv);
            fprintf(stderr, "execvp fail\n");
            return 0;
        }
    }
    //fprintf(stderr, "chld=%d\n", chld_pid);

    while (!g_child_fin)
    {
        //fprintf(stderr, "read...\n");
        getch();
    }
    //fprintf(stderr, "child finished\n");

    return 0;
}
