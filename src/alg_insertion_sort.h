#ifndef ALG_INSERTION_SORT_H
#define ALG_INSERTION_SORT_H

#include <stddef.h>

/*
 * Implementation of insertion sort algorithm.
 *
 * Performance (both average and worst): O(n^2)
 *
 * base     - pointer to the array that need to be sorted
 * nmemb    - number of elemnets in array
 * size     - size of an element
 * cmp_func - comparison function
 *
 * Comparison function should return value less than, equal to, or greater than
 * zero if the first argument is respectively less than, equal to, or greater
 * than the second.
 *
 * Function return 0 if successful; -1 otherwise.
 */
int alg_insertion_sort(void *base, size_t nmemb, size_t size,
        int (*cmp_func)(const void *, const void *));

#endif  /* ALG_INSERTION_SORT */