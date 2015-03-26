#include <stdlib.h>
#include <string.h>

#include "alg_insertion_sort.h"

int alg_insertion_sort(void *base, size_t nmemb, size_t size,
        int (*cmp_func)(const void *, const void *))
{
    if (nmemb < 2)
        return -1;

    void *buf = malloc(size);

    if (buf == NULL)
        return -1;

    for (void *i = base, *end = base + size * nmemb; i < end; i += size) {
        memcpy(buf, i, size);           // buf = A[i]

        void *j = i - size;
        while(base <= j && cmp_func(buf, j) < 0) {
            memmove(j + size, j, size); // A[j + 1] = A[j]
            j -= size;
        }

        memcpy(j + size, buf, size);    // A[j + 1] = buf
    }

    free(buf);
    return 0;
}