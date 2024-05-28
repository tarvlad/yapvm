#from test_resources.yapvm_cpython_thread_support import __yapvm_thread, __yapvm_thread_join


def s(n):
    i = 0
    while i < n:
        i = i + 1
    return i


def p(n):
    print(str(s(n)))
    return


threaded = True
n_threads = 4
n = 100000

threads = list()

i = 0
while i != n_threads:
    threads += __yapvm_thread(p, n)
    if not threaded:
        __yapvm_thread_join(threads[i])
    i = i + 1

if threaded:
    i = 0
    while i != n_threads:
        __yapvm_thread_join(threads[i])
        i = i + 1