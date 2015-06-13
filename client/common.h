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


/* There are 40 space, x2 for overwrite the print info of last */
#define PRINT_PROGRESS_WS "                                      "

#define PRINT_PROGRESS(str, length, nread)			\
	do {							\
		printf("\r"PRINT_PROGRESS_WS PRINT_PROGRESS_WS);\
		printf("\r%-73s", str);				\
		printf("[%2.1f%%]", (nread / length) * 100);	\
		fflush(stdout);					\
	} while(0)

char *cstring(unsigned int code);
void writen(const int fd, const void *buf, unsigned int len);
char *get_upload_file(struct server_attr *attr, char *pathname);
int resolve_path(struct server_attr *attr, char *path);
int depth_resolve_path(struct server_attr *attr, char *path);

#endif
