#define _GNU_SOURCE
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "debug.h"
#include "err_handler.h"
#include "common.h"

static void escape_space(char *buf);


/* Write n bytes data to the file descriptor which refered by fd. */
void writen(const int fd, const void *buf, unsigned int len)
{
	unsigned int n;

	while (len > 0) {
		n = write(fd, buf, len);
		if (n == -1)
			err_exit(errno, "write");
		len -= n;
		buf += n;
	}
}



char *get_upload_file(struct server_attr *attr, char *pathname)
{
	/*if ((resolve_path(attr, pathname) == -1) ||
			(depth_resolve_path(attr, pathname) == -1))
	*/
	if (depth_resolve_path(attr, pathname) == -1)
		return NULL;

	debug("Upload file: %s", pathname);
	return pathname;
}


int resolve_path(struct server_attr *attr, char *path)
{
	int i;
	char *cwd;

	escape_space(attr->data);
	cwd = get_current_dir_name();

	if (*attr->data == '/') {			/* ls /absolute/path */
		strcpy(path, attr->data);
	} else {
		strcpy(path, cwd);

		if (*(path + strlen(path) - 1) != '/')
			strcat(path, "/");
		strcat(path, attr->data);

		i = strlen(path) - 1;
		if (*(path + i) == '/')
			*(path + i) = 0;/* ensure no '/' end with path */
	}

	free(cwd);
	return 0;
}


/* Return 0, if path resolving have done, and new path security_checking()
 * is okay, or return -1, if security_checking() failed.
 * Example:
 * 	/var/fms/test/a/../../
 *	step1: /var/fms/test/../
 *	step2: /var/fms/	*/
int depth_resolve_path(struct server_attr *attr, char *path)
{
	char tmp[PATH_MAX];
	char *p, *p2;
	char c1, c2;


	while (1) {
		/* Following if statement showing three situations, them are:
		 * not ".." specified, "foo..", "foo/..bar" */
		if ((p = strstr(path, "..")) == NULL ||
				*(p - 1) != '/' ||
				(*(p + 2) != '/' && *(p + 2) != 0))
			break;

		*(p - 1) = 0;
		if ((p2 = strrchr(path, '/')) != NULL)
			*p2 = 0;

		strcpy(tmp, path);
		strcat(tmp, p + 2);
		strcpy(path, tmp);
	}

	/* Removeing the character '.' in the path
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

	debug("path: (%s)", path);
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
	case REQ_DOWNLOAD:
		RETURN("REQ_DOWNLODA");
	case REQ_EXIT:
		RETURN("REQ_EXIT");
	case REQ_DATA_FINISH:
		RETURN("REQ_DATA_FINISH");
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
