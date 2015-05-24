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
		if ((n = read(attr->fd, p, len)) == -1)
			err_exit(errno, "read");
		else if (n == 0)
			break;

		p += n;
		len -= n;
	}
	*(attr->data + attr->resp.len) = 0;
	debug("receive data: (%s)", attr->data);
}

