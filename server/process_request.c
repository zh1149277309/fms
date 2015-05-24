#include "debug.h"
#include "err_handler.h"
#include "process_ls.h"
#include "process_cd.h"
#include "process_pwd.h"
#include "process_mkdir.h"
#include "process_rm.h"
#include "process_download.h"
#include "process_upload.h"
#include "send_response.h"
#include "recv_request.h"
#include "process_request.h"


/* NOTE:
 * In general, mkdir and rm command is dangerous, so I decide to using
 * root privilege something like linux, only root user can using those
 * command listed above!  */
int process_request(struct client_attr *attr)
{

	switch (attr->req.code) {
	case REQ_LS:
		process_ls(attr);
		break;
	case REQ_CD:
		process_cd(attr);
		break;
	case REQ_PWD:
		process_pwd(attr);
		break;
	case REQ_MKDIR:
		process_mkdir(attr);
		break;
	case REQ_RM:
		process_rm(attr);
		break;
	case REQ_UPLOAD:
		/* (Principle of upload)
		 *	step1:	client(REQ_UPLOAD) --> server(RESP_UPLOAD)
		 *	step2:	client(files) --> server(files))
		 *	step3:	client(REQ_DATA_FINISH --> server()) */

		/* Notify the client start to upload files */
		attr->resp.code = RESP_UPLOAD;
		attr->resp.len = attr->req.len;
		send_response(attr);

		process_upload(attr);
		break;
	case REQ_DOWNLOAD:
		process_download(attr);
		break;
	case REQ_EXIT:
		attr->resp.code = RESP_EXIT;	/* Send the exit response */
		attr->resp.len = 0;
		send_response(attr);
		return REQ_EXIT;
		break;
	default:
		err_thread_exit(attr->fd, 0, "unrecognized command: %#X",
				attr->req.code);
	}

	return 0;	/* Suppress warning, it never reach here */
}
