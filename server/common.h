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
#include <malloc.h>		/* Need by cstring(), and it's caller need to free() */
#include "fmsserver.h"

void writen(const int fd, const void *buf, unsigned int len);
int resolve_path(struct client_attr *attr, char *path);
int depth_resolve_path(struct client_attr *attr, char *buf);
char *get_download_file(struct client_attr *attr, char *pathname);
char *cstring(unsigned int code);

#endif
