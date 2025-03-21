import threading
import time

counter = 0
num_threads = 10
target_count = 10000
increment_per_thread = target_count // num_threads

sense = True
barrier_count = 0

def sense_reversing_barrier():
    global sense
    global barrier_count
    barrier_count += 1
    
    if barrier_count == num_threads:
        sense = not sense
        barrier_count = 0

def increment_counter():
    global counter
    global sense

    for _ in range(increment_per_thread):
        counter += 1

    sense_reversing_barrier()

start_time = time.time()

threads = []
for _ in range(num_threads):
    thread = threading.Thread(target=increment_counter)
    threads.append(thread)

for thread in threads:
    thread.start()

for thread in threads:
    thread.join()

end_time = time.time()
elapsed_time = end_time - start_time

assert counter == target_count, f"Giá trị counter không đúng! Dự kiến là {target_count}, nhưng thực tế là {counter}"

print(f"Counter value with {num_threads} threads: {counter}")
print(f"Time taken: {elapsed_time:.6f} seconds")

