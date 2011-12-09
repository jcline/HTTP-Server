#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "jpeg.h"
#include "jpeg-6b/lowres.h"

img_t * shrink_img_1_svc(img_t * arg, struct svc_req * sr) {

	printf("arg->data.data_len: %d\t%d\n", arg->data.data_len, strlen(arg->data.data_val));

	int f = -1;
	f = open("scratch.jpg", O_RDWR | O_CREAT | O_TRUNC);
	if(f == -1)
		perror("open");

	printf("%d\n", arg->data.data_len);
	if(write(f, arg->data.data_val, arg->data.data_len) == -1)
		perror("write");

	if(arg->data.data_val)
		memset(arg->data.data_val, 0, arg->data.data_len);
	else {
		close(f);
		remove("scratch.jpg");
		return arg;
	}

	arg->data.data_len = 100;

	change_res_JPEG(f, &arg->data.data_val, (int*) &arg->data.data_len);
	remove("scratch.jpg");

	return arg;
}
