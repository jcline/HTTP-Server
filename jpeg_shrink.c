#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "jpeg.h"
#include "jpeg-6b/lowres.h"

img_t * shrink_img_1_svc(img_t * arg, struct svc_req * sr) {

	printf("arg->size: %d\t%d\n", arg->size, strlen(arg->data+1));

	int f = -1;
	f = open("scratch.jpg", O_RDWR | O_CREAT | O_TRUNC);
	if(f == -1)
		perror("open");

	printf("%d\n", arg->size);
	if(write(f, arg->data, arg->size) == -1)
		perror("write");

	if(arg->data)
		memset(arg->data, 0, arg->size);
	else {
		close(f);
		remove("scratch.jpg");
		return arg;
	}

	arg->size = 100;

	change_res_JPEG(f, &arg->data, (int*) &arg->size);
	remove("scratch.jpg");

	return arg;
}
