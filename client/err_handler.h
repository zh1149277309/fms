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
