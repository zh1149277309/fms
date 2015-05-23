#define _GNU_SOURCE
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "debug.h"
#include "common.h"

static void escape_space(char *buf);


char *get_upload_file(struct server_attr *attr, char *pathname)
{
	if ((resolve_path(attr, pathname) == -1) ||
			(depth_resolve_path(attr, pathname) == -1))
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


int depth_resolve_path(struct server_attr *attr, char *path)
{
	char tmp[PATH_MAX];
	char *p, *p2;
	char c1, c2;

	strcpy(tmp, path);
	while ((p = strstr(path, "..")) != NULL) {
		*(p - 1) = 0;
		if ((p2 = strrchr(path, '/')) != NULL)
			*p2 = 0;
		else
			break;

		strcpy(tmp, path);
		strcat(tmp, p + 2);
		strcpy(path, tmp);
	}

	p2 = path;
	while ((p = strchr(p2, '.')) != NULL) {
		c1 = *(p - 1);
		c2 = *(p + 1);

		if (( c1>= 'A' && c2 <= 'Z') || (c1 >= 'a' && c2 <= 'z')) {
			p2 = p + 1;
		} if (c1 == '/' && c2 == '/') {
			*(p - 1) = 0;
			strcpy(tmp, path);
			strcat(tmp, p + 2);
			strcpy(path, tmp);
		} else if (c1 == '/') {
			*(p - 1) = 0;
		} else if (c2 == '/') {
			strcpy(tmp, p + 2);
			strcpy(path, tmp);
		}
	}

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
