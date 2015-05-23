#include <unistd.h>
#include <errno.h>
#include "debug.h"
#include "common.h"
#include "err_handler.h"
#include "send_response.h"


inline void send_response(struct client_attr *attr)
{
	send_response_header(attr);
	send_response_data(attr);
}


void send_response_header(struct client_attr *attr)
{
	if (write(attr->fd, &attr->resp, sizeof(attr->resp))
			!= sizeof(attr->resp))
		err_thread_exit(attr->fd, errno, "write");

#ifdef __DEBUG__
	char *_point_to_cstring;	/* reduce name conflicts */
	_point_to_cstring = cstring(attr->resp.code);
	debug("send header: %s (%#X), length=%ld", _point_to_cstring,
			attr->resp.code, attr->resp.len);
	free(_point_to_cstring);
#endif

}

void send_response_data(struct client_attr *attr)
{
	if (write(attr->fd, attr->data, attr->resp.len) != attr->resp.len)
		err_thread_exit(attr->fd, errno, "write");
	/*debug("send data: (%s)", attr->data);*/
}

