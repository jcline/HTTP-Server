/* Just a part of the lowres functionality. It has to be in a separate
   file, otherwise the data destination manager routines would conflict
   with the ones used for decompression. Someone needs to write an
   object-oriented extension for C. ;-))) 
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"

/* YANNIS: This _fine_ library wants me to define a data destination
   manager just to be able to write compressed data in memory. */
typedef struct {
  struct jpeg_destination_mgr pub; /* public fields */

  size_t   total_size;
  JOCTET * output;        /* target stream */
} my_destination_mgr;

typedef my_destination_mgr * my_dest_ptr;

METHODDEF(void)
init_destination (j_compress_ptr cinfo)
{
  my_dest_ptr this_obj = (my_dest_ptr) cinfo->dest;
  
  this_obj->total_size = cinfo->image_width * cinfo->input_components * 
    cinfo->image_height;
  /* Probably too much space, but let's be conservative */
  this_obj->output = (JOCTET *) malloc(this_obj->total_size * sizeof(JOCTET));
  this_obj->pub.next_output_byte = this_obj->output;
  this_obj->pub.free_in_buffer = this_obj->total_size;
}

/* The following routine contributed by Ilya Bagrak (Spring'03) */
METHODDEF(boolean)
empty_output_buffer (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
  unsigned char* ret;

  ret = realloc(dest->output, dest->total_size + 1024);
  dest->output = ret;
  dest->total_size += 1024;
  dest->pub.next_output_byte = dest->output + dest->total_size - 1024;
  dest->pub.free_in_buffer = 1024;

  fprintf(stderr, "ASSERTION FAILURE\n");
  return TRUE;  /* This shouldn't happen */
}


/*
 * Terminate destination --- called by jpeg_finish_compress
 * after all data has been written.  Usually needs to flush buffer.
 */

METHODDEF(void)
term_destination (j_compress_ptr cinfo)
{
  return;   /* No need to do anything */
}


GLOBAL(void)
jpeg_mem_dest (j_compress_ptr cinfo)
{
  my_dest_ptr dest;

  if (cinfo->dest == NULL) {    /* first time for this JPEG object? */
    cinfo->dest = (struct jpeg_destination_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                           SIZEOF(my_destination_mgr));
  }

  dest = (my_dest_ptr) cinfo->dest;
  dest->pub.init_destination = init_destination;
  dest->pub.empty_output_buffer = empty_output_buffer;
  dest->pub.term_destination = term_destination;
}



GLOBAL(void)
write_JPEG_file (JSAMPARRAY buffer, int image_width, int image_height,
		 int color_comp, J_COLOR_SPACE color_space,
		 int quality, JOCTET **output, int *output_size)
{
  /* This struct contains the JPEG compression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   * It is possible to have several such structures, representing multiple
   * compression/decompression processes, in existence at once.  We refer
   * to any one struct (and its associated working data) as a "JPEG object".
   */
  struct jpeg_compress_struct cinfo;
  /* This struct represents a JPEG error handler.  It is declared separately
   * because applications often want to supply a specialized error handler
   * (see the second half of this file for an example).  But here we just
   * take the easy way out and use the standard error handler, which will
   * print a message on stderr and call exit() if compression fails.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct jpeg_error_mgr jerr;
  /* More stuff */
  JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
  int row_stride;		/* physical row width in image buffer */

  /* Step 1: allocate and initialize JPEG compression object */

  /* We have to set up the error handler first, in case the initialization
   * step fails.  (Unlikely, but it could happen if you are out of memory.)
   * This routine fills in the contents of struct jerr, and returns jerr's
   * address which we place into the link field in cinfo.
   */

  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(&cinfo);

  /* Step 2: set parameters for compression */

  /* First we supply a description of the input image.
   * Four fields of the cinfo struct must be filled in:
   */
  cinfo.image_width = image_width; 	/* image width and height, in pixels */
  cinfo.image_height = image_height;
  cinfo.input_components = color_comp;	/* # of color components per pixel */
  cinfo.in_color_space = color_space; 	/* colorspace of input image */

  /* Step 3: specify data destination (eg, a file) */

  jpeg_mem_dest(&cinfo);

  /* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */
  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */

  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   * To keep things simple, we pass one scanline per call; you can pass
   * more if you wish, though.
   */

  row_stride = image_width * 3;	/* JSAMPLEs per row in image_buffer */

  while (cinfo.next_scanline < cinfo.image_height) {
    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */
    row_pointer[0] = buffer[cinfo.next_scanline];

    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);

  {
    my_dest_ptr this_obj = (my_dest_ptr) cinfo.dest;
    *output = this_obj->output;
    *output_size = this_obj->total_size - this_obj->pub.free_in_buffer;
    
    /* We destroy that pointer so that it is not used to deallocate the
       output data */
    this_obj->output = NULL;
  }

  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);

  /* And we're done! */
}

#ifdef __cplusplus
}
#endif
