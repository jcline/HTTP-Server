#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "jpeg.h"
#include "jpeg-6b/lowres.h"

img_t * shrink_img_1_svc(img_t * arg, struct svc_req * sr) {

	int f = -1;
	f = open("scratch.jpg", O_RDWR);

	printf("%d\n", arg->size);
	write(f, arg->data, arg->size);

	memset(arg->data, 0, arg->size);

	arg->size = 100;

	change_res_JPEG(f, &arg->data, (int*) &arg->size);

	return arg;
}
