/* This is a light File Management System, Supports simple file transmission.
 *   			Copyright (C) 2015  Yang Zhang
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/
#include <errno.h>
#include <unistd.h>
#include "debug.h"
#include "common.h"
#include "err_handler.h"
#include "recv_request.h"


/* Decoding the source string src, and store results to the string points
 * by args;
void decode_args(char *src, char *args)
{
	while (1) {
		if (*src == '\0') {
			break;
		} else if (*src == ':' && *(src + 1) == ':') {
			*args++ = ':';
		} else if (*src == ':') {
			*args++ = ' ';
		} else {
			*args++ = *src;
		}

		src++;
	}
	args = 0;
} */


void recv_request_header(struct client_attr *attr)
{
	void *p;
	int n;
	unsigned long byte;

	p = &attr->req;
	byte = sizeof(attr->req);
	while ((n = read(attr->fd, p, byte)) < byte) {
		if (n < 0)		/* Error encountered */
			err_thread_exit(attr->fd, errno, "read");
		else if (n == 0)	/* End-of-file */
			err_thread_exit(attr->fd, 0, "client closed");

		p += n;
		byte -= byte;

	}

#ifdef __DEBUG__
	char *_point_to_cstring;		/* Reduce name conflicts */

	_point_to_cstring = cstring(attr->req.code);
	debug("receive header: %s (%#X), length=%d)", _point_to_cstring,
			attr->req.code, attr->req.len);
	free(_point_to_cstring);
#endif

}

void recv_request_data(struct client_attr *attr)
{
	void *p;
	int n, len;

	p = attr->data;
	len = attr->req.len;
	while (len > 0) {
		if ((n = read(attr->fd, p, len)) < 0)
			err_thread_exit(attr->fd, errno, "read");
		else if (n == 0)
			err_thread_exit(attr->fd, 0, "client closed");

		p += n;
		len -= n;
	}
	*(attr->data + attr->req.len) = 0;

/*	debug("receive data: (%s)", attr->data);	*/
}


