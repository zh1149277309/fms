#include <errno.h>
#include <sys/stat.h>
#include "auth.h"
#include "common.h"
#include "process_mkdir.h"


void process_mkdir(struct client_attr *attr)
{
	char path[PATH_MAX];

	if (get_root_privilege(attr) == -1) {	/* No permissions */
		SEND_ERR_TO_CLIENT(attr, RESP_MKDIR_ERR, "Permission denied");
		return;	
	}
	
	if (resolve_path(attr, path) == -1) {	/* No argument */
		SEND_ERR_TO_CLIENT(attr, RESP_MKDIR_ERR, "Missing operand");
		return;	
	}	
	

	/* 	resolve the "." or ".." on the path */
	if (depth_resolve_path(attr, path) == -1) {
		SEND_ERR_TO_CLIENT(attr, RESP_MKDIR_ERR, "fail to mkdir");
		return;
	}
#define DIR_MODE	0740	
	if (mkdir(path, DIR_MODE) == -1) {
		SEND_ERR_TO_CLIENT(attr, RESP_MKDIR_ERR, "%s", strerror(errno));
		return;
	}
	
	/* 	Send null data to client, only confirm server's response */
	*attr->data = 0;
	SEND_DATA_TO_CLIENT(attr, RESP_MKDIR);
}
