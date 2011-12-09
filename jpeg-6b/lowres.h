/* Header file for the lowres functionality */

#ifndef _LOWRES_H
#define _LOWRES_H

#ifdef  __cplusplus
extern "C" {
#endif

int change_res_JPEG (int insocket, char ** output, int *output_size);

int
change_res_JPEG_F (FILE *infile, char ** output, int *output_size);

#ifdef  __cplusplus
}
#endif

#endif /* ifndef _LOWRES_H */
