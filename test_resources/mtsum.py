#from test_resources.yapvm_cpython_thread_support import __yapvm_thread, __yapvm_thread_join


def s(n):
    i = 0
    while i < n:
        i = i + 1
    return i


def p(n):
    print(str(s(n)))
    return


threaded = False
n = 1000000
n_threads = 16

i = 0
threads = list()

while i < n_threads:
    threads += __yapvm_thread(p, n)
    if not threaded:
        __yapvm_thread_join(threads[i])
    i = i + 1

p(n)

if threaded:
    i = 0
    while i < n_threads:
        __yapvm_thread_join(threads[i])
        i = i + 1
