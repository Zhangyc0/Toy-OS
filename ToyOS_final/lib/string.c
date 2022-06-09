#include "string.h"
#include "sbi.h"

void *memset(void *dst, int c, uint64 n) {
    char *cdst = (char *)dst;
    for (uint64 i = 0; i < n; ++i)
        cdst[i] = c;

    return dst;
}
int getc(void)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_GETCHAR, 0, 0, 0, 0, 0, 0, 0);

	return ret.error;
}
void *memmove(void *dst, const void *src, uint n)
{
    char *cdst = (char *)dst;
    char *csrc = (char *)src;
    for (uint64 i = 0; i < n; ++i)
        cdst[i] = csrc[i];

    return dst;
}