#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

void __attribute__((noreturn)) _exit(int exit_value)
{
    (void)exit_value;
    *(int *)0x220a0000 = 0xdeadbeef;
    while (1)
        ;
}

void *__attribute__((weak)) _sbrk(ptrdiff_t incr)
{
    extern char *_end;        /* Symbol defined in the linker script */
    extern char *__stack_top; /* Symbol defined in the linker script */
    static char *heap_end = NULL;
    char *base;

    if (!heap_end)
        heap_end = (char *)&_end;
    base = heap_end;

    if (heap_end + incr > (char *)&__stack_top)
    {
        errno = ENOMEM;
        return (void *)-1;
    }
    heap_end += incr;

    return base;
}

ssize_t __attribute__((weak)) _write(int fd, const void *ptr, size_t len)
{
#if 1
    (void)fd;
    (void)ptr;
    (void)len;
    errno = EBADF;
    return -1;
#else
    size_t i = 0;
    if (fd == 1 || fd == 2)
    {
        for (i = 0; i < len; i++)
            *(char *)0x10000000 = ((char *)ptr)[i];
    }
    return i;
#endif
}

ssize_t __attribute__((weak)) _read(int fd, void *ptr, size_t len)
{
    (void)fd;
    (void)ptr;
    (void)len;
    errno = EBADF;
    return -1;
}

int __attribute__((weak)) _open(const char *path, int flags, int mode)
{
    (void)path;
    (void)flags;
    (void)mode;
    errno = ENOENT;
    return -1;
}

int __attribute__((weak)) _close(int fd)
{
    if ((fd >= 0) && (fd <= 2))
        return 0;
    errno = EBADF;
    return -1;
}

int __attribute__((weak)) _isatty(int fd)
{
    if (fd >= 0 && fd <= 2)
    {
        return 1;
    }
    errno = EBADF;
    return -1;
}

int __attribute__((weak)) _fstat(int fd, struct stat *st)
{
    if (fd >= 0 && fd <= 2)
    {
        st->st_dev = 22;
        st->st_ino = 7;
        st->st_mode = S_IFCHR | 0620;
        st->st_nlink = 1;
        st->st_uid = 0;
        st->st_gid = 0;
        st->st_rdev = (dev_t)34820;
        st->st_size = 0;
        st->st_blksize = 1024;
        st->st_blocks = 0;
        st->st_atim.tv_sec = 0;
        st->st_atim.tv_nsec = 0;
        st->st_mtim.tv_sec = 0;
        st->st_mtim.tv_nsec = 0;
        st->st_ctim.tv_sec = 0;
        st->st_ctim.tv_nsec = 0;
        return 0;
    }
    errno = EBADF;
    return -1;
}

int __attribute__((weak)) _stat(const char *path, struct stat *st)
{
    (void)path;
    (void)st;
    errno = ENOENT;
    return -1;
}

off_t __attribute__((weak)) _lseek(int fd, off_t offset, int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
    errno = EBADF;
    return -1;
}

int __attribute__((weak)) _gettimeofday(struct timeval *tp, void *tzp)
{
    (void)tp;
    (void)tzp;
    return -1;
}

int __attribute__((weak)) _unlink(const char *path)
{
    (void)path;
    errno = EBADF;
    return -1;
}

int __attribute__((weak)) _link(const char *old, const char *new)
{
    (void)old;
    (void)new;
    errno = EMLINK;
    return -1;
}
