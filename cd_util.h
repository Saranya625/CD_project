#ifndef CD_UTIL_H
#define CD_UTIL_H

#include <stdlib.h>
#include <string.h>

static inline char *cd_strdup(const char *s)
{
    if (!s) {
        return NULL;
    }
    size_t n = strlen(s) + 1;
    char *p = (char *) malloc(n);
    if (p) {
        memcpy(p, s, n);
    }
    return p;
}

#ifndef strdup
#define strdup cd_strdup
#endif

#endif
