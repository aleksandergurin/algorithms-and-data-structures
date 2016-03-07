def countingsort(lst, start, stop):
    # [start..stop) 'stop' is excluded
    count = [0] * (stop - start)

    for i in lst:
        count[i - start] += 1

    for i in range(1, stop - start):
        count[i] += count[i - 1]

    res = [0] * len(lst)
    for i in reversed(lst):
        res[count[i - start] - 1] = i
        count[i - start] -= 1

    return res
