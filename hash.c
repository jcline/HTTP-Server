#include "hash.h"

unsigned long hash(unsigned char * restrict str) {
	unsigned long hash = 5381;
	int c;

	while((c = *str++))
		hash = ((hash << 5) + hash) ^ c;

	return hash;
}
