#ifndef _COMMON_H
#define _COMMON_H
#include <malloc.h>	/* Need by cstring(), and it's caller need to free() */
#include "fmsclient.h"

/* Set printable string of download file info */
#define SET_PROGRESS_DOWNLOAD(str, name, length)			\
	do {								\
		double size;						\
									\
		size = length;						\
		while (size > 1024) { size = size / 1024; }		\
		sprintf(str, "Download file: %s, size: %.3g%s ", name,	\
			size,						\
			((unsigned long)size == length) ? "Bytes" :	\
			((unsigned long)size == length >> 10) ? "KB" :	\
			((unsigned long)size == length >> 20) ? "MB" :	\
			((unsigned long)size == length >> 30) ? "GB" :	\
			"TB");						\
	} while (0)

/* Set printable string of download file info */
#define SET_PROGRESS_UPLOAD(str, name, length)				\
	do {								\
		double size;						\
									\
		size = length;						\
		while (size > 1024) { size = size / 1024; }		\
		sprintf(str, "Upload file: %s, size: %.3g%s ", name,	\
			size,						\
			((unsigned long)size == length) ? "Bytes" :	\
			((unsigned long)size == length >> 10) ? "KB" :	\
			((unsigned long)size == length >> 20) ? "MB" :	\
			((unsigned long)size == length >> 30) ? "GB" :	\
			"TB");						\
	} while (0)


/* There are 42 space, x2 for overwrite the print info of last */
#define PRINT_PROGRESS_WS "                                        "

#define PRINT_PROGRESS(str, length, nread)			\
	do {							\
		printf("\r"PRINT_PROGRESS_WS PRINT_PROGRESS_WS);\
		printf("\r%s", str);				\
		printf("[%2ld%%]", (nread * 100 / length));	\
		fflush(stdout);					\
	} while(0)

char *cstring(unsigned int code);
void writen(const int fd, const void *buf, unsigned int len);
char *get_upload_file(struct server_attr *attr, char *pathname);
int resolve_path(struct server_attr *attr, char *path);
int depth_resolve_path(struct server_attr *attr, char *path);

#endif
