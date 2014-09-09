#include <rtthread.h>

/* only for GNU GCC */
#if !(defined(__GNUC__) && defined(__x86_64__))
typedef unsigned int size_t;
#else
#include <stdio.h>
#endif

char * strncat(char *dst, const char *src, unsigned int c);

char *strncpy(char *dst, const char *src, unsigned int c);

int strcmp(const char *a, const char *b);

void *memset(void *dst, int val, unsigned int size);

int strncmp(const char *cs, const char *ct, size_t count);

int atoi(const char* s);

extern size_t strlen(const char*s);
extern void* memcpy( void* s1, void* s2, size_t n);
