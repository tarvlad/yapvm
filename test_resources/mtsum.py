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
n = 100000

t1 = __yapvm_thread(p, n)
if not threaded:
    __yapvm_thread_join(t1)

t2 = __yapvm_thread(p, n)
if not threaded:
    __yapvm_thread_join(t2)

t3 = __yapvm_thread(p, n)
if not threaded:
    __yapvm_thread_join(t3)

p(n)

if threaded:
    __yapvm_thread_join(t1)
    __yapvm_thread_join(t2)
    __yapvm_thread_join(t3)
