# launcher

A tiny process launcher written in C. Give it a file and it gets out of the way: `launcher` replaces itself with the target process and forwards the rest of the arguments.

## Build

```sh
make
```

No dependencies beyond a C compiler and `make`. You can also compile it directly:

```sh
cc -O2 -Wall -Wextra -Wpedantic -o launcher main.c
```

## Usage

```sh
./launcher <file> [args...]
```

`launcher` uses `execvp`, so `<file>` can be an absolute or relative path, or a command name looked up on `PATH`.

```console
$ ./launcher ls -la
...
$ ./launcher ./scripts/deploy.sh production
...
$ ./launcher python3 -c 'print("hello from launcher")'
hello from launcher
```

If the target cannot be launched, `launcher` prints the reason and exits with status 1.

Usage and error messages are colourised only when `stderr` is a terminal. Colour is disabled automatically for redirected output, when `TERM=dumb`, or when the `NO_COLOR` environment variable is set.

## Test

```sh
make test
```

## How it works

On POSIX systems, `launcher` calls [`execvp(3)`](https://man7.org/linux/man-pages/man3/exec.3.html), replacing the launcher's process image with the target. The target inherits the launcher's PID, file descriptors, environment, and exit status, so there is no intermediate process left behind. On Windows, the build uses `_execvp` from `<process.h>` for the same replace-this-process behavior.

## License

MIT
