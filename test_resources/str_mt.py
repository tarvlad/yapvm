import threading


def __yapvm_thread(f, args):
    f(args)
    return 42


def __yapvm_thread_join(thread):
    pass


n = 50000000
num_tasks = 4
threaded = True

def worker_job(args):
    num = args[0]
    id_ = args[1]

    res = "str" * num
    print(str(num))
    results[id_] = res
    return

results = list()
i = 0
while i < num_tasks:
    results.append("")
    i = i + 1


if threaded:
    workers = list()
    worker_idx = 0

    while worker_idx < num_tasks:
        args = list()
        args.append(n)
        args.append(worker_idx)
        workers.append(__yapvm_thread(worker_job, args))
        worker_idx = worker_idx + 1


    worker_idx = 0
    while worker_idx < num_tasks:
        __yapvm_thread_join(workers[worker_idx])
        worker_idx = worker_idx + 1
else:
    i = 0
    while i < num_tasks:
        args = list()
        args.append(n)
        args.append(i)
        t = __yapvm_thread(worker_job, args)
        __yapvm_thread_join(t)
        i = i + 1