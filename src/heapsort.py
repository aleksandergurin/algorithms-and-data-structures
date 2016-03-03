def heapsort(lst):
    def heapify(heap, size, i):
        left = i * 2 + 1
        right = left + 1
        if left < size and heap[left] > heap[i]:
            largest = left
        else:
            largest = i
        if right < size and heap[right] > heap[largest]:
            largest = right
        if largest != i:
            heap[i], heap[largest] = heap[largest], heap[i]
            heapify(heap, size, largest)

    def build_max_heap(heap):
        size = len(heap)
        for i in range(size // 2 - 1, -1, -1):
            heapify(heap, size, i)

    build_max_heap(lst)
    for idx in range(len(lst) - 1, 0, -1):
        lst[idx], lst[0] = lst[0], lst[idx]
        heapify(lst, idx, 0)
    return lst
