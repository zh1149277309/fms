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
#ifndef _FMSCLIENT_H
#define _FMSCLIENT_H
#include <netinet/in.h>		/* INET_ADDRSTRLEN */
#include <limits.h>		/* PATH_MAX */

#define DEFAULT_DL_DIR			"fmsdownload"
#define DEFAULT_DL_DIR_MODE		0700
#define DEFAULT_DL_FILE_MODE		0640

#define PORT	"40325"		/* Default port number and configure file*/
#define BUFSZ	1024
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLLEN	16	/* xxx.xxx.xxx.xxx + '\0' */
#endif
#define PORT_LEN	6		/* xxxxx + '\0' */

/* Request Code, All of the following code should keep same with server */
#define REQ_AUTH		0x10
#define REQ_LS			0x20
#define REQ_CD			0x21
#define REQ_PWD			0x22
#define REQ_MKDIR		0x23
#define REQ_RM			0x24
#define	REQ_UPLOAD		0x25
#define	REQ_DOWNLOAD		0x26
#define REQ_EXIT		0x27
#define REQ_DATA_FINISH		0x80



/* Response Code, Some responses doesn't send addtional data to client. Just
 * for comfirm. */
#define RESP_AUTH_OK		0x101
#define RESP_AUTH_ERR		0x102
#define RESP_LS			0x103
#define RESP_LS_ERR		0x104
#define RESP_CD			0x105
#define RESP_CD_ERR		0x106
#define RESP_PWD		0x107
#define RESP_DOWNLOAD		0x108
#define RESP_DOWNLOAD_ERR	0x109
#define RESP_UPLOAD		0x10A
#define RESP_UPLOAD_ERR		0x10B
#define RESP_EXIT		0x10C
#define RESP_SERVER_ERR		0x10D
#define RESP_MKDIR		0x10E
#define RESP_MKDIR_ERR		0x10F
#define RESP_RM			0x110
#define RESP_RM_ERR		0x111
#define RESP_DATA_FINISH	0x800


struct request {			/* All requests types of client */
	unsigned int code;		/* Requests code */
	unsigned int len;		/* The length of data */
};

struct response {
	unsigned int code;		/* Requests code */
	unsigned int len;		/* The length of data */
};


struct auth {
	char user[BUFSZ];
	char pwd[BUFSZ];
};

struct server_attr {
	int fd;
	struct auth auth;
	struct request req;
	struct response resp;
	char data[BUFSZ];		/* Store arguments or other data */
	char ip[INET_ADDRSTRLEN];
	char port[PORT_LEN];
	char cwd[PATH_MAX];
	char buf[BUFSZ];		/* buffer the addtional data */
};

#endif
