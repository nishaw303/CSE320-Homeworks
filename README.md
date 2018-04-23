# Homework #4

##### Main Homework  
[Go to Part 1](#part-1)  
[Go to Part 2](#part-2)  
[Go to Part 3](#part-3)  

##### Extra Credits:  
[Go to EC 1](#ec-1)  
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
After that, we have the two heads of the linked lists to store the open files and memory pointers, with variables that will keep track of the number of open files and memory pointers, and the heads of the freed and closed addresses and files.

We now have our `static pthread_mutex` so that our methods can be queued when in a multithreaded environment, as each cse320 method starts with a lock on the mutex and ends with an unlock.

Next, we have the linked list implementation, first with the `Node` struct , then with our `insert_node` method that simply adds the node to the head of the list, and finally the `remove_node` method to remove a node.

#### cse320_malloc()

- The `malloc()` wrapper that I have implemented takes in the `size` of the block to be allocated. We then go into a lock on the global mutex called `lock`.  

- We then declare a void pointer called `pointer` and assign it to the return value of `malloc(size)`. After that, we call malloc again to create our new struct to be placed in the linked list. If either malloc call returns NULL, we set `errno = ENOMEM`, unlock `lock` and return with a value of -1. 

- After this, all we have to do is assign the pointer to the field in the struct, set `ref_count` to 1, and increment `num_addr`. After that, all we do is add the node to the linked list, unlock `lock` and return the pointer.

- Note: we also do a loop to check in the rare case that the assigned pointer is the same as an existing pointer in our list of freed pointers. If it is, we remove that node from the list.

#### cse320_free()

- Next, we have the `free()` wrapper, which takes in a void pointer called `ptr` for the memory to be freed. We first lock `lock`, then go into our loop to try and find the address that is being freed and check if it has already been freed. If we find the address has already been freed, we are in a double free situation. We print our error message, assign `errno = EADDRNOTAVAIL`, unlock `lock`, and return with -1.

- Next we loop through our linked list to find the address to be freed if we find it, we can free the pointer, add the pointer to our freed address list, remove the ndoe from the linked list, decrement `num_addr`, unlock `lock` and return with 0.
- We iterate to the net node in the linked list with `node = node->next` and repeat.

- If we didn't find the address in our linked list, that means we have an illegal address, which will be handled outside of the loop at the end of our method, where we print the error message, assign `errno = EFAULT`, and return with -1.
	
#### cse320_fopen()

- We now have the `fopen()` wrapper, where again we take in the same args as the normal method, `filename` and `mode`. We lock `lock` and declare the file descriptor `file`. We then call malloc for our new struct for the linked list, and if it returns `NULL`, we print our error message, set `errno = ENFILE`, unlock `lock` and return with -1. 

- After this, we go into our loop to check to see if the file already has a pointer by comparing the filename to the `basename(filename)` which will just check the filename itself instead of any of the path data. 

- If we find a match we increment the `ref_count`, unlock `lock`, and return the file descriptor.

- If we didn't find a match, we check to see if the file esists, and if it does we call `fopen(filename, mode)` to get a file descriptor, and check if it is not `NULL`.  
- If `file` is does not exist, we set `errno = ENFILE`, print our error message, unlock `lock`, and return `NULL`.  
- If `file` exists, we assign the filename to `basename(filename)`, set `file_desc` to `file`, increment `ref_count` and `num_file` and break from the loop. We then unlock `lock` and return `file`.

- Note: we also do a loop to check in the rare case that the assigned file descriptor is the same as an existing file descriptor in our list of closed files. If it is, we remove that node from the list.

#### cse320_fclose()

- Onto the `fclose()` wrapper, which takes in a file descriptor called `stream`. Again we lock `lock`, then go into a loop to see if we can find `stream` in list of closed files. If we do find it that means we have a double close situation. We print our error message, set `errno = EINVAL`, unlock `lock` and return with -1.

- After that, we loop through our linked list to find our file descriptor. If `ref_count` is not zero, we decrement. If it is now zero we can call `fclose(stream)` to close the file descriptor, add the file descriptor to our list of closed files, remove the node from the linked list, unlock `lock` and return with 0.

- If we didn't find `file` that means we have an illegal filename. So we print our error message, set `errno = ENOENT`, unlock `lock` and return with -1.

#### cse320_clean()

- Finally, we have `cse320_clean()` which takes no args. All we do here is lock `lock` and go into a loop to visit ever node in our linked lists of addresses and files, adding the respective pointer or file descriptor to our list of freed and closed pointers/files, free the address or close the file, and free the memory used for the linked list node. We then move to the next node and repeat.

- Implementing `cse320_clean()` in this way is possible because we will have no nodes in our linked lists that have `ref_count > 0`, so we can just go to every node in order instead of searching.

- Once we have checked every node, we unlock `lock` and return with 0.

## Part 3

Now we can go over the two global variables at the beginning of the header file.

First, we have `timer`, which is the variable we will use to store the time in seconds for our reaping functionality. The default value for this timer is 5 seconds.  
Next, we have `pid`, which will be used to store the pid of the child process to be passed to our handler method for the `SIGALRM`.

#### cse320_fork()

- This is a simple wrapper for `fork()`. What it does is lock `lock`, calls `fork()` and stores the pid in `pid`. It checks if `pid != 0`, meaning it is the parent process. It then sets and `alarm` with length `timer`. If it is not the parent, it does nothing. In either case, it unlocks `lock` and returns the value of `pid`.

#### cse320_settimer()

- This is a very simple method only taking one argument, `t` which is the time in seconds to change the timer to. All this method does is lock `lock`, set `timer` to `t`, unlock `lock` and return with 0.

#### cse320_handler()

- This is the handler for the `SIGALRM` signal. All it does is lock `lock`, check if `pid` is not 0, and if it isn't sends a `SIGKILL` signal to the process with pid equal to `pid`. After that, it unlocks `lock`.


## Extra Credits

#### EC 1

- The implementation for this extra credit was reletively simple. What I had to do was create a struct called `Node` to be used for the linked list. Each node held a pointer to it's data and a pointer to the next node in the list. With this done, the only other things that needed to be done were to create methods to add nodes to the linked lists and to integrate the linked lists into the methods I had made for arrays. 

- The `insert_node` method takes in the head of the linked list and the data to be stored in the node. It calls `malloc()` to allocate memory to the new node, sets the `next` pointer to point to the previous head node, sets the `data` pointer to point to the data struct, and sets the head pointer to point to the new node.

- Once that was all done it was just a matter of changing the loops in the methods to work with linked lists instead of arrays.

#### EC 2.1

- To implement this extra credit, I made a new method for my linked lists called `remove_node`, and a list of freed addresses with the head label `Freed_head` so that whenever `cse320_free()` was called on an address, I would check to see if it exists in the freed list, and if it did return the double free error message. If it wasn't, I could find it in the linked list and add the address to the list of freed addresses and pass the node to my remove node method to remove it and free it from the linked list. 

- I also changed the `cse320_clean()` method to free all nodes left in the linked list after freeing their allocated data, as any node left in the list will have `ref_count < 0` by definition.

#### EC 2.2

- Similar to the previous extra credit, I use the `remove_node` method and a list of closed file descriptors with the head label `Closed_head` so that whenever `cse320_fclose()` was called on a file descriptor, I would check to see if it exists in the closed list, and if it did return the ref count zero error message. If it wasn't, I could find it in the linked list and, if the ref count was zero after decrementing it, add the file descriptor to the list of closed file descriptors and pass the node to my remove node method to remove it and free it from the linked list. 

- I also changed the `cse320_clean()` method to free all nodes left in the linked list after freeing their allocated data, as any node left in the list will have `ref_count < 0` by definition.  
