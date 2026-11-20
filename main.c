/*
 * launcher - a tiny process launcher.
 *
 * Replaces the current process with <file>, forwarding any remaining
 * command-line arguments. If the file cannot be launched, reports why
 * and exits with a nonzero status.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <io.h>
#include <process.h>
#include <stdint.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#define ANSI_RESET "\033[0m"
#define ANSI_BOLD  "\033[1m"
#define ANSI_ERROR "\033[1;31m"

static int stream_is_tty(FILE *stream)
{
#if defined(_WIN32)
    return _isatty(_fileno(stream));
#else
    return isatty(fileno(stream));
#endif
}

static int colors_enabled(FILE *stream)
{
    const char *no_color = getenv("NO_COLOR");
    const char *term = getenv("TERM");

    if ((no_color != NULL && no_color[0] != '\0') ||
        (term != NULL && strcmp(term, "dumb") == 0) ||
        !stream_is_tty(stream)) {
        return 0;
    }

#if defined(_WIN32)
    intptr_t fd = _get_osfhandle(_fileno(stream));

    if (fd == -1) {
        return 0;
    }

    HANDLE handle = (HANDLE)fd;
    DWORD mode;

    if (!GetConsoleMode(handle, &mode)) {
        return 0;
    }

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

    if (!(mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) &&
        !SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        return 0;
    }
#endif

    return 1;
}

static void usage(const char *program)
{
    if (colors_enabled(stderr)) {
        fprintf(stderr, ANSI_BOLD "usage:" ANSI_RESET " %s <file> [args...]\n",
                program);
    } else {
        fprintf(stderr, "usage: %s <file> [args...]\n", program);
    }
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

    if (colors_enabled(stderr)) {
        fprintf(stderr, ANSI_ERROR "%s: cannot launch '%s': %s" ANSI_RESET "\n",
                argv[0], argv[1], strerror(errno));
    } else {
        fprintf(stderr, "%s: cannot launch '%s': %s\n",
                argv[0], argv[1], strerror(errno));
    }

    return 1;
}
