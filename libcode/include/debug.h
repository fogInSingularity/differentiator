#ifndef LIB_DEBUG_H_
#define LIB_DEBUG_H_

#include <stdio.h>

#include "lib_config.h"

#include "color.h"

#ifdef DEBUG_ON
#define PRINT_BYTE(n)    fprintf(stderr, ">>> %x <<<\n", n);
#define PRINT_INT(n)     fprintf(stderr, ">>> %d <<<\n", n);
#define PRINT_UINT(n)    fprintf(stderr, ">>> %u <<<\n", n);
#define PRINT_LONG(n)    fprintf(stderr, ">>> %ld <<<\n", n);
#define PRINT_ULONG(n)   fprintf(stderr, ">>> %lu <<<\n", n);
#define PRINT_ULX(n)     fprintf(stderr, ">>> %lX <<<\n", n);
#define PRINT_DOUBLE(n)  fprintf(stderr, ">>> %lf <<<\n", n);
#define PRINT_FLOAT(n)   fprintf(stderr, ">>> %d <<<\n", n);
#define PRINT_POINTER(p) fprintf(stderr, "%s: %p\n", #p, p);

#define $         fprintf(stderr, BOLD MAGENTA ">>> %s(%d) %s\n"  RESET,    \
                         __FILE__, __LINE__, __PRETTY_FUNCTION__);
#define $$(...) { fprintf(stderr, BOLD MAGENTA ">>> %s\n" RESET,            \
                          #__VA_ARGS__);                                      \
                  __VA_ARGS__; }
#else
#define PRINT_BYTE(n)   ;
#define PRINT_INT(n)    ;
#define PRINT_UINT(n)   ;
#define PRINT_LONG(n)   ;
#define PRINT_ULONG(n)  ;
#define PRINT_ULX(n)    ;
#define PRINT_DOUBLE(n) ;
#define PRINT_FLOAT(n)  ;

#define $ ;
#define $$(...) { __VA_ARGS__; }
#endif // DEBUG_ON

#endif // LIB_DEBUG_H_
