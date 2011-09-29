#include "list.h"

void* pop_back(struct list_t* restrict list) {
	struct node_t* ret = pop_back_n(list);
	if(ret) {
		void* ret_data = ret->data;
		free(ret);
		return ret_data;
	}
	else
		return NULL;
}

struct node_t* pop_back_n(struct list_t* restrict list) {
	assert(list);
	pthread_mutex_lock(&(list->global_lock));

	while(!list->size) {
		pthread_cond_wait(&(list->work), &(list->global_lock));
	}

	struct node_t *restrict ret = list->tail;

	if(list->size > 1) {
		list->tail = list->tail->prev;
		list->tail->next = NULL;
		--list->size;
	}
	else if(list->size) {
		list->tail = NULL;
		list->head = NULL;
		list->size = 0;
	}

	assert(list->tail != ret);
	pthread_mutex_unlock(&(list->global_lock));
	return ret;
}

void* pop_front(struct list_t* restrict list) {
	struct node_t* ret = pop_front_n(list);
	if(ret) {
		void* ret_data = ret->data;
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

	if(list->size > 1) {
		list->head = list->head->next;
		list->head->prev = NULL;
		--list->size;
	}
	else if(list->size) {
		list->tail = NULL;
		list->head = NULL;
		list->size = 0;
	}

	assert(list->head != ret);
	pthread_mutex_unlock(&(list->global_lock));
	return ret;
}

void push_back(struct list_t* restrict list, void* restrict a, size_t s,
    char * restrict label, int misc) {
	assert(list);

	struct node_t *restrict x = (struct node_t*) malloc( sizeof(struct node_t) );
	assert(x);

	x->data = (void * restrict) malloc(sizeof(char)*s);
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

	if(!list->size)
		list->head = list->tail;

	++list->size;

	assert(list->head);
	assert(list->tail);

	// We want to make sure all the threads are awake when there's lot's of work to do
	if(list->size > 1)
		pthread_cond_broadcast(&(list->work));
	else
		pthread_cond_signal(&(list->work));

	pthread_mutex_unlock(&(list->global_lock));
}

void push_front(struct list_t* restrict list, void* restrict a, size_t s,
    char * restrict label, int misc) {
	assert(list);

	struct node_t *x = (struct node_t*) malloc( sizeof(struct node_t) );
	assert(x);

	x->data = (void * restrict) malloc(sizeof(char)*s);
	memcpy(x->data, a, s);

	x->size = s;
	x->misc = misc;
	if(label) {
		x->label = (char* restrict) malloc(sizeof(char)*strlen(label));
		memcpy(x->label, label, strlen(label));
	}
	else
		x->label = NULL;

	x->prev = NULL;

	// Take the lock as late as possible
	pthread_mutex_lock(&(list->global_lock));

	x->next = list->head;

	if(list->head != NULL)
		list->head->prev = x;

	list->head = x;
	if(!list->size)
		list->tail = list->head;

	++list->size;

	if(list->size > 1)
		pthread_cond_broadcast(&(list->work));
	else
		pthread_cond_signal(&(list->work));

	assert(list->head);
	assert(list->tail);

	pthread_mutex_unlock(&(list->global_lock));
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

	free((void*)list->cache);

}

void init(struct list_t* restrict list) {
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
	list->cache_size = 10;
	list->cache = (void ** restrict) malloc(sizeof(void*) * list->cache_size);
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
	pthread_mutex_lock(&(list->global_lock));
	assert(list->head);
	assert(i < list->size);

	if(i >= list->size) {
		pthread_mutex_unlock(&(list->global_lock));
		return NULL;
	}

	struct node_t *ptr = getnode(list,i);

	pthread_mutex_unlock(&(list->global_lock));
	return ptr;
}

struct node_t* getval_n_l(struct list_t* restrict list, char * restrict label, size_t i) {
	assert(list);
	assert(label);
	assert(i);
	pthread_mutex_lock(&(list->global_lock));
	assert(list->head);

	struct node_t *it;
	size_t j = 0;
	for(it = list->head; it != NULL; ++j) {
		if( !strncmp(it->label, label, i) )  {
			if(it->data != NULL) {
				pthread_mutex_unlock(&(list->global_lock));
				return it;
			}
			else
				--j;
		}
		it = it->next;
	}

	pthread_mutex_unlock(&(list->global_lock));
	return NULL;
}

// without knowing the size of what's returned, 
// this is largely useless
void* getval(struct list_t* restrict list, size_t i) {
	assert(list);
	pthread_mutex_lock(&(list->global_lock));
	assert(list->head);
	assert(i < list->size);

	if(i >= list->size) {
		pthread_mutex_unlock(&(list->global_lock));
		return NULL;
	}

	struct node_t *ptr = getnode(list,i);
	if(!ptr)
		return NULL;
	void* ret = ptr->data;

	pthread_mutex_unlock(&(list->global_lock));
	return ret;
}

void replace(struct list_t* restrict list, size_t i, void* restrict a, size_t s) {
	assert(list);
	pthread_mutex_lock(&(list->global_lock));
	assert(list->head);
	assert(a);
	assert(i < list->size);
	if(i >= list->size) {
		pthread_mutex_unlock(&(list->global_lock));
		return;
	}

	struct node_t *data = getnode(list, i);
	assert(data);
	free(data->data);
	data->data = malloc(sizeof(char)*s);
	memcpy(data->data, a, s);

	pthread_mutex_unlock(&(list->global_lock));
}

