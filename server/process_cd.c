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
#include <string.h>
#include "debug.h"
#include "err_handler.h"
#include "common.h"				/* security_checking(), depth_resolve_path() */
#include "send_response.h"
#include "process_cd.h"



/* Implement:
 * Since chdir() will change the same directory between other threads;
 * so I can't using default functions of Operating System's path resolution.
 * When client need to executes 'ls', 'upload', or 'download'...etc command,
 * It will using absolute path to do them jobs internally */
void process_cd(struct client_attr *attr)
{
	char path[PATH_MAX];

	/* If no argument, sending error message to client */
	if (resolve_path(attr, path) == -1){
		SEND_ERR_TO_CLIENT(attr, RESP_CD_ERR, "Missing operand");
		return;
	}

	/* Resolve the "." or ".." on the path */
	if (depth_resolve_path(attr, path) == -1) {
		SEND_ERR_TO_CLIENT(attr, RESP_CD_ERR, "Change directory failed");
		return;
	}


	strcpy(attr->cwd, path);
	strcpy(attr->data, EXCLROOTDIR(attr->cwd, attr->rootdir));
	SEND_DATA_TO_CLIENT(attr, RESP_CD);

}
