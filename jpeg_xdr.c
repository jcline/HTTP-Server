/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "jpeg.h"

bool_t
xdr_img_t (XDR *xdrs, img_t *objp)
{
	register int32_t *buf;

	 if (!xdr_array (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, ~0,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->suc))
		 return FALSE;
	return TRUE;
}
