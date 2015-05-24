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
#include <string.h>
#include "err_handler.h"
#include "process_pwd.h"


/* Process PWD request from the client, send current word directory to client.
 *	*/
void process_pwd(struct client_attr *attr)
{

	/* to ensure follow of the strstr() return correct result, the attr->cwd
	 * structure must be add the strict restriction to keep always include
	 * the attr->rootdir;
	 * that's what I've done in the security_checking() function */

	/*
	if ((p = strstr(attr->cwd, attr->rootdir) + strlen(attr->rootdir)) != NULL)
		strcpy(attr->data, p);
	else
		strcpy(attr->data, "/");	*/

	strcpy(attr->data, EXCLROOTDIR(attr->cwd, attr->rootdir));
	SEND_DATA_TO_CLIENT(attr, RESP_PWD);
}

