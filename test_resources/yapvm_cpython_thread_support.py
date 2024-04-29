import threading


def __yapvm_thread(f, args):
    t = threading.Thread(None, f, "Thread", args)
    t.start()
    return t


def __yapvm_thread_join(thread):
    thread.join()
