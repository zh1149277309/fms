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
#define _DEFAULT_SOURCE
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>		/* lstat(), remoev symbol link itself */
#include <dirent.h>			/* opendir(), readdir(), closedir() */
#include <errno.h>
#include "auth.h"
#include "common.h"
#include "process_rm.h"


static int remove_recursive(char *path);


int process_rm(struct client_attr *attr)
{
	char path[PATH_MAX];
	struct stat sb;
	int flags = 0;
	int s = 0;


	if (get_root_privilege(attr) == -1) {	/* NO permissions */
		SEND_ERR_TO_CLIENT(attr, RESP_RM_ERR, "Permission denied");
		return -1;
	}

	/* Option 'f' for force to remove a directory */
	if (strncmp("-f ", attr->data, 3) == 0) {
#define	RM_DIR_FORCE 0x01
		flags |= RM_DIR_FORCE;
		strcpy(attr->data, attr->data + 2);		/* Does this works? */
	}

	if (resolve_path(attr, path) == -1)	{
		SEND_ERR_TO_CLIENT(attr, RESP_RM_ERR, "Missing operand");
		return -1;
	}

	if (depth_resolve_path(attr, path) == -1) {
		SEND_ERR_TO_CLIENT(attr, RESP_RM_ERR, "Invalid path");
		return -1;
	}

	if (lstat(path, &sb) == -1) {
		SEND_ERR_TO_CLIENT(attr, RESP_RM_ERR, "%s", strerror(errno));
		return -1;
	}

	/* Removed path is not directory, remove it directly */
	if ((S_IFMT & sb.st_mode) != S_IFDIR) {
		if (unlink(path) == -1) {
			SEND_ERR_TO_CLIENT(attr, RESP_RM_ERR, "%s", strerror(errno));
			return -1;
		}
	} else if (flags == RM_DIR_FORCE) {		/* DIRECTORY */
		if ((s = remove_recursive(path)) != 0) {
			SEND_ERR_TO_CLIENT(attr, RESP_RM_ERR, "%s", strerror(errno));
			return -1;
		}
	} else {
		if (rmdir(path) == -1) {
			SEND_ERR_TO_CLIENT(attr, RESP_RM_ERR, "%s", strerror(errno));
			return -1;
		}
	}

	attr->resp.len = 0;
	SEND_DATA_TO_CLIENT(attr, RESP_RM);

	return 0;
}


/* Force to remove all files and directories under the "path" */
static int remove_recursive(char *path)
{
	char tmp[PATH_MAX];
	DIR *dp;
	struct dirent entry, *result;

	errno = 0;
	if ((dp = opendir(path)) == NULL) {
		return errno;
	}

	while (readdir_r(dp, &entry, &result) == 0 && result) {
		if (strcmp(entry.d_name, ".") == 0 || strcmp(entry.d_name, "..") == 0)
			continue;

		/* Using absolute path to rm directories or files */
		strcpy(tmp, path);
		if (*(tmp + strlen(tmp) - 1) != '/')
			strcat(tmp, "/");
		strcat(tmp, entry.d_name);


		/* Remove all things under it directory first */
		if (entry.d_type == DT_DIR) {

			remove_recursive(tmp);
			errno = 0;
			if (rmdir(tmp) == -1)	/* Occur error */
				return errno;
		} else {
			errno = 0;
			if (unlink(tmp) == -1)
				return errno;
		}
	}

	/* Normally, there is no error, DOES IT WORKS? */
	while (closedir(dp) == -1);
	return 0;
}



