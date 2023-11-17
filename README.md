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
The output of this on my virtual machine with 10 cores is as follows
```shell
Generation: 100,685 usec
Hash table base: 9,301,736 usec
  - 0 missing
Hash table v1: 15,589,953 usec
  - 0 missing
Hash table v2: 1,594,016 usec
  - 0 missing
```

## First Implementation
In the `hash_table_v1_add_entry` function, I added a single mutex to implement synchronization. To do this, I initialized the pthread_mutex_t lock in the struct definition of hash_table_v1. I then initialize the lock in the hash_table_v1_create() function. The function that lock protects is hash_table_v1_add_entry, in which I added lock and unlock calls and at the beginning and end of the function body, respectively. I destroy the lock in the hash table cleanup function called hash_table_v1_destroy. 

### Performance
Plese refer to the Running section on how to run the tester program. Again, this was the output for 10 threads and 100000 entries per thread
```shell
Generation: 100,685 usec
Hash table base: 9,301,736 usec
  - 0 missing
Hash table v1: 15,589,953 usec
  - 0 missing
Hash table v2: 1,594,016 usec
  - 0 missing
```
Version 1 is a little slower than the base version. As I put the lock around the entire hash_table_v1_add_entry function in Version 1, the functionality is essentially the same as the base version, and there is no multithreading occuring that would offer a speedup. However, since Version 1 has these locks at all, the added overhead from locking and unlocking at every call of hash_table_v1_add_entry made it slower than the base version.

## Second Implementation
In the `hash_table_v2_add_entry` function, I added one lock around the SLIST_INSERT_HEAD call. I deemed this was necessary as race conditions could occur without it since we are changing the head pointer of the LL, and inserting two new items at once before the head pointer is updated will cause an overwrite of the first written item. I deemed locking the get table entry call unneeded since it just calls the hash function and returns the right bucket for the key input. I also deemed locking the updating values and return section unecessary as race conditions don't matter - the value is getting overwritten anyway and the order threads operate in is arbitrary.

### Performance
To test performance, I ran the following command with changing cores, threads, and number of entries. 
```shell
./hash-table-tester -t numthreads -s numentries.
```
I changed the number of cores on my VM through settings.

With 4 cores:
default
```shell
Generation: 8,858 usec
Hash table base: 46,323 usec
  - 0 missing
Hash table v1: 80,045 usec
  - 0 missing
Hash table v2: 8,382 usec
  - 0 missing
```
high and weak performance satisfied
*there was a lot of variation in this test

medium number of elements (8 threads, 50000)
```shell
Generation: 38,421 usec
Hash table base: 515,533 usec
  - 0 missing
Hash table v1: 1,362,824 usec
  - 0 missing
Hash table v2: 193,503 usec
  - 0 missing
```
weak performance criteria satisfied

high number of elements (8 threads, 100000)
```shell
Generation: 81,868 usec
Hash table base: 4,472,920 usec
  - 0 missing
Hash table v1: 6,994,105 usec
  - 0 missing
Hash table v2: 1,051,018 usec
  - 0 missing
```
high and weak performance critera satisfied

With 8 cores:
default
```shell
Generation: 8,069 usec
Hash table base: 25,922 usec
  - 0 missing
Hash table v1: 100,168 usec
  - 0 missing
Hash table v2: 13,437 usec
  - 0 missing
```
there was a lot of variation in this test

medium number of elements (8 threads, 50000)
```shell
Generation: 39,571 usec
Hash table base: 715,187 usec
  - 0 missing
Hash table v1: 1,983,831 usec
  - 0 missing
Hash table v2: 188,796 usec
  - 0 missing
```

high number of elements (8 threads, 100000)
```shell
Generation: 72,769 usec
Hash table base: 5,614,345 usec
  - 0 missing
Hash table v1: 10,733,665 usec
  - 0 missing
Hash table v2: 963,884 usec
  - 0 missing
```
weak performance criteria passed

V2.1
With 10 cores:
default(4 threads, 25000 entries)
```shell
Generation: 8,075 usec
Hash table base: 35,389 usec
  - 0 missing
Hash table v1: 87,932 usec
  - 0 missing
Hash table v2: 27,981 usec
  - 0 missing
```

medium number of elements (8 threads, 50000 entries)
```shell
Generation: 38,669 usec
Hash table base: 857,056 usec
  - 0 missing
Hash table v1: 2,848,176 usec
  - 0 missing
Hash table v2: 412,992 usec
  - 0 missing
```

high number of elements (8 threads, 100000 entries)
```shell
Generation: 77,233 usec
Hash table base: 5,711,779 usec
  - 0 missing
Hash table v1: 12,477,464 usec
  - 0 missing
Hash table v2: 1,418,410 usec
  - 0 missing
```

## Cleaning up
To clean the generated binary files from the build, run the following command.
```shell
make clean
```