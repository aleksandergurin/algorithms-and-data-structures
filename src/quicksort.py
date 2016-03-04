def quicksort(lst):
    def partition(li, l, r):
        x = li[r]
        i = l - 1
        for j in range(l, r):
            if li[j] < x:
                i += 1
                li[i], li[j] = li[j], li[i]
        i += 1
        li[i], li[r] = li[r], li[i]
        return i

    def sort(li, l, r):
        if l < r:
            idx = partition(li, l, r)
            sort(li, l, idx - 1)
            sort(li, idx + 1, r)

    sort(lst, 0, len(lst) - 1)
    return lst
