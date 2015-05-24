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
#ifndef _ERR_HANDLER_H
#define _ERR_HANDLER_H

/* I wish to print the file name and its line numbers when problem occur */
#define err_thread_exit(fd, err, fmt, ...)	\
	_err_thread_exit(fd, err, "["__FILE__",%d] "fmt, __LINE__, ##__VA_ARGS__)
#define err_msg(err, fmt, ...)				\
	_err_msg(err, "["__FILE__",%d] "fmt, __LINE__, ##__VA_ARGS__)
#define err_exit(err, fmt, ...)				\
	_err_exit(err, "["__FILE__",%d] "fmt, __LINE__, ##__VA_ARGS__)


void _err_thread_exit(int fd, const int err, char *fmt, ...)
		__attribute__((noreturn));
void _err_msg(const int err, char *fmt, ...);
void _err_exit(const int err, char *fmt, ...);	/* print fmt+strerror()+\n */

#endif
