# Hash Hash Hash
A hash

## Building
```shell
TODO
```

## Running
```shell
TODO how to run and results
```

## First Implementation
In the `hash_table_v1_add_entry` function, I added a single mutex to implement synchronization. To do this, I initialized the pthread_mutex_t lock in the struct definition of hash_table_v1. I then initialize the lock in the hash_table_v1_create() function. The function that lock protects is hash_table_v1_add_entry, in which I added lock and unlock calls and at the beginning and end of the function body, respectively. I destroy the lock in the hash table cleanup function called hash_table_v1_destroy. 

### Performance
```shell
TODO how to run and results
```
Version 1 is a little slower/faster than the base version. As TODO

## Second Implementation
In the `hash_table_v2_add_entry` function, I added 

### Performance
```shell
TODO how to run and results
```

TODO more results, speedup measurement, and analysis on v2

## Cleaning up
```shell
TODO how to clean
```