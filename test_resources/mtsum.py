#from test_resources.yapvm_cpython_thread_support import __yapvm_thread, __yapvm_thread_join


def s(n):
    i = 0
    while i < n:
        i = i + 1
    return i


def p(v):
    print(str(v))
    return


t = __yapvm_thread(p, s(100000))
#__yapvm_thread_join(t)
w = __yapvm_thread(p, s(100000))
#__yapvm_thread_join(w)
x = __yapvm_thread(p, s(100000))
#__yapvm_thread_join(x)
p((s(100000)))
__yapvm_thread_join(t)
__yapvm_thread_join(w)
__yapvm_thread_join(x)
