#include <stdio.h>

#include "jpeg.h"
#include "jpeg-6b/lowres.h"

img_t * shrink_img_1_svc(img_t * arg, struct svc_req * sr) {

	FILE* f = fopen("scratch.jpg", "w+");

	static img_t * i = (img_t*) malloc(sizeof(img_t));

	fwrite(arg->data, sizeof(char), arg->size, f);

	memset(arg->data, 0, arg->size);

	i->size = 100;
	i->data = (char *)malloc(sizeof(char) *( 100 * 100 + 50 ));

	change_res_JPEG_F(f, &i.data, &i->size);

	return &i;
}
