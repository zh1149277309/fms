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
