#from test_resources.yapvm_cpython_thread_support import __yapvm_thread, __yapvm_thread_join

n = 1000000
num_workers = 4
batch_size = int(n / num_workers)

v1 = list()
v2 = list()

i = 0
while i < n:
    v1.append(i)
    v2.append(n - (i + 1))
    i = i + 1


def worker_job(args):
    left = args[0]
    right = args[1]
    ptr = args[2]
    border = args[3]
    results = args[4]
    worker_id = args[5]

    res = 0
    while ptr < border:
        res = res + left[ptr] * right[ptr]
        ptr = ptr + 1

    results[worker_id] = res
    return

workers = list()
results = list()

i = 0
while i < num_workers:
    results += 0
    i = i + 1

worker_idx = 0

while worker_idx < num_workers:
    args = list()
    args.append(v1)
    args.append(v2)
    args.append(batch_size * worker_idx)
    args.append(batch_size * (worker_idx + 1))
    args.append(results)
    args.append(worker_idx)
    workers.append(__yapvm_thread(worker_job, args))
    worker_idx = worker_idx + 1

print("Workers created, waiting... ")

worker_idx = 0
while worker_idx < num_workers:
    __yapvm_thread_join(workers[worker_idx])
    worker_idx = worker_idx + 1

i = 0
res = 0
while i < num_workers:
    res = res + results[i]
    i = i + 1

print(str(res))