/*
 * launcher - a tiny process launcher.
 *
 * Replaces the current process with <file>, forwarding any remaining
 * command-line arguments. If the file cannot be launched, reports why
 * and exits with a nonzero status.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <process.h>
#else
#include <unistd.h>
#endif

static void usage(const char *program)
{
    fprintf(stderr, "usage: %s <file> [args...]\n", program);
}

static int launch(const char *file, char *const argv[])
{
#if defined(_WIN32)
    return _execvp(file, (const char *const *)argv);
#else
    return execvp(file, argv);
#endif
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    launch(argv[1], &argv[1]);

    fprintf(stderr, "%s: cannot launch '%s': %s\n",
            argv[0], argv[1], strerror(errno));
    return 1;
}
