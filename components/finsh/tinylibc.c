#include "tinylibc.h"

char * strncat(char *dst, const char *src, unsigned int c) {
        char *dstx = dst;
        while (*(++dstx)) c--;
        while (*src && c>1) {
          *(dstx++) = *(src++);
          c--;
        }
        if (c>0) *dstx = 0;
        return dst;
}
char *strncpy(char *dst, const char *src, unsigned int c) {
        char *dstx = dst;
        while (*src && c>1) {
          *(dstx++) = *(src++);
          c--;
        }
        if (c>0) *dstx = 0;
        return dst;
}

int strcmp(const char *a, const char *b) {
        while (*a && *b) {
                if (*a != *b)
                        return *a - *b; // correct?
                a++;b++;
        }
        return *a - *b;
}


void *memset(void *dst, int val, unsigned int size) {
  unsigned char *d = (unsigned char*)dst;
  unsigned int i;
  for (i=0;i<size;i++)
    d[i]=(unsigned char)val;
  return dst;
}

int strncmp(const char *cs, const char *ct, size_t count)
{
    register signed char __res = 0;

    while (count)
    {
        if ((__res = *cs - *ct++) != 0 || !*cs++)
            break;
        count --;
    }

    return __res;
}

int atoi(const char* s)
{
	long int v=0;
	int sign=1;
	while ( *s == ' '  ||  (unsigned int)(*s - 9) < 5u) s++;
	switch (*s)
	{
	case '-':
		sign=-1;
	case '+':
		++s;
	}
	while ((unsigned int) (*s - '0') < 10u)
	{
		v=v*10+*s-'0';
		++s;
	}
	return sign==-1?-v:v;
}
