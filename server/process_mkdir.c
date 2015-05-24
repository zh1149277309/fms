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
#include <sys/stat.h>
#include "auth.h"
#include "common.h"
#include "process_mkdir.h"


void process_mkdir(struct client_attr *attr)
{
	char path[PATH_MAX];

	if (get_root_privilege(attr) == -1) {	/* No permissions */
		SEND_ERR_TO_CLIENT(attr, RESP_MKDIR_ERR, "Permission denied");
		return;
	}

	if (resolve_path(attr, path) == -1) {	/* No argument */
		SEND_ERR_TO_CLIENT(attr, RESP_MKDIR_ERR, "Missing operand");
		return;
	}


	/* resolve the "." or ".." on the path */
	if (depth_resolve_path(attr, path) == -1) {
		SEND_ERR_TO_CLIENT(attr, RESP_MKDIR_ERR, "fail to mkdir");
		return;
	}
#define DIR_MODE	0740
	if (mkdir(path, DIR_MODE) == -1) {
		SEND_ERR_TO_CLIENT(attr, RESP_MKDIR_ERR, "%s", strerror(errno));
		return;
	}

	/* Send null data to client, only confirm server's response */
	*attr->data = 0;
	SEND_DATA_TO_CLIENT(attr, RESP_MKDIR);
}
