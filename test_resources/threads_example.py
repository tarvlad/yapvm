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
        # For small vectors, compute dot product directly
        return sum(vector1[i] * vector2[i] for i in range(vector_size))
    else:
        # For large vectors, use multithreading
        num_threads = 8  # Number of threads to use
        chunk_size = vector_size // num_threads
        threads = []
        result = [0]  # Use a list to hold the result so it can be modified by threads
        lock = threading.Lock()  # Lock for updating the result safely
        
        for i in range(num_threads):
            start = i * chunk_size
            end = (i + 1) * chunk_size if i < num_threads - 1 else vector_size
            thread = threading.Thread(target=dot_product_chunk, args=(start, end, vector1, vector2, result, lock))
            threads.append(thread)
            thread.start()
        
        # Wait for all threads to complete
        for thread in threads:
            thread.join()
        
        return result[0]

# Example usage
vector1 = [1, 2, 3, 4, 5] * 200  # Example vector (size 1000)
vector2 = [5, 4, 3, 2, 1] * 200  # Example vector (size 1000)

result = dot_product(vector1, vector2)
print(result)