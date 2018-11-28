# HW #5

- This code can be run by simply doing "make", which will compile both "main" and "mem", and running both of them at the same time.
Running "make clean" will remove "main" and "mem".

- For my virtual addressing, I did a one-to-one mapping of virtual addresses to physical addresses, so each process gets a total of 256 bytes.
This in turn means that a total of 32 int of size 4 bytes can be stored in physical memory for each process. 

- My N, the number of addresses for my first page table will be 8, and my M, the number of addresses per page of the second page table, will be 32. 
This means that I will have a total of 256 total virtual memory addresses possible for each process.

- When my "main" spawns a thread, it just creates a thread that runs the "thread_func" method, which simply loops infinitum until the thread
is terminated by the "kill" command, or "exit". I use a simillar approach in my "mem", where I continuously loop through waiting for a
"read" or "write" message to physical memory to be recieved.

- My code will print whenever it performs a successful operation, such as writing or reading from memory, cache, etc.

- My piping between memory and the prompting application is done through FIFO, having a seperate FIFO for read operations and one for writes.

- My caching works by having an array of structs which store two ints, the physical address and the value. I implement a sort of write through,
where I only write into cache on writes if a cache line is empty. 

- On reads I check cache, on hit print the int, on miss check simulated memory. If cache is empty, I write to cache after simulated memory returns. 
If it is full, I choose a random cache line to evict and do so, writing the new int and address to cache.
