#ifndef _FMSCLIENT_H
#define _FMSCLIENT_H
#include <netinet/in.h>		/* INET_ADDRSTRLEN */
#include <limits.h>			/* PATH_MAX */

#define DEFAULT_DL_DIR			"fmsdownload"
#define DEFAULT_DL_DIR_MODE		0700
#define DEFAULT_DL_FILE_MODE	0640

#define PORT	"40325"		/* Default port number and configure file*/
#define BUFSZ	1024
#ifndef INET_ADDRSTRLEN	
#define INET_ADDRSTRLLEN	16	/* xxx.xxx.xxx.xxx + '\0' */
#endif
#define PORT_LEN	6			/* xxxxx + '\0' */

/*	Request Code
 *	NOTE:
 *		All of the following code should keep same with server */
#define REQ_AUTH			0x10
#define REQ_LS				0x20
#define REQ_CD				0x21
#define REQ_PWD				0x22
#define REQ_MKDIR			0x23
#define REQ_RM				0x24
#define	REQ_UPLOAD			0x25
#define REQ_UPLOAD_ACCOM	0x26
#define	REQ_DOWNLOAD		0x27	
#define REQ_EXIT			0x28



/* 	Response Code
 *  NOTE:
 *  	Some responses doesn't send addtional data to client. Just for 
 *  	comfirm. */
#define RESP_AUTH_OK		0x101
#define RESP_AUTH_ERR		0x102	
#define RESP_LS				0x103
#define RESP_LS_ERR			0x104
#define RESP_CD				0x105
#define RESP_CD_ERR			0x106
#define RESP_PWD			0x107
#define RESP_DOWNLOAD		0x108
#define RESP_DOWNLOAD_ERR	0x109
#define RESP_DOWNLOAD_ACCOM	0x10A
#define RESP_UPLOAD			0x10B
#define RESP_UPLOAD_ERR		0x10C
#define RESP_EXIT			0x10D
#define RESP_SERVER_ERR		0x10E
#define RESP_MKDIR			0x10F
#define RESP_MKDIR_ERR		0x111
#define RESP_RM				0x112
#define RESP_RM_ERR			0x113


struct request {			/* All requests types of client */
	unsigned int code;		/* Requests code */
	unsigned long len;		/* The length of data */
};

struct response {			
	unsigned int code;		/* Requests code */
	unsigned long len;		/* The length of data */
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


struct data {
	char buf[BUFSZ];
	struct data *next;		/* In the last message, next will be NULL */
};


#endif
