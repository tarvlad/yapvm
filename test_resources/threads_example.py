import threading

def dot_product_chunk(start, end, vector1, vector2, result, lock):
    chunk_result = 0
    for i in range(start, min(end, len(vector1))):
        chunk_result += vector1[i] * vector2[i]
    
    with lock:
        result[0] += chunk_result

def dot_product(vector1, vector2):    
    vector_size = len(vector1)
    if vector_size <= 512:
        total = 0
        for i in range(vector_size):
            total += vector1[i] * vector2[i]
        return total
    else:
        num_threads = 8
        chunk_size = vector_size // num_threads
        threads = list()
        result = list()
        result.append(0)
        lock = threading.Lock()
        
        for i in range(num_threads):
            start = i * chunk_size
            if i < num_threads - 1:
                end = (i + 1) * chunk_size
            else:
                end = vector_size
            thread = threading.Thread(dot_product_chunk, start, end, vector1, vector2, result, lock)  # Removed named keywords
            threads.append(thread)
            thread.start()

        for thread in threads:
            thread.join()
        
        return result[0]

# Example usage
vector1 = list()
vector2 = list()

# Populating the vectors without using direct list declarations
for _ in range(200):
    for i in range(1, 6):  # Equivalent to [1, 2, 3, 4, 5]
        vector1.append(i)
    for i in range(5, 0, -1):  # Equivalent to [5, 4, 3, 2, 1]
        vector2.append(i)

result = dot_product(vector1, vector2)
print(result)
