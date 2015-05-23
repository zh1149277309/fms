#include <unistd.h>
#include <errno.h>
#include "debug.h"
#include "common.h"
#include "err_handler.h"
#include "send_request.h"



inline void send_request(struct server_attr *attr)
{
	send_request_header(attr);
	send_request_data(attr);
}


void send_request_header(struct server_attr *attr)
{

	if (write(attr->fd, &attr->req, sizeof(attr->req)) != sizeof(attr->req))
		err_exit(errno, "write");

#ifdef __DEBUG__
	char *_point_to_cstring;		/* Reduce name conflicts */

	_point_to_cstring = cstring(attr->req.code);
	debug("send header: %s (%#X), length=%ld)", _point_to_cstring,
			attr->req.code, attr->req.len);
	free(_point_to_cstring);
#endif

}

void send_request_data(struct server_attr *attr)
{
	if (write(attr->fd, attr->data, attr->req.len) != attr->req.len)
		err_exit(errno, "write");
/*	debug("send data: (%s)", attr->data);	*/
}




/* 	Encoding the source string src, and store it to the string pointed by args,
 * 	which separated by ':';
 * 	NOTE:
 *		string "::" for meaning ':' */

/*	NOTE:
 *		deprecated
void encode_args(char *src, char *args)
{
	while (1) {
		if (*src == '\0') {
			break;
		} else if (*src == ':') {
			*args++ = ':';
			*args++ = ':';
		} else {
			*args++ = *src;
		}

		src++;
	}
	args = 0;
} */
