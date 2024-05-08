#import test_resources.yapvm_cpython_thread_support

def greet(n):
    print("Hello " + str(n))
    return


t = __yapvm_thread(greet, 42)
__yapvm_thread_join(t)