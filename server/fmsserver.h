#ifndef _FMSSERVER_H
#define _FMSSERVER_H
/* 	Warning: 
 *		If defined here, there struct client_attr used in send_response 
 *		funtion doesn't declaration */
/*	#include "send_response.h"	*/	
#ifndef NAME_MAX		
#define NAME_MAX	255
#endif
#ifndef PATH_MAX
#define PATH_MAX	4096
#endif
#define BUFSZ	1024
#define DEFAULT_UPLOAD_FILE_MODE 0700


struct request {
	unsigned int code;		/* Request code */
	unsigned long len;		/* The length of data */
};

struct response {			
	unsigned int code;		/* Requests code */
	unsigned long len;		/* The length of data */
};

struct user_info {	
	char user[BUFSZ];		/* Save username */
	char pwd[BUFSZ];		/* Save encrypted password */
};

struct conf {				/* Settings by read from configure file */
	unsigned short port;
	char rootdir[PATH_MAX];	/* Restrict the path of user access */
	char passfile[PATH_MAX + NAME_MAX];	/* specify special password file */
};

struct client_attr {		/* /root/dir + /current/work/directory */
	int fd;
	char *rootdir;					/* Always no '/' on the tail */
	char cwd[PATH_MAX + NAME_MAX];	/* Always no '/' on the tail */
	char data[BUFSZ];
	char *passfile;
	struct request req;
	struct response resp;
	struct user_info usrinfo;
};


struct data {
	char buf[BUFSZ];
	struct data *next;		/* In the last message, next will be NULL */
};



#define PORT	40325		/* default port number and configure file*/
#define CONF_FILE 	"/usr/share/fmsserver/server.conf"
#define PASS_FILE	"passwd"



#include "send_response.h"
#include <stdio.h>
/* send data to client */
#define SEND_ERR_TO_CLIENT(attr, code_type, msg, ...)					\
do {																	\
	char str[BUFSZ];													\
	attr->resp.code = code_type;										\
	sprintf(str, msg, ##__VA_ARGS__);									\
	strcpy(attr->data, str);											\
	attr->resp.len = strlen(attr->data);								\
	send_response(attr);												\
} while (0)

/* send error message to client */
#define SEND_DATA_TO_CLIENT(attr, code_type)							\
	attr->resp.code = code_type;										\
	attr->resp.len = strlen(attr->data);								\
	send_response(attr);		


/* NOTES: 
 *		this macro should used only to strip the "rootdir" from the "cwd",
 *  since the implementation include the security-checking, so that string
 * 	"s" always include substring "subs"
 *	returned string that exclude the substring "subs".
 */
#include <string.h>
#define EXCLROOTDIR(s, subs)					\
	(strstr(s, subs) + strlen(subs))

/*	Request Code
 *	NOTE:
 *		All of the following code should keep same with client */
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


#endif
