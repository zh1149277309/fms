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
#include "send_response.h"


inline void send_response(struct client_attr *attr)
{
	send_response_header(attr);
	send_response_data(attr);
}


void send_response_header(struct client_attr *attr)
{
	/*if (write(attr->fd, &attr->resp, sizeof(attr->resp))
			!= sizeof(attr->resp)) {
		err_thread_exit(attr->fd, errno, "write");
	}*/
	
	writen(attr->fd, &attr->resp, sizeof(attr->resp));

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
	/*if (write(attr->fd, attr->data, attr->resp.len) != attr->resp.len) {
		err_thread_exit(attr->fd, errno, "write");
	}*/

	writen(attr->fd, attr->data, attr->resp.len);
	/*debug("send data: (%s)", attr->data);*/
}

