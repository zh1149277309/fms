#ifndef _COMMON_H
#define _COMMON_H
#include <malloc.h>		/* Need by cstring(), and it's caller need to free() */
#include "fmsclient.h"

char *cstring(unsigned int code);
char *get_upload_file(struct server_attr *attr, char *pathname);
int resolve_path(struct server_attr *attr, char *path);
int depth_resolve_path(struct server_attr *attr, char *path);

#endif
