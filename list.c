#include "list.h"

void* pop_front(struct list_t* restrict list) {
	struct node_t* ret = pop_front_n(list);
	if(ret) {
		void* ret_data = ret->data;
		free(ret->label);
		free(ret);
		return ret_data;
	}
	else
		return NULL;
}

struct node_t* pop_front_n(struct list_t* restrict list) {
	assert(list);
	pthread_mutex_lock(&(list->global_lock));

	while(!list->size) {
		pthread_cond_wait(&(list->work), &(list->global_lock));
	}

	struct node_t *restrict ret = list->head;

	switch(list->size) {
		case 0:
			break;
		case 1:
			list->tail = NULL;
			list->head = NULL;
			--list->size;
			break;
		default:
			list->head = ret->next;
			list->head->prev = NULL;
			--list->size;
			break;
	}

	pthread_mutex_unlock(&(list->global_lock));
	return ret;
}

void push_back(struct list_t* restrict list, void* restrict a, size_t s,
    char * restrict label, int misc) {
	assert(list);

	struct node_t *restrict x = (struct node_t*) malloc( sizeof(struct node_t) );
	assert(x);

	x->data = (void * restrict) malloc(sizeof(char)*s);
	assert(x->data);
	memcpy(x->data, a, s);

	x->size = s;
	x->misc = misc;
	if(label) {
		size_t s = strlen(label) + 1;
		x->label = (char* restrict) malloc(sizeof(char)*s);
		memcpy(x->label, label, s);
	}
	else
		x->label = NULL;

	x->next = NULL;

	// Take the lock as late as possible
	pthread_mutex_lock(&(list->global_lock));

	x->prev = list->tail;

	if(list->tail != NULL)
		list->tail->next = x;

	list->tail = x;

	if(!list->size) {
		list->head = list->tail;
		assert(list->head);
	}

	assert(list->tail);

	++list->size;

	pthread_mutex_unlock(&(list->global_lock));
	pthread_cond_signal(&(list->work));

}

void destroy(struct list_t* restrict list) {
	assert(list);
	assert(list->head);

	struct node_t *it = list->head,
								*tmp = NULL;
	for(; it != NULL; it = it->next) {
		free(tmp);
		if(it->data) {
			free(it->data);
		}
		tmp = it;
	}

	if(tmp)
		free(tmp);

}

void init(struct list_t* restrict list) {
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
	pthread_mutex_init(&(list->global_lock), NULL);
	pthread_cond_init(&(list->work), NULL);
}

struct node_t* getnode(struct list_t* restrict list, size_t i) {
	struct node_t *it;
	size_t j = 0;
	for(it = list->head; it != NULL; ++j) {
		if( i == j )  {
			if(it->data != NULL) {
				return it;
			}
			else
				--j;
		}
		it = it->next;
	}
	return NULL;
}

struct node_t* getval_n(struct list_t* restrict list, size_t i) {
	assert(list);
	assert(list->head);
	assert(i < list->size);

	if(i >= list->size) {
		return NULL;
	}

	struct node_t *ptr = getnode(list,i);

	return ptr;
}

