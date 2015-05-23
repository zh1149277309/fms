#include <string.h>
#include "err_handler.h"
#include "process_pwd.h"


/* 	Process PWD request from the client, send current word directory to client.
 *	*/
void process_pwd(struct client_attr *attr)
{

	/* 	to ensure follow of the strstr() return correct result, the attr->cwd
	 * 	structure must be add the strict restriction to keep always include
	 * 	the attr->rootdir;
	 * 	that's what I've done in the security_checking() function */

	/*
	if ((p = strstr(attr->cwd, attr->rootdir) + strlen(attr->rootdir)) != NULL)
		strcpy(attr->data, p);
	else
		strcpy(attr->data, "/");	*/

	strcpy(attr->data, EXCLROOTDIR(attr->cwd, attr->rootdir));
	SEND_DATA_TO_CLIENT(attr, RESP_PWD);
}

