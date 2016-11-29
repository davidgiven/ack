#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <setjmp.h>
#include "diagnostics.h"

static bool timed_out = false;
static bool child_exited = false;
static char** command = NULL;
static int timeout = 0;
static jmp_buf exit_jmp;

static void parse_arguments(int argc, char* const argv[])
{
    program_name = argv[0];
    for (;;)
    {
        int c = getopt(argc, argv, "t:");
        if (c == -1)
            break;

        switch (c)
        {
            case 't':
                timeout = atoi(optarg);
                break;

            default:
                fatal("syntax: testrunner <timeout in secs> -- <command>\n");
        }
    }

    command = &argv[optind];
    if (!command[0])
        fatal("you must supply a command");
    if (timeout <= 0)
        fatal("timeout missing or invalid");
}

static void sigalrm_cb(int sigraised)
{
    timed_out = true;
    longjmp(exit_jmp, 1);
}

int main(int argc, char* const argv[])
{
    const int READ = 0;
    const int WRITE = 1;

    int fds[2];
    int pid;
    FILE* childin;
    int wstatus;
    char buffer[4096];

    parse_arguments(argc, argv);

    if (setjmp(exit_jmp) == 0)
    {
        /* First time through. */

        signal(SIGALRM, sigalrm_cb);
        alarm(timeout);

        pipe(fds);
        pid = fork();
        if (pid == 0)
        {
            /* Child */
            close(fds[READ]);
            close(0);
            dup2(fds[WRITE], 1);
            dup2(fds[WRITE], 2);
            execvp(command[0], command);
            _exit(1);
        }

        childin = fdopen(fds[READ], "r");
        while (!timed_out)
        {
            if (!fgets(buffer, sizeof(buffer), childin))
                break;
            fputs(buffer, stdout);

            if (strcmp(buffer, "@@FINISHED\n") == 0)
                break;
            if (strcmp(buffer, "@@FINISHED\r\n") == 0)
                break;
        }
    }

    /* Reached via longjmp, EOF, or seeing a @@FINISHED. */

    kill(pid, SIGKILL);
    waitpid(pid, &wstatus, 0);
    if (timed_out)
        exit(1);
    exit(WEXITSTATUS(wstatus));
}
