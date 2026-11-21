/**
 * @file syscalls.c
 * @brief Minimal newlib "system call" stubs shared by all boards.
 *
 * newlib-nano (the C library) expects an OS to provide these. There is no OS,
 * so they do nothing. Without them the linker falls back to nosys.specs and
 * prints "_close is not implemented" warnings.
 *
 * Next step: make _write() send bytes to the ST-LINK virtual COM port UART,
 * then printf() works.
 */
#include <sys/stat.h>

/* Prototypes (newlib declares no public headers for these). */
int _write(int fd, const char *buf, int len);
int _read(int fd, char *buf, int len);
int _close(int fd);
int _lseek(int fd, int offset, int whence);
int _fstat(int fd, struct stat *st);
int _isatty(int fd);

int _write(int fd, const char *buf, int len)
{
    (void)fd;
    (void)buf;
    return len; /* pretend everything was written */
}

int _read(int fd, char *buf, int len)
{
    (void)fd;
    (void)buf;
    (void)len;
    return 0; /* end of file */
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
