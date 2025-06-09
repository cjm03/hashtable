// ht.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "prime.h"
#include "prime.c"
#include "ht.h"

static ht_item*
htNewItem(const char* k, const char* v)
{
    // allocate a chunk of memory the size of ht_item
    ht_item* i = malloc(sizeof(ht_item));

    // save a copy of the string `k` in key
    i->key = strdup(k);

    // save a copy of the string `v` in value
    i->value = strdup(v);
    return i;
}

ht_table*
htNewTable(void)
{

    // deprecated, resorts to htNewTableSized()
    return htNewTableSized(HT_INIT_BASESIZE);
}

static
ht_table* htNewTableSized(const int base_size)
{
    // allocate a chunk of memory the size of ht_table and assign it to pointer `ht`
    ht_table* ht = malloc(sizeof(ht_table));

    // assign the new table's base_size element with the value passed in the func call
    ht->base_size = base_size;

    // assign element size with the next prime number after base_size
    ht->size = nextPrime(ht->base_size);

    // assign count to 0
    ht->count = 0;

    // allocate ht->size memory objects and initialize them all to NULL for the items array
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));

    // return the new table
    return ht;
}

static void
htDelItem(ht_item* i)
{
    // given a pointer to a ht_item, free the memory allocated for both the key and the value,
    // and then deallocate the ht_item itself
    free(i->key);
    free(i->value);
    free(i);
}

void
htDelTable(ht_table* ht)
{
    // for every item stored in ht
    for (int i = 0; i < ht->size; i++) {

        // assign the item to pointer variable `item`
        ht_item* item = ht->items[i];

        // if the item holds data, call htDelItem to delete the data
        if (item != NULL) {
            htDelItem(item);
        }
    }

    // after iterating through each item, deallocate the array entirely, and then the table itself
    free(ht->items);
    free(ht);
}

static int
htHash(const char* s, const int a, const int m)
{
    // initialize long type variable hash with 0
    long hash = 0;

    // declare len_s constant integer type, assign with string len of s
    const int len_s = strlen(s);

    // until i reaches len_s size, add the result of `a` raised to the power of the result of subtracting
    // ((i + 1) * the ascii value of the character at s[i]) from len_s to the long variable `hash`, and 
    // then reassign it the result modulo `m`.
    for (int i = 0; i < len_s; i++) {
        hash += (long)pow(a, len_s - (i + 1)) * s[i];
        hash = hash % m;
    }
    // return hash type casted to int
    return (int)hash;
}

unsigned long
htSdbm(const char* s)
{
    unsigned long hash = 0;
    int c;
    while ((c = *s++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

static int
htGetHash(const char* s, const int num_buckets, const int attempt)
{
    // assign hashA the result of the hash function using string `s` and prime number `HT_PRIME1`
    const int hashA = htHash(s, HT_PRIME1, num_buckets);

    // assign hashB the result of the hash function using string `s` and prime number `HT_PRIME2`
    const int hashB = htHash(s, HT_PRIME2, num_buckets);

    // perform the adding of 1 to hashB, multiplying that by integer attempt, adding hashA, and then
    // return the result modulo integer num_buckets
    return (hashA + (attempt * (hashB + 1))) % num_buckets;
}

void
htInsert(ht_table* ht, const char* key, const char* value)
{

        // calculate load on table, upscale to avoid floats
    const int load = ht->count * 100 / ht->size;

        // if load has surpassed 0.7, resize up (make new larger table)
    if (load > 70) htResizeUp(ht);

        // create a new ht_item by passing key and value to htNewItem(), assign it to item
    ht_item* item = htNewItem(key, value);

    // initialize index with result of htGetHash() called with the new item's key, the current size
    // of the hash table, and 0 representing attempts
    int index = htGetHash(item->key, ht->size, 0);

    // declare cur_item a pointer to a ht_item, assign with the hash table's item located at the index
    // previously calculated with htGetHash().
    ht_item* cur_item = ht->items[index];

    // initialize i with 1, representing 1 attempt
    int i = 1;

    // while cur_item holds data
    while (cur_item != NULL) {

        // if cur_item wasn't recently deleted
        if (cur_item != &HT_DELETED_ITEM) {

            // compare cur_item's key to key being searched for, delete and return if matched
            if (strcmp(cur_item->key, key) == 0) {
                htDelItem(cur_item);
                ht->items[index] = item;
                return;
            }
        }

        // calculate new index after each attempt
        index = htGetHash(item->key, ht->size, i);

        // assign item at new index to cur_item to check for data, as per the while loop condition
        cur_item = ht->items[index];

        // increment i with each attempt
        i++;
    }

    // once empty item is found, call it by index and assign it the item
    ht->items[index] = item;
    // increment the count
    ht->count++;
}

char*
htSearch(ht_table* ht, const char* key)
{
    // initialize and assign index with result of htGetHash called with the key, ht size, and 0 attempts
    int index = htGetHash(key, ht->size, 0);

    // assign `item` pointer to ht_item with the item in the hash table at the previously calculated index
    ht_item* item = ht->items[index];

    // assign i with 1, representing 1 attempt
    int i = 1;

    // while item holds data...
    while (item != NULL) {

        // if item isnt recently deleted
        if (item != &HT_DELETED_ITEM) {

            // check if the key being searched for matches the key at the current index
            if (strcmp(item->key, key) == 0) {

                // return the matching value if found
                return item->value;
            }
        }

        // otherwise, calculate new index to get next item, and increment attempt counter
        index = htGetHash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }

    // return NULL if no match
    return NULL;
}

// declare ht_item variable `HT_DELETED_ITEM` and assign its key/value pair as both NULL
void
htDelete(ht_table* ht, const char* key)
{

    // calculate load on table, upscale to avoid floats
    const int load = ht->count * 100 / ht->size;

    // if load is below 0.1, resize down
    if (load < 10) htResizeDown(ht);

    // init index and assign result of htGetHash passed with the key, ht size, and 0 attempts
    int index = htGetHash(key, ht->size, 0);

    // init and assign ht_item pointer `item` with hash table item found at resulting index^
    ht_item* item = ht->items[index];

    // assign i with 1, representing first attempt
    int i = 1;

    // while item holds data
    while (item != NULL) {

        // if item isn't the most recently deleted item
        if (item != &HT_DELETED_ITEM) {

            // compare key being searched for with key stored in current item, delete if its a match
            if (strcmp(item->key, key) == 0) {
                htDelItem(item);
                ht->items[index] = &HT_DELETED_ITEM;
            }
        }
        // calculate new index
        index = htGetHash(key, ht->size, i);
        // store item at new index, increment attempt counter
        item = ht->items[index];
        i++;
    }
    // decrement the count after deletion
    ht->count--;
}

static void
htResize(ht_table* ht, const int base_size)
{

    // prevent tables from getting too small (sec vuln. potentially)
    if (base_size < HT_INIT_BASESIZE) {
        return;
    }

    // create new ht_table structure `new_ht` with the size passed in the function call
    ht_table* new_ht = htNewTableSized(base_size);

    // while indexing variable is less than size element
    for (int i = 0; i < ht->size; i++) {

        // temporarily assign item with the ht_table 's item stored at current index
        ht_item* item = ht->items[i];

        // if the item holds data and wasn't recently deleted, it may be copied over to the new table
        if (item != NULL && item != &HT_DELETED_ITEM) {
            htInsert(new_ht, item->key, item->value);
        }
    }

    // raise the table's base_size to match that of the temp table's, likewise for the count
    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    /* swap sequence to increase the size of ht to match the size of new_ht */
    const int tmpsize = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmpsize;

    /* declare temp array of ptrs to store items for swap sequence */
    ht_item** tmpitems = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmpitems;

    // delete the temp table
    htDelTable(new_ht);
}

static void
htResizeUp(ht_table* ht)
{
    // utilize htResize() to double the size of the table so as to not overload it
    const int newsize = ht->base_size * 2;
    htResize(ht, newsize);
}

static void
htResizeDown(ht_table* ht)
{
    // utilize htResize() to half the size of the table so as to not waste memory
    const int newsize = ht->base_size / 2;
    htResize(ht, newsize);
}

// + --------------------------------------------------------------------------------------------------------+
// |            main                                                                                         |
// + --------------------------------------------------------------------------------------------------------+

int main(void)
{
    // create a hash table, store it in `ht`
    ht_table* ht = htNewTable();

    // create and insert a ht_item key/value pair into `ht`
    htInsert(ht, "crab", "25364102");

    // attempt to obtain the key's value via htSearch(), store value as a string to be referenced by a char ptr
    char* searchFor = htSearch(ht, "crab");

    // display the value
    printf("%s\n", searchFor);
}

