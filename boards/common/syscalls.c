/**
 * @file syscalls.c
 * @brief Minimal newlib "system calls" shared by all boards.
 *
 * newlib-nano (the C library) expects an OS to provide these. There is no OS:
 * - _write() sends stdout/stderr to the board console, so printf() works,
 * - the rest are harmless stubs (no files, no input yet).
 */
#include <stdio.h>
#include <sys/stat.h>

#include "board.h"
#include "board_common.h"

#define STDOUT_FD          1
#define STDERR_FD          2
#define STDOUT_BUFFER_SIZE 128U

/* Prototypes (newlib has no public header for these). */
int _write(int fd, const char *buf, int len);
int _read(int fd, char *buf, int len);
int _close(int fd);
int _lseek(int fd, int offset, int whence);
int _fstat(int fd, struct stat *st);
int _isatty(int fd);

static char s_stdout_buffer[STDOUT_BUFFER_SIZE];

void board_common_stdio_init(void)
{
    /* Line-buffered: output goes out at every '\n' (or when 128 bytes are
     * queued). A static buffer means stdio never needs malloc(). */
    (void)setvbuf(stdout, s_stdout_buffer, _IOLBF, sizeof s_stdout_buffer);
    /* stderr unbuffered: error messages appear immediately. */
    (void)setvbuf(stderr, NULL, _IONBF, 0U);
}

int _write(int fd, const char *buf, int len)
{
    if ((fd != STDOUT_FD) && (fd != STDERR_FD))
    {
        return -1;
    }
    /* Terminals expect "\r\n"; C code writes "\n". Translate here, in one
     * place, so apps can use plain "\n". */
    int start = 0;
    for (int i = 0; i < len; i++)
    {
        if (buf[i] == '\n')
        {
            board_console_write(&buf[start], (size_t)(i - start));
            board_console_write("\r\n", 2U);
            start = i + 1;
        }
    }
    board_console_write(&buf[start], (size_t)(len - start));
    return len;
}

int _read(int fd, char *buf, int len)
{
    (void)fd;
    (void)buf;
    (void)len;
    return 0; /* no input yet: end of file */
}

int _close(int fd)
{
    (void)fd;
    return -1;
}

int _lseek(int fd, int offset, int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
    return 0;
}

int _fstat(int fd, struct stat *st)
{
    (void)fd;
    st->st_mode = S_IFCHR; /* a character device, like a terminal */
    return 0;
}

int _isatty(int fd)
{
    (void)fd;
    return 1;
}
