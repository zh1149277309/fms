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
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdarg.h>
#include <errno.h>
#include "err_handler.h"

#define BUFSZ	1024
#define ERR_HANDLER_EXIT	1	/* Hidden the implementation */
#define ERR_HANDLER_NOEXIT	0

static void err_handler(int is_exit, const int err, char *fmt, va_list ap);



/* Close the file descriptor of which connceted to client first, and print
 * the error message if errno != 0, and addtional mssage, then, call the
 * pthread_exit() to exit to current thread */
void _err_thread_exit(int fd, const int err, char *fmt, ...)
{
	int ret;
	va_list ap;

	va_start(ap, fmt);
	err_handler(ERR_HANDLER_NOEXIT, err, fmt, ap);
	va_end(ap);

	if (fd >= 0) 			/* ensure fd is correct */
		close(fd);

	ret = 1;
	pthread_exit(&ret);		/* Return 1 */
}


/* Print the formatted message only, doesn't exit */
static void err_handler(int is_exit, const int err, char *fmt, va_list ap)
{
	char s[BUFSZ];

	*s = 0;

	strcat(s, fmt);
	if (err) {
		strcat(s, " [");
		strcat(s, strerror(err));
		strcat(s, "]");
	}
	strcat(s, "\n");
	vfprintf(stderr, s, ap);

	if (is_exit)
		exit(EXIT_FAILURE);
}

/* Print formatted message and string of errno, and abort ALL the program */
void _err_exit(const int err, char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_handler(ERR_HANDLER_EXIT, err, fmt, ap);
}

/* Print a message to console */
void _err_msg(const int err, char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_handler(ERR_HANDLER_NOEXIT, err, fmt, ap);
	va_end(ap);
}
