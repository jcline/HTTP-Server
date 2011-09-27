#include "list.h"

struct timespec gettime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	struct timespec ts;

	ts.tv_sec = tv.tv_sec;
	ts.tv_nsec = tv.tv_usec * 1000;

	ts.tv_sec += 5; // How long to wait

	return ts;
}

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
		struct timespec ts = gettime();
		if (pthread_cond_timedwait(&(list->work), &(list->global_lock), &ts) == ETIMEDOUT) {
			return NULL;
		}
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
		struct timespec ts = gettime();
		if (pthread_cond_timedwait(&(list->work), &(list->global_lock), &ts) == ETIMEDOUT) {
			return NULL;
		}
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
	pthread_mutex_lock(&(list->global_lock));

	struct node_t *restrict x = (struct node_t*) malloc( sizeof(struct node_t) );
	assert(x);

	x->data = (void * restrict) malloc(sizeof(char)*s);
	memcpy(x->data, a, s);

	x->size = s;
	x->misc = misc;
	x->label = NULL;

	x->next = NULL;

	x->prev = list->tail;

	if(list->tail != NULL)
		list->tail->next = x;

	list->tail = x;

	if(!list->size)
		list->head = list->tail;

	++list->size;

	assert(list->head);
	assert(list->tail);

	if(list->size > 1)
		pthread_cond_broadcast(&(list->work));
	else
		pthread_cond_signal(&(list->work));

	pthread_mutex_unlock(&(list->global_lock));
}

void push_front(struct list_t* restrict list, void* restrict a, size_t s,
    char * restrict label, int misc) {
	assert(list);
	pthread_mutex_lock(&(list->global_lock));

	struct node_t *x = (struct node_t*) malloc( sizeof(struct node_t) );
	assert(x);

	x->data = (void * restrict) malloc(sizeof(char)*s);
	memcpy(x->data, a, s);

	x->size = s;
	x->misc = misc;
	x->label = NULL;

	x->prev = NULL;

	x->next = list->head;

	if(list->head != NULL)
		list->head->prev = x;

	list->head = x;
	if(!list->size)
		list->tail = list->head;

	++list->size;

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

void* getval(struct list_t* restrict list, size_t i) {
	assert(list);
	pthread_mutex_lock(&(list->global_lock));
	assert(list->head);
	assert(i < list->size);

	if(i >= list->size) {
		pthread_mutex_unlock(&(list->global_lock));
		return NULL;
	}

	struct node_t *it;
	size_t j = 0;

	for(it = list->head; it != NULL; ++j) {
		if( i == j )  {
			if( it->data ) {
				pthread_mutex_unlock(&(list->global_lock));
				return it->data;
			}
			else
				--j;
		}
		it = it->next;
	}

	pthread_mutex_unlock(&(list->global_lock));
	return NULL;
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

