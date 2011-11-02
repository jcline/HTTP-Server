#include "include.h"

int shared_get(key_t key, size_t size) {
	int rt = shmget(key, size, IPC_CREAT | 
			S_IRUSR | S_IWUSR);
	if(rt == -1) {
		perror("shmget");
	}
	return rt;
}

struct shm_thread_t * shared_mmap(int id, size_t size) {
	void * rp = shmat(id, NULL, 0);
	if(rp == (void *)-1) {
		perror("shmat");
	}

	memset(rp, 0, size);
	return (struct shm_thread_t *) rp;
}

void shared_end(const void* shm) {
	if(shmdt(shm) == -1)
		perror("shmdt");
}
