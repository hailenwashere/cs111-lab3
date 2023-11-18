# Hash Hash Hash
A hashtable is a data structure used for quick data accessing by hashing the key we want to search with. There can be collisions if the hash function returns the same index of the array that is the hash table for two different keys; this is avoided in this assignment using a linked list to add the separate key value pair to the same index returned from the hash function. This assignment tasks us to make the add entry function of the described hash table thread-safe.

## Building
Run the following command to build the program
```shell
make
```

## Running
The tester program is run using the following command, where the -t option specifies the number of threads (default 4) and -s specificies the number of entries each thread will add (default 25000).
```shell
./hash-table-tester -t 10 -s 100000
```
The output of this on my local machine with 8 performance cores is as follows
```shell
Generation: 115,325 usec
Hash table base: 7,239,607 usec
  - 0 missing
Hash table v1: 8,342,351 usec
  - 0 missing
Hash table v2: 989,709 usec
  - 0 missing
```

## First Implementation
In the `hash_table_v1_add_entry` function, I added a single mutex to implement synchronization. To do this, I initialized the pthread_mutex_t lock in the struct definition of hash_table_v1. I then initialize the lock in the hash_table_v1_create() function. The function that lock protects is hash_table_v1_add_entry, in which I added lock and unlock calls and at the beginning and end of the function body, respectively. I destroy the lock in the hash table cleanup function called hash_table_v1_destroy. 

### Performance
Plese refer to the Running section on how to run the tester program. Again, this was the output for 10 threads and 100000 entries per thread
```shell
Generation: 115,325 usec
Hash table base: 7,239,607 usec
  - 0 missing
Hash table v1: 8,342,351 usec
  - 0 missing
Hash table v2: 989,709 usec
  - 0 missing
```
Version 1 is a little slower than the base version. As I put the lock around the entire hash_table_v1_add_entry function in Version 1, the functionality is essentially the same as the base version, and there is no multithreading occuring that would offer a speedup. However, since Version 1 has these locks at all, the added overhead from locking and unlocking at every call of hash_table_v1_add_entry made it slower than the base version. In short, every thread executed sequentially like the base version but with extra overhead. 

## Second Implementation
In the `hash_table_v2_add_entry` function, I added one lock around the SLIST_INSERT_HEAD call. I deemed this was necessary as race conditions could occur without it since we are changing the head pointer of the LL, and inserting two new items at once before the head pointer is updated will cause an overwrite of the first written item. I deemed locking the get table entry call unneeded since it just calls the hash function and returns the right bucket for the key input. I also deemed locking the updating values and return section unecessary as race conditions don't matter - the value is getting overwritten anyway and the order threads operate in is arbitrary. Also, instead of declaring the lock in the hash_table struct, I declare it in the hash_table_entry struct for better granularity and lock at the linked list level instead. 

### Performance
To test performance, I ran the following command with changing cores, threads, and number of entries. 
```shell
./hash-table-tester -t numthreads -s numentries.
```
With 4 cores: (run on lnxsrv15 SEASNet, 4 cores)
default
```shell
Generation: 17,273 usec
Hash table base: 53,839 usec
  - 0 missing
Hash table v1: 141,845 usec
  - 0 missing
Hash table v2: 16,850 usec
  - 0 missing
```
  this passes the high performance test (16850 <= 53839 / 3  = 17946)
  this passes the weak performance test (16850 <= 54639 / 2 = 26919)


medium number of elements (8 threads, 50000)
```shell
Generation: 68,773 usec
Hash table base: 1,086,678 usec
  - 0 missing
Hash table v1: 1,506,681 usec
  - 0 missing
Hash table v2: 330,478 usec
  - 0 missing
```
  this passes the high performance test (330478 <= 1086678 / 3 = 365559)
  this passes the weak performance test (330478 <= 1086678 / 2 = 548339)

high number of elements (8 threads, 100000)
```shell
Generation: 134,524 usec
Hash table base: 6,588,806 usec
  - 0 missing
Hash table v1: 8,212,784 usec
  - 0 missing
Hash table v2: 2,099,910 usec
  - 0 missing
```
  this passes the high performance test (2099910 <= 6588806 / 3 = 2196628)
  this passes the weak performance test (2099910 <= 6588806 / 2 = 3294403)

With 8 cores (run locally on Mac, 8 performance cores, 2 efficiency cores)
default
```shell
Generation: 19,057 usec
Hash table base: 31,138 usec
  - 0 missing
Hash table v1: 63,553 usec
  - 0 missing
Hash table v2: 6,629 usec
  - 0 missing
```
  this passes the weak performance test (6629 <= 31138 / 4 = 7784.5)

medium number of elements (8 threads, 50000)
```shell
Generation: 54,915 usec
Hash table base: 433,341 usec
  - 0 missing
Hash table v1: 833,311 usec
  - 0 missing
Hash table v2: 57,471 usec
  - 0 missing
```
  this passes the high performance test (57471 <=  433341 / 7 = 61906)
  this passes the weak peformance test (57471 <= 433341 / 4 = 108335)
high number of elements (8 threads, 100000)
```shell
Generation: 94,557 usec
Hash table base: 3,157,485 usec
  - 0 missing
Hash table v1: 3,948,371 usec
  - 0 missing
Hash table v2: 414,591 usec
  - 0 missing
```
   this passes the high performance test (414591 <= 3157486 / 7 = 451069)
   this passes the weak performance test (414591 <= 3157486 / 4 = 789371)

## Cleaning up
To clean the generated binary files from the build, run the following command.
```shell
make clean
```