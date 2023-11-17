#include "hash-table-base.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include <pthread.h>

struct list_entry {
	const char *key;
	uint32_t value;
	SLIST_ENTRY(list_entry) pointers;
};

SLIST_HEAD(list_head, list_entry);

struct hash_table_entry {
	struct list_head list_head;
	pthread_mutex_t lock;
};

struct hash_table_v2 {
	// pthread_mutex_t lock;
	struct hash_table_entry entries[HASH_TABLE_CAPACITY];
};

struct hash_table_v2 *hash_table_v2_create()
{
	struct hash_table_v2 *hash_table = calloc(1, sizeof(struct hash_table_v2));
	assert(hash_table != NULL);
	for (size_t i = 0; i < HASH_TABLE_CAPACITY; ++i) {
		struct hash_table_entry *entry = &hash_table->entries[i];

		// V2.2
		int err = pthread_mutex_init(&entry->lock, NULL);
		if (err != 0)
		{
			exit(err);
		}

		SLIST_INIT(&entry->list_head);
	}

	// V2.1
	// int err = pthread_mutex_init(&hash_table->lock, NULL);
	// if (err != 0)
	// {
	// 	exit(err);
	// }
	
	return hash_table;
}

static struct hash_table_entry *get_hash_table_entry(struct hash_table_v2 *hash_table,
                                                     const char *key)
{
	assert(key != NULL);
	uint32_t index = bernstein_hash(key) % HASH_TABLE_CAPACITY;
	struct hash_table_entry *entry = &hash_table->entries[index];
	return entry;
}

static struct list_entry *get_list_entry(struct hash_table_v2 *hash_table,
                                         const char *key,
                                         struct list_head *list_head)
{
	assert(key != NULL);

	struct list_entry *entry = NULL;
	
	SLIST_FOREACH(entry, list_head, pointers) {
	  if (strcmp(entry->key, key) == 0) {
	    return entry;
	  }
	}
	return NULL;
}

bool hash_table_v2_contains(struct hash_table_v2 *hash_table,
                            const char *key)
{
	struct hash_table_entry *hash_table_entry = get_hash_table_entry(hash_table, key);
	struct list_head *list_head = &hash_table_entry->list_head;
	struct list_entry *list_entry = get_list_entry(hash_table, key, list_head);
	return list_entry != NULL;
}

void hash_table_v2_add_entry(struct hash_table_v2 *hash_table,
                             const char *key,
                             uint32_t value)
{
	// we don't need to lock these since we don't change these values - the hash table will always have the same
	// hash_table_entry's and they will each have a list_head
	struct hash_table_entry *hash_table_entry = get_hash_table_entry(hash_table, key); // get hash entry at the hashed key index 
	struct list_head *list_head = &hash_table_entry->list_head; // gets the head of the table entry LL
	struct list_entry *list_entry = get_list_entry(hash_table, key, list_head); // returns list entry w same key

	/* if key alr in LL of this hash table entry, update value*/ 
	if (list_entry != NULL) {
		list_entry->value = value;
		return;
	}

	// we don't need to lock these since this is just creating the new entry, not changing the hashtable
	list_entry = calloc(1, sizeof(struct list_entry)); //thread safe
	list_entry->key = key;
	list_entry->value = value;

	// we updated head ptr and change the hashtable here, so we need to lock this
	// so that the next insert_head does so w the new head ptr
	// V2.1
	// pthread_mutex_lock(&(hash_table->lock)); // added this
	// SLIST_INSERT_HEAD(list_head, list_entry, pointers);
	// pthread_mutex_unlock(&(hash_table->lock)); // added this

	// V2.2
	pthread_mutex_lock(&(hash_table_entry->lock)); // added this
	SLIST_INSERT_HEAD(list_head, list_entry, pointers);
	pthread_mutex_unlock(&(hash_table_entry->lock)); // added this
}

uint32_t hash_table_v2_get_value(struct hash_table_v2 *hash_table,
                                 const char *key)
{
	struct hash_table_entry *hash_table_entry = get_hash_table_entry(hash_table, key);
	struct list_head *list_head = &hash_table_entry->list_head;
	struct list_entry *list_entry = get_list_entry(hash_table, key, list_head);
	assert(list_entry != NULL);
	return list_entry->value;
}

void hash_table_v2_destroy(struct hash_table_v2 *hash_table)
{
	// V2.1
	// int err = pthread_mutex_destroy(&(hash_table->lock));
	// if (err != 0)
	// {
	// 	exit(err);
	// }

	for (size_t i = 0; i < HASH_TABLE_CAPACITY; ++i) {
		struct hash_table_entry *entry = &hash_table->entries[i];
		// V2.2
		int err = pthread_mutex_destroy(&(entry->lock));
		if (err != 0)
		{
			exit(err);
		}
		struct list_head *list_head = &entry->list_head;
		struct list_entry *list_entry = NULL;
		while (!SLIST_EMPTY(list_head)) {
			list_entry = SLIST_FIRST(list_head);
			SLIST_REMOVE_HEAD(list_head, pointers);
			free(list_entry);
		}
	}
	free(hash_table);
}
