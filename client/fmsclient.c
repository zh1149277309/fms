#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>	/* not any help for linux, but for compatiblity */
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <crypt.h>
#include <errno.h>
#include "debug.h"
#include "send_request.h"
#include "recv_response.h"
#include "set_request.h"
#include "process_response.h"
#include "fms_completion.h"
#include "err_handler.h"
#include "fmsclient.h"

static int auth(struct server_attr *attr);
static void help(void);
static char *getpassword();
static void init_cli(char *arg, struct server_attr *attr);
static int cli_conn(struct server_attr *attr);
static int auth(struct server_attr *attr);

static char *progname;


static void help(void)
{
	fprintf(stderr, "Usage: %s [options] user@IP\n", progname);
	fprintf(stderr, "  -p    Port number\n");
	exit(EXIT_FAILURE);
}

/* Return last directory in the string s */
static char *getcwdpr(char *s)
{
	char *p;
	
	if (( p = strrchr(s, '/')) != NULL) {
		p++;
		return (*p == 0) ? "/" : p;
	} else
		return s;
}


/* Not thread-safe, but we use it only first connect to server */
static char *getpassword()
{
	char *p, *p2;
	const char *const salt = "FMSIOPQWE/MNCBCKLASJZIOQWIEASD";
	
	if ((p = getpass("Entry Password: ")) == NULL)
		err_exit(errno, "getpass");
	
	if ((p2 = crypt(p, salt)) == NULL)
		err_exit(errno, "crypt");

	bzero(p, strlen(p));
	return p2;			
}


/* Setting user and passwd of structure auth, and port with ip */
static void init_cli(char *arg, struct server_attr *attr)
{
	char *p, *p2;
	
	p2 = getpassword();
	
	if ((p = strchr(arg, '@')) == NULL)
		help();
	else	
		*p++ = 0;	
	
	strcat(attr->auth.user, arg);		
	strcat(attr->auth.pwd, p2);	/* username:encrypted-password */
	strcpy(attr->ip, p);		/* IP string */
	
	if (*attr->port == 0)
		strcpy(attr->port, PORT);
	strcpy(attr->cwd, "/");	/* First login, assume directory is '/'*/
	
	/*if (mkdir(DEFAULT_DL_DIR, DEFAULT_DL_DIR_MODE) == -1 &&
			errno != EEXIST) 
		err_exit(errno, "mkdir");*/
	initialize_fms_readline(attr);
	debug("initial client finish");
}


static int cli_conn(struct server_attr *attr)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;


	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = /*AI_NUMERICHOST | */AI_NUMERICSERV;

	if (getaddrinfo(attr->ip, attr->port, &hints, &result) != 0)
		err_exit(errno, "getaddrinfo");

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		if ((attr->fd = socket(rp->ai_family, 
				rp->ai_socktype, rp->ai_protocol)) == -1)
			continue;

		if (connect(attr->fd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;
		close(attr->fd);
	}	
	if (NULL == rp)
		err_exit(0, "could not connect to server");

	freeaddrinfo(result);
	debug("conncetion established");
	return attr->fd;
}

/* If authentication success, return 0, otherwise, -1 returned */
static int auth(struct server_attr *attr)
{
	strcpy(attr->data, attr->auth.user);
	strcat(attr->data, ":");
	strcat(attr->data, attr->auth.pwd);


	attr->req.code = REQ_AUTH;
	attr->req.len = (unsigned long)strlen(attr->data);

	/*Write header of request to server */
	send_request(attr);

	/* Read message from the server, ensure authenticate was success */
	recv_response(attr);
	/*debug("response code (%#X)", attr->resp.code);*/
	if (attr->resp.code == RESP_AUTH_OK)
		return 0;
	return -1;
}



int main(int argc, char **argv)
{
	int opt;
	struct server_attr attr;
	char buf[BUFSZ], *cmd;

	progname = argv[0];
	if (argc < 2)
		help();

	bzero(&attr, sizeof(attr));
	while ((opt = getopt(argc, argv, "p:")) != -1) {
		switch (opt) {
		case 'p':
			strcpy(attr.port, optarg);
			break;
		default:
			help();
		}
	}

	init_cli(argv[optind], &attr);
	cli_conn(&attr);

	if (auth(&attr) == -1)
		err_exit(0, "authentication failed");

	while (1) {
		sprintf(buf, "[%s@%s %s]$ ", attr.auth.user, attr.ip,
			getcwdpr(attr.cwd));
		cmd = readline(buf);	

		if (set_request(cmd, &attr) == -1) {
			printf("Command not found...\n");
			continue;
		}

		free(cmd);
		send_request(&attr);	/* Send request to server */
		recv_response(&attr);
		if (process_response(&attr) == RESP_EXIT)
			break;
	}	

	return 0;
}
