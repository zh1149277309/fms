#ifndef _COMMON_H
#define _COMMON_H
#include <malloc.h>		/* Need by cstring(), and it's caller need to free() */
#include "fmsserver.h"

void writen(const int fd, const void *buf, unsigned int len);
int resolve_path(struct client_attr *attr, char *path);
int depth_resolve_path(struct client_attr *attr, char *buf);
char *get_download_file(struct client_attr *attr, char *pathname);
char *cstring(unsigned int code);

#endif
