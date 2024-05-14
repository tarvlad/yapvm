#from test_resources.yapvm_cpython_thread_support import __yapvm_thread, __yapvm_thread_join


def s(n):
    s = 0
    i = 0
    while i < n:
        s = s * 10
        s = s + i
        i = i + 1
    return s


def p(v):
    print(str(v))
    return


t = __yapvm_thread(p, s(100000))
w = __yapvm_thread(p, s(100000))
p((s(100000)))
__yapvm_thread_join(t)
__yapvm_thread_join(w)