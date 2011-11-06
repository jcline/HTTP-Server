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

struct cache_t {
	struct node_t * node;
};


struct list_t {
	struct node_t *head, *tail;
	volatile size_t size;
	pthread_mutex_t global_lock;
	pthread_cond_t work;
	int stop;
};


void push_back(struct list_t* restrict list, void* restrict a, size_t s,
    char* restrict label, int misc);
struct node_t* pop_front_n_c(struct list_t* restrict list, int* pass);
void* pop_front_c(struct list_t* restrict list, int* pass);

void* pop_front(struct list_t* restrict list);
struct node_t* pop_front_n(struct list_t* restrict list);
struct node_t* pop_back_n(struct list_t* restrict list);

void destroy(struct list_t* restrict list);
void init(struct list_t* restrict list);

struct node_t* getval_n(struct list_t* restrict list, size_t i);

#endif
