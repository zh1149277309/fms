#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "debug.h"
#include "err_handler.h"
#include "set_request.h"


static int getrequestype(char *s);
static void strip_ws(char *cmd);


/* 	NOTE:	
 *		How to parsing input data, and encode it to server?
 *
 *		Example:
 *			[test@127.0.0.1 /]$ ls /foo\ bar/test.c
 *		Step1:
 *			strip repeating space characters whichs are ' ', '\t', '\n'... 
 *		Step2:
 *			the first encountered space as separate CMD and ARGS 
 *		Step3:
 *			send it to server;	when server parsing it, (space ' ' as default
 *			separate also, but except the string which are "\ ")*/
int set_request(char *cmd, struct server_attr *attr)
{
	char *p;
	
	strip_ws(cmd);							/* Step1 */

	*attr->data = 0;
	attr->req.len = 0;	
	if ((p = strchr(cmd, ' ')) != NULL) {	/* Step2 */
		*p++ = 0;		
	
		strcpy(attr->data, p);				/* May include "\ " */
		attr->req.len = strlen(attr->data);
		strcpy(attr->buf, p);				/* save current request data */
	}
	

	attr->req.code = getrequestype(cmd);	/* Get request code */
	if (attr->req.code == -1)
		return -1;


	
	debug("cmd=(%s)", cmd);
	debug("args (%s)", attr->data);
	debug("sending request to %s", attr->ip);
	return 0;
}



/* 	Return request type on success, or return -1 on error */
static int getrequestype(char *s)
{
#define RETURN_REQ_TYPE(cmd_str, type)					\
	if (strcmp(s, cmd_str) == 0)						\
		return type;

	RETURN_REQ_TYPE("ls", 		REQ_LS)
	RETURN_REQ_TYPE("pwd", 		REQ_PWD)
	RETURN_REQ_TYPE("cd", 		REQ_CD)
	RETURN_REQ_TYPE("mkdir",	REQ_MKDIR);
	RETURN_REQ_TYPE("rm",		REQ_RM);
	RETURN_REQ_TYPE("upload", 	REQ_UPLOAD)
	RETURN_REQ_TYPE("download", REQ_DOWNLOAD)
	RETURN_REQ_TYPE("exit", 	REQ_EXIT)

	return -1;
}




/* 	Strip the whitespace of the string cmd */
static void strip_ws(char *cmd)
{
	char buf[BUFSZ], *p, *p2;
	int _is_first_run, _is_space, _is_space_first;
	
	/* 	NOTE: 
	 *		Can not to increament of decrement the address of array;
	 * 		buf++; 	or buf += 1; 	both are error on compiling  */
	
	p = buf;
	p2 = cmd;
	_is_first_run = 1;
	_is_space_first = 1;
	while (1) {
		
		if (*p2 == '\0') {
			break;
		} else if ((_is_space = isspace(*p2)) && _is_first_run) {
			p2++;
			continue;
		} else if (_is_space && _is_space_first) {
			*p++ = ' ';				/* All whitespace instead by ' ' */
			_is_space_first = 0;
		} else if (!_is_space) { 
			*p++ = *p2;
			_is_first_run = 0;
			_is_space_first = 1;
		}
		
		p2++;
	}
	if (*(p - 1) == ' ')
		*(p - 1) = 0;
	else
		*p = 0;
	
	strcpy(cmd, buf);
}

