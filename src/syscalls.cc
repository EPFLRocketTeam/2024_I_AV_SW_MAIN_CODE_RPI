#include <unistd.h>

// Stub for _exit, required by libc.
void _exit(int status) {
    while (1);  // Infinite loop to halt execution (no return)
}

// Stub for _write, used by printf and other output functions.
int _write(int file, char *ptr, int len) {
    int i;
    for (i = 0; i < len; i++) {
        // Implement this to write data to a UART, console, etc.
        // For now, it does nothing.
    }
    return len;
}

// Stub for _read, if needed for your program (e.g., stdin).
int _read(int file, char *ptr, int len) {
    return 0; // No input handling
}

// Stub for _close, if needed.
int _close(int file) {
    return -1; // No file system
}

// Stub for _lseek, if needed.
int _lseek(int file, int ptr, int dir) {
    return 0; // No file system
}

// Stub for _fstat, if needed.
int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;  // No file system
}

// Stub for _isatty, if needed.
int _isatty(int file) {
    return 1; // Pretend we always have a terminal
}

// Stub for _sbrk, used to allocate memory (heap).
caddr_t _sbrk(int incr) {
    static char *heap = 0x10000; // Start of the heap memory
    char *prev_heap = heap;
    heap += incr;
    return (caddr_t) prev_heap;
}
