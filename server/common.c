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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "debug.h"
#include "err_handler.h"
#include "common.h"

static int security_checking(struct client_attr *attr, char *buf);
static void escape_space(char *buf);

/* Write n bytes data to the file descriptor which refered by fd. */
void writen(const int fd, const void *buf, unsigned int len)
{
	unsigned int n;

	while (len > 0) {
		n = write(fd, buf, len);
		if (n == -1)
			err_thread_exit(fd, errno, "write");

		len -= n;
		buf += n;
	}
}


/* Simple to resolve path according attr->rootdir and attr->cwd, return 0 on
 * success, or -1 on error(which corresponding request must have arguments,
 * but the length of request is 0). */
int resolve_path(struct client_attr *attr, char *path)
{
	int i;

	/* All commands which need at least one argument, like: cd mkdir rm ...;
	 * if "\ " does exist, that's always transmitted by the attr->data */
	escape_space(attr->data);

	/* Follow the requests must need arguments, so if not! return -1 */
	if ((attr->req.code == REQ_CD || attr->req.code == REQ_MKDIR ||
			attr->req.code == REQ_RM ||
			attr->req.code == REQ_DOWNLOAD ||
			attr->req.code == REQ_UPLOAD) &&
			(attr->req.len == 0)) {
		return -1;
	} else if (attr->req.len == 0)	{
		/* No arguments are specified, using the current directory */
		strcpy(path, attr->cwd);
	} else if (*attr->data == '/') {
		/* Argument is the abosolute path, ls /absolute/path */
		strcpy(path, attr->rootdir);
		strcat(path, attr->data);
	} else {
		/* /root/dir + /current/work/directory + subdir */				strcpy(path, attr->cwd);

		/* add '/' if necessary */
		if (*(path + strlen(path) - 1) != '/')
			strcat(path, "/");
		strcat(path, attr->data);

		/* ensure no '/' end with path */
		i = strlen(path) - 1;
		if (*(path + i) == '/')
			*(path + i) = 0;
	}

	return 0;
}



/* Return 0, if path resolving have done, and new path security_checking()
 * is okay, or return -1, if security_checking() failed.
 * Example:
 * 	/var/fms/test/a/../../
 *	step1: /var/fms/test/../
 *	step2: /var/fms/	*/
int depth_resolve_path(struct client_attr *attr, char *path)
{
	char tmp[PATH_MAX];
	char *p, *p2;
	char c1, c2;

	/* Now, path point to the address of relative path */
	path = path + strlen(attr->rootdir);
	while (1) {
		/* Following if statement showing three situations, them are:
		 * not ".." specified, "foo..", "foo/..bar" */
		if ((p = strstr(path, "..")) == NULL ||
				((p > path) && *(p - 1) != '/' )||
				(*(p + 2) != '/' && *(p + 2) != 0))
			break;

		*(p - 1) = 0;
		if ((p2 = strrchr(path, '/')) != NULL)
			*p2 = 0;

		strcpy(tmp, path);
		strcat(tmp, p + 2);
		strcpy(path, tmp);
	}

	/* Removing the character '.' in the path
	 * Example:
	 *	/foo/bar/.		REMOVE
	 *	./foo/bar 		REMOVE
	 *	/foo/./bar		REMOVE
	 *	/foo.bar		OK		*/
	p2 = path;
	while ((p = strchr(p2, '.')) != NULL) {
		c1 = *(p - 1);
		c2 = *(p + 1);

		if (c1 == '/' && c2 == '/') {
			*p = 0;
			strcpy(tmp, path);
			strcat(tmp, p + 2);
			strcpy(path, tmp);
		} else if (c1 == '/') {
			*(p - 1) = 0;
		} else if (c2 == '/') {
			strcpy(path, p + 2);
		}

		p2 = p + 1;
	}

	/* Relative path must start with '/' */
	if (*path == 0)
		strcat(path, "/");

	path = path - strlen(attr->rootdir);
	/* Security-checking failed */
	if (security_checking(attr, path) == -1)
		return -1;

	/* If not any string in attr->cwd, the security-checking is okay, but
	 * we need add root string "/" to the attr->cwd, ensure the mechanism
	 * work correctly that all depends on it */
//	if (strcmp(attr->rootdir, path) == 0) {
//		strcat(path, "/");
//	}

	debug("path: (%s)", path);
	return 0;
}




/* Return a pointer which are point to the path name on success, or NULL on
 * error */
char *get_download_file(struct client_attr *attr, char *pathname)
{
	char *p;


	/* Specified the path */
	if ((p = strrchr(attr->data, '/')) != NULL) {
		*p = 0;
		if (resolve_path(attr, pathname) == -1){
			/* SEND_ERR_TO_CLIENT(attr, RESP_DOWNLOAD_ERR,
			 	"Missing operand");	*/
			return NULL;
		}

		if (depth_resolve_path(attr, pathname) == -1) {
			/* SEND_ERR_TO_CLIENT(attr, RESP_DOWNLOAD_ERR,
			 	"Download failed");	*/
			return NULL;
		}

		*p = '/';
		strcat(pathname, p);
	} else {
		strcpy(pathname, attr->cwd);

		/* Normally, there is no '/' at the end of attr->cwd, except
		 * attr->cwd is a rootdir which are "rootdir/" */
		if (*(pathname + strlen(pathname) - 1) != '/')
			strcat(pathname, "/");
		strcat(pathname, attr->data);
	}

	debug("Download file: %s", pathname);
	return pathname;
}


/* About space ' ' separate, since we using space as default separate, If
 * we need input the space in the file name or path name, user should typed
 * backslash '\' first and append space ' ' second, forms like:'"foo\ bar",
 * the parsing result is "foo bar"*/
static void escape_space(char *buf)
{
	char *p, *p2;

	p2 = buf;
	while ((p = strstr(p2, "\\ ")) != NULL) {
		*p++ = 0;

		/* p point to space ' ' which after backslash '\' */
		strcat(buf, p);
		p2 = buf;
	}
}

/* Return 0, if new path is security, otherwise, or -1 returned */
static int security_checking(struct client_attr *attr, char *path)
{
	int i;
	struct stat sb;


	debug("security_checking: root=%s, path=%s", attr->rootdir, path);
	i = strlen(attr->rootdir);
	if (strncmp(attr->rootdir, path, i) != 0)
		return -1;

	/* ls command only support argument is directory, not files */
	if (attr->req.code == REQ_CD /*|| attr->req.code == REQ_LS*/)
		if (stat(path, &sb) != 0 || (sb.st_mode & S_IFMT) != S_IFDIR)
			return -1;

	return 0;
}


/* Return converted request or response string on success, or NULL if allocate
 * memory failure, The caller should free it after use finished */
char *cstring(unsigned int code)
{
/* Support variable arguments, don't warry about the performance; */
#define RETURN(str, ...) 					\
	do {							\
		char *s;					\
		while ((s = malloc(BUFSZ)) == NULL);		\
		sprintf(s, str, ##__VA_ARGS__);			\
		return s;					\
	} while (0)


	switch (code) {
	case REQ_AUTH:
		RETURN("REQ_AUTH");
	case REQ_LS:
		RETURN("REQ_LS");
	case REQ_CD:
		RETURN("REQ_CD");
	case REQ_PWD:
		RETURN("REQ_PWD");
	case REQ_MKDIR:
		RETURN("REQ_MKDIR");
	case REQ_UPLOAD:
		RETURN("REQ_UPLOAD");
	case REQ_DATA_FINISH:
		RETURN("REQ_DATA_FINISH");
	case REQ_DOWNLOAD:
		RETURN("REQ_DOWNLODA");
	case REQ_EXIT:
		RETURN("REQ_EXIT");
	case RESP_AUTH_OK:
		RETURN("RESP_AUTH_OK");
	case RESP_AUTH_ERR:
		RETURN("RESP_AUTH_FAIL");
	case RESP_LS:
		RETURN("RESP_LS");
	case RESP_LS_ERR:
		RETURN("RESP_LS_ERR");
	case RESP_CD:
		RETURN("RESP_CD");
	case RESP_CD_ERR:
		RETURN("RESP_CD_ERR");
	case RESP_PWD:
		RETURN("RESP_PWD");
	case RESP_DOWNLOAD:
		RETURN("RESP_DOWNLOAD");
	case RESP_DOWNLOAD_ERR:
		RETURN("RESP_DOWNLOAD_ERR");
	case RESP_DATA_FINISH:
		RETURN("RESP_DATA_FINISH");
	case RESP_UPLOAD:
		RETURN("RESP_UPLOAD");
	case RESP_UPLOAD_ERR:
		RETURN("RESP_UPLOAD_ERR");
	case RESP_EXIT:
		RETURN("RESP_EXIT");
	case RESP_SERVER_ERR:
		RETURN("RESP_SERVER_ERR");
	case RESP_MKDIR_ERR:
		RETURN("RESP_MKDIR_ERR");
	case RESP_MKDIR:
		RETURN("RESP_MKDIR");
	case RESP_RM_ERR:
		RETURN("RESP_RM_ERR");
	case RESP_RM:
		RETURN("RESP_RM");
	default:
		RETURN("UNKNOWN CODE: (%#X)", code);
	}
}
