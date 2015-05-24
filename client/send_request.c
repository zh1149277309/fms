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

	/*if (write(attr->fd, &attr->req, sizeof(attr->req)) !=
			sizeof(attr->req))
		err_exit(errno, "write");
	*/

	writen(attr->fd, &attr->req, sizeof(attr->req));
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
	/*if (write(attr->fd, attr->data, attr->req.len) != attr->req.len)
		err_exit(errno, "write");
	*/
	writen(attr->fd, attr->data, attr->req.len);
	/*debug("send data: (%s)", attr->data);*/
}




/* Encoding the source string src, and store it to the string pointed by args,
 * which separated by ':';
 * NOTE:
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
