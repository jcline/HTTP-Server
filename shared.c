#include "include.h"

extern int errno;

int shared_manage(void** restrict ptr, int * restrict id, key_t key, size_t size) {
	int exist = 0;
	*id = shmget(key, size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	if(*id == -1) {
		if(errno == EEXIST)
			exist=1;
		else {
			perror("shmget");
			return -1;
		}
	}

	if(exist) {
		*id = shmget(key, size, IPC_CREAT | S_IRUSR | S_IWUSR);
		if(*id == -1) {
			perror("shmget");
			return -1;
		}
	}

	*ptr = shmat(*id, NULL, 0);
	if(ptr == (void*) -1) {
		perror("shmat");
		return -1;
	}

	if(!exist) 
		memset(*ptr, 0, size);

	return 0;
}

void shared_end(const void* shm) {
	if(shmdt(shm) == -1)
		perror("shmdt");
}
