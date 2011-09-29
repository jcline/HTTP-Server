#ifndef LIST_H
#define LIST_H

#include "include.h"

struct node_t {
	// data should _always_ be self contained, that is, no other pointer should ever access it
	// while it is in the list
	void * restrict data;
	// next should never equal prev, if gcc is smart enough this may speed up list operations
	struct node_t * restrict next, * restrict prev;
	// size of data
	size_t size;
	// a label/name for this item
	char * restrict label;
	// misc info
	int misc;
};

struct list_t {
	struct node_t *head, *tail;
	size_t size, cache_size;
	void* restrict * cache;
	pthread_mutex_t global_lock;
	pthread_cond_t work;
};


void push_front(struct list_t* restrict list, void* restrict a, size_t s, 
    char* restrict label, int misc);
void push_back(struct list_t* restrict list, void* restrict a, size_t s,
    char* restrict label, int misc);

void* pop_front(struct list_t* restrict list);
void* pop_back(struct list_t* restrict list);
struct node_t* pop_front_n(struct list_t* restrict list);
struct node_t* pop_back_n(struct list_t* restrict list);

void destroy(struct list_t* restrict list);
void init(struct list_t* restrict list);

struct node_t* getval_n_l(struct list_t* restrict list, char * restrict label, size_t i);
struct node_t* getval_n(struct list_t* restrict list, size_t i);
void* getval(struct list_t* restrict list, size_t i);
void replace(struct list_t* restrict list, size_t i, void* restrict a, size_t s);


#endif
