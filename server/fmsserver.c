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
#define _GNU_SOURCE		/* get_current_dir_name() */
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "auth.h"
#include "conf.h"
#include "recv_request.h"
#include "send_response.h"
#include "err_handler.h"
#include "process_request.h"
#include "debug.h"
#include "fmsserver.h"


static struct conf conf;

/*
static void cleanup_thread(void *fd)
{
	debug("CLOSE FILE DESCRIPTOR (%d)", *(int *)fd);
	close(*((int *)fd));
	free(fd);
}*/


static void help(char *progname)
{
	fprintf(stderr, "Usage: %s [options]\n", progname);
	fprintf(stderr, "  -f file    Specify the configure file\n"
					"  -h         Print help message\n");
	exit(EXIT_FAILURE);
}



/* All requests of clients, will be processed under this session */
static void *session(void *_fd)
{
	int fd = *((int *)_fd);
	struct client_attr attr;	/* Thread-level attributes */

	debug("new session: %d", fd);

	/* Initialize the structure client_attr */
	attr.fd = fd;
	attr.rootdir = conf.rootdir;
	attr.passfile = conf.passfile;
	strcpy(attr.cwd, conf.rootdir);
	strcat(attr.cwd, "/");

	/*pthread_cleanup_push(cleanup_thread, _fd);
	pthread_cleanup_pop(0);	*/

	if (auth(&attr) == -1)		/* User authentication failed */
		err_thread_exit(fd, 0, "authentication was failed");

	/* NOTE:
	 * if encounter the end of stream, it will return 0, exit by
	 * reciving the REQ_EXIT request from clients */
	while (1) {
		recv_request(&attr);	/* include header and data */
		if (process_request(&attr) == REQ_EXIT)
			break;
	}

	close(fd);
	free(_fd);
	return (void *)0;
}




static void new_thread(const int *fd)
{
	int s;
	pthread_t thread;

	if ((s = pthread_create(&thread, NULL, session, (void *)fd)) != 0) {
		err_msg(s, "pthread_create");
		return;
	}

	if ((s = pthread_detach(thread)) != 0)
		err_msg(s, "pthread_join");
}



/* Listen the port for accept the requests from the client, default:
 * PROT=40325 */
static void conn_listen(unsigned short port)
{
	int flags;
	int sockfd, *fd;
	socklen_t clilen;
	struct sockaddr_in serv_in, cli_in;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_exit(errno, "socket");

	/* Use SO_REUSEADDR to reuse local addresses, if it's not active */
	flags = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&flags,
			sizeof(flags)) == -1)
		err_exit(errno, "setsockopt");

	bzero(&serv_in, sizeof(serv_in));
	serv_in.sin_family = AF_INET;
	serv_in.sin_port = htons(port);
	serv_in.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr *)&serv_in,
			(socklen_t)sizeof(struct sockaddr_in)) == -1)
		err_exit(errno, "bind");

	if (listen(sockfd, 100) == -1)
		err_exit(errno, "listen");

	/* Start to listen the port, and accept the request */
	clilen = sizeof(cli_in);
	while (1) {
		while ((fd = malloc(sizeof(int))) == NULL);

		*fd = accept(sockfd, (struct sockaddr *)&cli_in, &clilen);
		if (*fd == -1)
			err_msg(errno, "accept");

	/* Create a new thread to process this connection --- Iterations */
		new_thread(fd);
	}

	/* This will never be executed, odd! */
	close(sockfd);
}



/* Read configrue settings from the file, and store them to 'struct conf' */
static void server_init(char *file)
{
	char name[BUFSZ], val[BUFSZ];
	char *p;

	bzero(&conf, sizeof(conf));

	if (conf_init(file) == -1)
		err_exit(0, "conf_init failed");

	while (conf_read(name, BUFSZ, val, BUFSZ) == 0) {
		if (strcmp("port", name) == 0)
			conf.port = (unsigned short)atoi(val);
		else if (strcmp("rootdir", name) == 0)
			strcpy(conf.rootdir, val);
		else if (strcmp("passfile", name) == 0)
			strcpy(conf.passfile, val);
		else
			err_exit(0, "invalid configure: %s=%s\n", name, val);
	}

	p = get_current_dir_name();
	if (conf.port == 0)
		conf.port = PORT;	/* Default PORT number 40325 */
	if (*conf.rootdir == 0)
		strcpy(conf.rootdir, p);
	free(p);

	p = (conf.rootdir + strlen(conf.rootdir) - 1);
	if (*p == '/')			/* Ensure no '/' end with rootdir */
		*p = 0;


	conf_close();
}


int main(int argc, char **argv)
{
	char *file;
	int opt;

	file = NULL;
	while ((opt = getopt(argc, argv, "f:h")) != -1) {
		switch (opt) {
		case 'f':
			strcpy(file, optarg);
			break;
		case 'h':
			help(argv[0]);
		default:
			help(argv[0]);
		}
	}
	server_init(file);

	conn_listen(conf.port);
	return 0;
}
