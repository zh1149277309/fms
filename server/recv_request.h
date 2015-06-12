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
#ifndef _RECV_REQUEST_H
#define _RECV_REQUEST_H
#include "fmsserver.h"

void recv_request_header(struct client_attr *attr);
void recv_request_data(struct client_attr *attr);
void decode_args(char *src, char *args);

/* Receive request from the client, */
static inline void recv_request(struct client_attr *attr)
{
/*	debug("reciving request header...");	*/
	recv_request_header(attr);
/*	debug("reciving request data...");	*/
	recv_request_data(attr);
}
#endif

