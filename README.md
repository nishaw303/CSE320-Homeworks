# Homework #4

[Go to Part 1](#part-1)  
[Go to Part 2](#part-2)  
[Go to Part 3](#part-3)  

Extra Credits Implemented:  
[Go to EC 2.1](#ec-21)  
[Go to EC 2.2](#ec-22)  

## Part 1

Very similar to my implementation from Homework 3, but with changes to how the filepath is parsed to be run and using `execvp` instead of `execve`.

The way the filepath is parsed is as follows:
- filepath is first isolated from the "run" command with strtok
- filepath is moved into parameter array
- if filepath does not contain and '/' characters, we use `realpath` to locate the file
- `execvp` is run on the file with arguments being added to the params array
	
The `SIGINT` signal is blocked in the parent but not the child, as we create out sigpromask that blocks `SIGINT` in the parent and when we have a child we remove the block. This means that the child can be interrupted but nor the parent.

## Part 2

Before we go into the functions, I'll go over all of the global variables in the header file.

The `timer` and `pid` are used in Part 3 so I'll wait until then to explain them.  
Next, we have our two structs that were defined in the documentation. I added an extra field to the `file_in_use` struct called `file_desc` to store the file descriptor to be returned in the fopen method.  
After that, we have the two arrays of structs to store the open files and memory pointers, with variables that will keep track of the number of open files and memory pointers.

We now have our `static pthread_mutex` so that our methods can be queued when in a multithreaded environment, as each cse320 method starts with a lock on the mutex and ends with an unlock.

Next, we have the linked list implementation, first with the `Node` struct and then with our `insert` method that simply adds the node to the head of the list.

#### cse320_malloc()

- The `malloc()` wrapper that I have implemented takes in the `size` of the block to be allocated. We then go into a lock on the global mutex called `lock`. After that, we check to see if we have 25 addresses stored in our array, and if we do, we set `errno = ENOMEM`, unlock `lock` and return with a value of -1. 

- If we have less than 25 in our array, we then declare a void pointer called `pointer` and assign it to the return value of `malloc(size)`. 

- After this, all we have to do is add the pointer to an available struct in the array, and we do that through our loop, incrementing `ref_count` and `num_addr` and breaking from the loop. After that, all we do is unlock `lock` and return the pointer.

#### cse320_free()

- Next, we have the `free()` wrapper, which takes in a void pointer called `ptr` for the memory to be freed. We first lock `lock`, then go into our loop to try and find the address that is being freed. If we find the address and it is not `NULL`, we can then check the `ref_count`.

- If `ref_count` is not zero, we decrement it, call `free(ptr)` to free the memory, unlock `lock` and return with 0.  
- If `ref_count` is zero, we are in a double free situation. We print our error message, assign `errno = EADDRNOTAVAIL`, unlock `lock`, and return with -1. 

- If we didn't find the address in our array, that means we have an illegal address, which will be handled outside of the loop at the end of our method, where we print the error message, assign `errno = EFAULT`, and return with -1.
	
#### cse320_fopen()

- We now have the `fopen()` wrapper, where again we take in the same args as the normal method, `filename` and `mode`. We lock `lock` and declare the file descriptor `file`. we then go into our loop to check to see if the file already has a pointer by comparing the filename to the `basename(filename)` which will just check the filename itself instead of any of the path data. 

- If we find a match and the `file_desc` is zero, we can reopen the file with `fopen(filename, mode)`. We then increment the `ref_count`, unlock `lock`, and return the file descriptor.

- If we didn't find a match, we then go into a loop to find the first available struct in our array. When we find one, we call `fopen(filename, mode)` to get a file descriptor, and check if it is not `NULL`.  
- If `file` is `NULL`, we set `errno = ENFILE`, print our error message, unlock `lock`, and return `NULL`.  
- If `file` is not null, we assign the filename to `basename(filename)`, set `file_desc` to `file`, increment `ref_count` and `num_file` and break from the loop. We then unlock `lock` and return `file`.

#### cse320_fclose()

- Onto the `fclose()` wrapper, which takes in a file descriptor called `stream`. Again we lock `lock`, then go into a loop to see if we can find `stream` in our array. If we do find it and it is not `NULL`, we check the `ref_count`.

- If `ref_count` is not zero, we decrement. If it is now zero we can call `fclose(stream)` to close the file descriptor, unlock `lock` and return with 0.  
- If `ref_count` is zero, we have a double close situation. We print our error message, set `errno = EINVAL`, unlock `lock` and return with -1.

- If we didn't find `file` that means we have an illegal filename. So we print our error message, set `errno = ENOENT`, unlock `lock` and return with -1.

#### cse320_clean()

- Finally, we have `cse320_clean()` which takes no args. All we do here is lock `lock`, go into a loop for each array of structs and check if `ref_count > 0`. If true, either call `free()` or `fclose()` and set `ref_count = 0`.

- Once we have checked every struct, we unlock `lock` and return with 0.

## Part 3

Now we can go over the two global variables at the beginning of the header file.

First, we have `timer`, which is the variable we will use to store the time in seconds for our reaping functionality. The default value for this timer is 5 seconds.  
Next, we have `pid`, which will be used to store the pid of the child process to be passed to our handler method for the `SIGALRM`.

#### cse320_fork()

- This is a simple wrapper for `fork()`. What it does is lock `lock`, calls `fork()` and stores the pid in `pid`. It checks if `pid != 0`, meaning it is the parent process. It then sets and `alarm` with length `timer`. If it is not the parent, it does nothing. In either case, it unlocks `lock` and returns the value of `pid`.

#### cse320_setttimer()

- This is a very simple method only taking one argument, `t` which is the time in seconds to change the timer to. All this method does is lock `lock`, set `timer` to `t`, unlock `lock` and return with 0.

#### cse320_handler()

- This is the handler for the `SIGALRM` signal. All it does is lock `lock`, check if `pid` is not 0, and if it isn't sends a `SIGKILL` signal to the process with pid equal to `pid`. After that, it unlocks `lock`.


## Extra Credits

#### EC 2.1

- To implement this extra credit, all I had to do was in `cse320_free()` in the for loop, after I find the address, was after I freed `ptr` to set `ref_count = 0`, set `addr = 0` and decrement `num_addr`. Doing this means that the struct in the array is not empty and can be used for another call of `cse320_malloc()` in the future.

#### EC 2.2

- The implementation for this extra credit was almost exactly the same as EC 2.1, the onyl difference was that instead of doing the clear every time the method was called, I had to check first if `ref_count = 0`, as it is possible to have multiple references to the same file descriptor. So in `cse320_fclose()` I just had to check if `ref_count = 0`, then close the file, set `filename` and `file_desc` to 0, decrement `num_file` and return.
