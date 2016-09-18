/*
 * Standard Tiny Arcade Input/Output
 *
 * This library provides stdio compatability on the Arduino platforms, allowing
 * the standard libc functions on both simulator and hardware.
 */

#ifndef __STAIO_H__
#define __STAIO_H__

#include <stdio.h>


#if defined(ARDUINO_ARCH_AVR)

// TODO

#elif defined(ARDUINO_ARCH_SAMD)

#include <errno.h>
#undef errno

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    extern int errno;
    extern char **environ;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // defined(ARDUINO_ARCH_AVR)


#if !defined(ARDUINO_ARCH_AVR)

// AVR compatiblity macros
#define vfprintf_P(file, fmt, args)         vfprintf((file), (fmt), (args))
#define printf_P(...)                       printf(__VA_ARGS__)
#define sprintf_P(...)                      sprintf(__VA_ARGS__)
#define snprintf_P(...)                     snprintf(__VA_ARGS__)
#define vsprintf_P(str, fmt, args)          vsprintf((str), (fmt), (args))
#define vsnprintf_P(str, size, fmt, args)   vsnprintf((str), (size), (fmt), (args))
#define fprintf_P(...)                      fprintf(__VA_ARGS__)
#define fputs_P(str, file)                  fputs((str), (file)) // Conflicts with SdFat
#define puts_P(str)                         puts((str))
#define vfscanf_P(file, fmt, args)          vfscanf((file), (fmt), (args))
#define fscanf_P(...)                       fscanf(__VA_ARGS__)
#define scanf_P(...)                        scanf(__VA_ARGS__)
#define sscanf_P(...)                       sscanf(__VA_ARGS__)

#endif // defined(ARDUINO_ARCH_AVR)


#ifndef MAX_OPEN_FILES
#define MAX_OPEN_FILES 100
#endif // MAX_OPEN_FILES

#endif // __STAIO_H__
