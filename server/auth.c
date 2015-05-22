#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include "debug.h"
#include "err_handler.h"
#include "send_response.h"
#include "recv_request.h"
#include "fmsserver.h"
/* #include "auth.h" */


/* Prototypes */
static int pass_init(char *file, FILE **fp);
static int pass_read(char *user, char *pwd, FILE *fp);
static void pass_close(FILE *fp);
static int is_auth_ok(char *file, char *u, char *p);
/* static void readn(int fd, char *buf, unsigned long len);	*/




/* Return 0, if authentication success, or -1 on error; if occurs fatal error, 
 * current thread will be exit */
int auth(struct client_attr *attr)
{
	int  /* s, */ auth_ok;
	char /*	*buf, */ *p;
/*	struct request req;
	struct response resp;	*/

	
/*	@improve on current code section
	
	if ((s = read(attr->fd, &req, sizeof(req))) != sizeof(req))
		err_thread_exit(attr->fd, s, "read");	
	
	debug("code=%d, len=%ld", req.code, req.len);
	
	if (req.code != REQ_AUTH)
		err_thread_exit(attr->fd, 0, "not a authentication command");
		
	if ((buf = malloc(req.len + 1)) == NULL)
		err_thread_exit(attr->fd, errno, "malloc buffer failed");
	
	
	readn(attr->fd, buf, req.len);	*/

 	recv_request(attr);
 	if (attr->req.code != REQ_AUTH) {
 		SEND_ERR_TO_CLIENT(attr, RESP_AUTH_ERR, 
				"Please authenticate user first");
 		err_thread_exit(attr->fd, 0, "Not a authentication request");
 	}
 	
 	
	if ((p = strchr(attr->data, ':')) == NULL) /* username:encrypted-passwd */
		err_thread_exit(attr->fd, 0, "Unrecogonized format of authentication");
	else
		*p++ = 0;	
	
	strcpy(attr->usrinfo.user, attr->data);
	strcpy(attr->usrinfo.pwd, p);
	
	debug("user=(%s), pwd=(%s)", attr->usrinfo.user, attr->usrinfo.pwd);
	auth_ok = is_auth_ok(attr->passfile, attr->usrinfo.user,
		attr->usrinfo.pwd);

	/* Response message to client, for authentication success */
	if (auth_ok)
		attr->resp.code = RESP_AUTH_OK;
	else
		attr->resp.code = RESP_AUTH_ERR;


	attr->resp.len = 0;
 	send_response(attr);
	
	return auth_ok ? 0 : -1;	
}



/*  NOTE:
 * 		You can implement a root privilege user list, if necessary!
 *	Return 0 if user has root privileges, otherwise, return -1.	*/
int get_root_privilege(struct client_attr *attr)
{
	if (strcmp("root", attr->usrinfo.user) == 0)
		return 0;
		
	return -1;
}





/* Return 1 on success, otherwise, 0 is returned if authentication failed */
static int is_auth_ok(char *file, char *u, char *p)
{
	int auth_ok;
	char user[BUFSZ], pwd[BUFSZ];
	FILE *fp;									/* used in pass_init() */
	
	auth_ok = 0;
	if (-1 == pass_init(file, &fp)) {
		err_msg(0, "pass_init");
		return 0;	
	}
	
	while (pass_read(user, pwd, fp) != -1) {
	/*	debug("strcmp: user(%s, %s) pwd(%s, %s)", user, u, pwd, p);	*/
		if (strcmp(user, u) == 0 && strcmp(pwd, p) == 0) {
			auth_ok = 1;
			break;
		}
	}
	
	pass_close(fp);
	return auth_ok;
}


/* Return 0 on success, otherwise, -1 is returned */
static int pass_init(char *file, FILE **fp)
{
/*	debug("password file=%s", file);	*/
	if ((*fp = fopen(file, "r")) == NULL) { 
		err_msg(errno, "fopen");
		return -1;
	}
	
	return 0;
}


/* On success, zero is returned, otherwise, -1 is returned */
static int pass_read(char *user, char *pwd, FILE *fp)
{
	char buf[BUFSZ], *p;

	if ((fgets(buf, BUFSZ, fp)) == NULL)
		return -1;
	if ((p = strchr(buf, ':')) == NULL)
		return -1;
	
	*p++ = 0;
	strcpy(user, buf);
	strcpy(pwd, p);
	
	if ((p = strchr(pwd, '\n')) != NULL)	/* Strip at the end of newline */
		*p = 0;				
	return 0;	
}

static void pass_close(FILE *fp)
{
	fclose(fp);
}




/* NOTE: 
 *     deprecated, instead by recv_request()
 * Read n bytes data, much simple 
static void readn(int fd, char *buf, unsigned long len)
{
	int n;
	while (len > 0) {
		n = read(fd, buf, len);	
		buf += n;
		len -= n;
	}
	
	*(buf + len) = 0;
}	*/



