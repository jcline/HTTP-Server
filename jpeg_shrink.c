#include <stdio.h>

#include "jpeg.h"
#include "jpeg-6b/lowres.h"

img_t * shrink_img_1_svc(img_t * arg, struct svc_req * sr) {

	FILE* f = fopen("scratch.jpg", "w+");

	printf("%d\n", arg->size);
	fwrite(arg->data, sizeof(char), arg->size, f);

	memset(arg->data, 0, arg->size);

	arg->size = 100;

	change_res_JPEG_F(f, &arg->data, (int*) &arg->size);

	return arg;
}
