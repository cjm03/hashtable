// ht.h
#ifndef HT_H
#define HT_H
#include <stdio.h>


/**********************************************************************************************/


/**************/
/*** DEFINE ***/
/**************/
#define HT_PRIME1 691
#define HT_PRIME2 811
#define HT_INIT_BASESIZE 50


/**********************************************************************************************/


/******************/
/*** STRUCTURES ***/
/******************/
typedef struct {
    char* key;          // identifier
    char* value;        // value obtained via identifier
} ht_item;
typedef struct {
    int base_size;      // initial size at declaration
    int size;           // maximum amount of items that can be stored
    int count;          // number of items stored in the table
    ht_item** items;    // array of pointers to ht_item 's
} ht_table;


/**********************************************************************************************/


/******************/
/*** PROTOTYPES ***/
/******************/
static ht_item* htNewItem(const char* k, const char* v);
ht_table* htNewTable(void);
static ht_table* htNewTableSized(const int base_size);
static void htDelItem(ht_item* i);
void htDelTable(ht_table* ht);
static int htHash(const char* s, const int a, const int m);
static int htGetHash(const char* s, const int num_buckets, const int attempt);
void htInsert(ht_table* ht, const char* key, const char* value);
char* htSearch(ht_table* ht, const char* key);
void htDelete(ht_table* ht, const char* key);
static void htResize(ht_table* ht, const int base_size);
static void htResizeUp(ht_table* ht);
static void htResizeDown(ht_table* ht);


/**********************************************************************************************/


/********************************/
/*** DELETED ITEM DECLARATION ***/
/********************************/
static ht_item HT_DELETED_ITEM = {NULL, NULL};


/**********************************************************************************************/


#endif
