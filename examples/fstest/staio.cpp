#include "staio.h"

#if defined(ARDUINO_ARCH_AVR)

// TODO

#elif defined(ARDUINO_ARCH_SAMD)

/* Collect newlib-flavored open() flags */
#include <sys/_default_fcntl.h>

int cvtflags(int flags) {
    int ret = (flags + 1) & 3; // O_RDONLY, O_WRONLY

    if (flags & _FAPPEND) {
        ret |= O_APPEND;
    }

    if (flags & _FCREAT) {
        ret |= O_CREAT;
    }

    if (flags & _FTRUNC) {
        ret |= O_TRUNC;
    }

    if (flags & _FEXCL) {
        ret |= O_EXCL;
    }

    return ret;
}

#undef O_RDONLY
#undef O_WRONLY
#undef O_RDWR
#undef O_ACCMODE
#undef O_APPEND
#undef O_SYNC
#undef O_TRUNC
#undef O_CREAT
#undef O_EXCL
/* Done with newlib-flavored open() flags */


#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>

#include <vector>

#include <Arduino.h>
#include <SPI.h>

#undef fputs_P // FIXME: Conflicts with SdFat
#include <SdFat.h>
#define fputs_P(str, file) fputs((str), (file))


#define FIRST_FD            (STDERR_FILENO + 1)
#define APP_START_ADDRESS   0x0000

char *__env[1] = { 0 };
bool ready = false;
SdFat sd;
std::vector<SdFile*> files;
std::vector<int> reuse_fds;

extern "C" {
    int errno = 0;
    char **environ = __env;

    void _exit(int code) {
        /* Reset SD and USB buses */
        SerialUSB.end();
        USBDevice.detach();
        SPI.end();
        SPI1.end();

        /* Pointer to the Application Section */
        void (*application_code_entry)(void);

        /* Rebase the Stack Pointer */
        __set_MSP(*(uint32_t *) APP_START_ADDRESS);

        /* Rebase the vector table base address */
        SCB->VTOR = ((uint32_t) APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);

        /* Load the Reset Handler address of the application */
        application_code_entry = (void (*)(void))(unsigned *)(*(unsigned *)(APP_START_ADDRESS + 4));

        /* Jump to user Reset Handler in the application */
        application_code_entry();
    }

    int _close(int fd) {
        fd -= FIRST_FD;
        if (fd >= 0 && fd < files.size() && files[fd] && files[fd]->close()) {
            delete files[fd];
            files[fd] = NULL;
            reuse_fds.push_back(fd);

            return 0;
        }

        errno = EBADF;
        return -1;
    }

    int _open(const char *name, int flags, int mode) {
        int fd;

        if (!ready) {
            sd.begin(10);
            SPI.setClockDivider(0);
            SPI1.setClockDivider(0);
            ready = true;
        }

        SdFile *file = new SdFile();
        if (!file->open(name, cvtflags(flags))) {
            errno = EACCES;
            return -1;
        }

        if (reuse_fds.size()) {
            fd = reuse_fds.back();
            files[fd] = file;

            reuse_fds.pop_back();
        }
        else {
            fd = files.size();
            if (fd >= MAX_OPEN_FILES) {
                errno = ENFILE;
                return -1;
            }

            files.push_back(file);
        }

        return fd + FIRST_FD;
    }

    int _read(int fd, char *ptr, int len) {
        if (fd <= 0) {
            errno = EBADF;
            return -1;
        }

        switch (fd) {
        case STDIN_FILENO:
            return SerialUSB.readBytes(ptr, len);

        case STDOUT_FILENO:
        case STDERR_FILENO:
            errno = EINVAL;
            return -1;

        default:
            int ret = files[fd - FIRST_FD]->read(ptr, len);
            if (ret < 0) {
                errno = EBADF;
            }

            return ret;
        }
    }

    int _write(int fd, char *ptr, int len) {
        if (fd <= 0) {
            errno = EBADF;
            return -1;
        }

        switch (fd) {
        case STDIN_FILENO:
            errno = EINVAL;
            return -1;

        case STDOUT_FILENO:
        case STDERR_FILENO:
            return SerialUSB.write(ptr, len);

        default:
            int ret = files[fd - FIRST_FD]->write(ptr, len);
            if (ret < 0) {
                errno = EBADF;
            }

            return ret;
        }
    }

    int _stat(const char *name, struct stat *st) {
        // TODO
        st->st_mode = S_IFCHR;
        return 0;
    }
}

#endif
