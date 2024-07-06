#ifndef LIB_DEBUG_H_
#define LIB_DEBUG_H_

#include <stdio.h>

#include "lib_config.h"

#include "color.h"

#ifdef DEBUG_ON
#define PRINT_BYTE(n)    (void)fprintf(stderr, ">>> %x <<<\n", n);
#define PRINT_INT(n)     (void)fprintf(stderr, ">>> %d %s <<<\n", n, #n);
#define PRINT_UINT(n)    (void)fprintf(stderr, ">>> %u <<<\n", n);
#define PRINT_LONG(n)    (void)fprintf(stderr, ">>> %ld <<<\n", n);
#define PRINT_ULONG(n)   (void)fprintf(stderr, ">>> %lu <<<\n", n);
#define PRINT_ULX(n)     (void)fprintf(stderr, ">>> %lX <<<\n", n);
#define PRINT_DOUBLE(n)  (void)fprintf(stderr, ">>> %lf <<<\n", n);
#define PRINT_FLOAT(n)   (void)fprintf(stderr, ">>> %d <<<\n", n);
#define PRINT_POINTER(p) (void)fprintf(stderr, ">>> %s: %p <<<\n", #p, p);
#define PRINT_CHAR(n)    (void)fprintf(stderr, ">>> %c <<<\n", n);
#define PRINT_SIZE(n)    (void)fprintf(stderr, ">>> %lu <<<\n", n);
#else
#define PRINT_BYTE(n)   ;
#define PRINT_INT(n)    ;
#define PRINT_UINT(n)   ;
#define PRINT_LONG(n)   ;
#define PRINT_ULONG(n)  ;
#define PRINT_ULX(n)    ;
#define PRINT_DOUBLE(n) ;
#define PRINT_FLOAT(n)  ;
#endif // DEBUG_ON

#ifdef SINGLE_DOLLAR_ON
#define $         (void)fprintf(stderr, BOLD MAGENTA ">>> %s(%d) %s\n"  RESET,\
                         __FILE__, __LINE__, __PRETTY_FUNCTION__);
#else
#define $ ;
#endif // SINGLE_DOLLAR_ON

#ifdef DOUBLE_DOLLAR_ON
#define $$(...) { (void)fprintf(stderr,                                       \
                                BOLD GREEN"  [ %s ][ %d ][ %s ][ %s ]\n" RESET,\
                                __FILE__,                                     \
                                __LINE__,                                     \
                                __PRETTY_FUNCTION__,                          \
                                #__VA_ARGS__);                                \
                  __VA_ARGS__; }
#else
#define $$(...) { __VA_ARGS__; }
#endif // DOUBLE_DOLLAR_ON

#ifdef TRIPLE_DOLLAR_ON
#define $$$       (void)fprintf(stderr,                                       \
                                BOLD BLUE "{ %s }{ %d }{ %s }\n" RESET,     \
                                __PRETTY_FUNCTION__,                          \
                                __LINE__,                                     \
                                __FILE__);
#else
#define $$$ ;
#endif // TRIPLE_DOLLAR_ON

#endif // LIB_DEBUG_H_
