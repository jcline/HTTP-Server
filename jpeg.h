/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _JPEG_H_RPCGEN
#define _JPEG_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct img_t {
	char *data;
	u_int size;
};
typedef struct img_t img_t;

#define JPEG_SHRINK 0x31234567
#define JPEG_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define SHRINK_IMG 1
extern  img_t * shrink_img_1(img_t *, CLIENT *);
extern  img_t * shrink_img_1_svc(img_t *, struct svc_req *);
extern int jpeg_shrink_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define SHRINK_IMG 1
extern  img_t * shrink_img_1();
extern  img_t * shrink_img_1_svc();
extern int jpeg_shrink_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_img_t (XDR *, img_t*);

#else /* K&R C */
extern bool_t xdr_img_t ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_JPEG_H_RPCGEN */