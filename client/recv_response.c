#include <errno.h>
#include <unistd.h>
#include "debug.h"
#include "common.h"
#include "err_handler.h"
#include "recv_response.h"



inline void recv_response(struct server_attr *attr)
{
	recv_response_header(attr);
	recv_response_data(attr);
}



void recv_response_header(struct server_attr *attr)
{
	void *p;
	int n;
	unsigned long byte;
	
	p = &attr->resp;
	byte = sizeof(attr->resp);
	while ((n = read(attr->fd, p, byte)) < byte) {
		if (n < 0)			/* Error encountered */
			err_exit(errno, "read");
		else if (n == 0)	/* No data arrive */
			continue;		/* NEED IMPLEMENT TO SLEEP */
			
		p += n;
		byte -= byte;
	} 
	
#ifdef __DEBUG__
	char *_point_to_cstring;		/* Reduce name conflicts */
	
	_point_to_cstring = cstring(attr->resp.code);
	debug("receive header: %s (%#X), length=%ld)", _point_to_cstring,
			attr->resp.code, attr->resp.len);
	free(_point_to_cstring);
#endif

}



void recv_response_data(struct server_attr *attr)
{
	void *p;
	int n, len;

	p = attr->data;
	len = attr->resp.len;

	while (len > 0) {
		if ((n = read(attr->fd, p, len)) < 0)
			err_exit(errno, "read");
		else if (n == 0)
			continue;

		p += n;
		len -= n;
	}
	*(attr->data + attr->resp.len) = 0;
	debug("receive data: (%s)", attr->data);
}

