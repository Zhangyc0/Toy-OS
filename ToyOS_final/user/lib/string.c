#include "syscall.h"
#include "string.h"
#include "stdio.h"

static inline long read(int fd, void* buf, int nbuf)
{
	long ret;
	asm volatile (	"li a7, %4\n"
					"mv a0, %3\n"
					"mv a1, %2\n"
					"mv a2, %1\n"
					"ecall\n"
					"mv %0, a0\n"
					: "+r" (ret)
					: "r" (nbuf), "r" (buf), "r" (fd),
					"i" (SYS_READ)
					);
	return ret;
}

char *gets(char *buf, int max) {
    int i, cc;
    char c;

    for (i = 0; i + 1 < max;) {
        cc = read(0, &c, 1);
        //printf("cc=%d\n",cc);
        //printf("c=%c\n",c);
        //printf("c=%d\n",c);
        if (cc == 0)
            continue;
        if (cc < 1)
            break;
        buf[i++] = c;
        //printf("%c",buf[i-1]);
        //printf("buf[%d]=%c\n",i-1,buf[i-1]);
        if (c == '\n' || c == '\r')
            break;
        else printf("%c",buf[i-1]);
    }
    buf[i - 1] = '\0';
    printf("\n");
    return buf;
}

void *memset(void *dst, int c, unsigned int n) {
    char *cdst = (char *)dst;
    unsigned int i;
    for (i = 0; i < n; i++) {
        cdst[i] = (char)c;
    }
    return dst;
}

int strcmp(const char *p, const char *q) {
    while (*p && *p == *q)
        p++, q++;
    return (unsigned char)*p - (unsigned char)*q;
}