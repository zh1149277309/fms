/* #define _BSD_SOURCE		Depreccated */
#define _DEFAULT_SOURCE	
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stddef.h>
#include <malloc.h>
#include <errno.h>
#include "debug.h"
#include "common.h"
#include "err_handler.h"
#include "send_response.h"
#include "process_ls.h"

static int getpath(struct client_attr *attr, char *buf);


/* I can using exec() sets to list files and directorys under current work
 * directory, but for shown the how to get informations about files, I decide
 * implements this function by myself */
void process_ls(struct client_attr *attr)
{
	int n;
	DIR *dp;
	char path[PATH_MAX];
	char buf[BUFSZ];
	struct dirent *entry, *result;
	

	/* Resolving the  */
	if (getpath(attr, path) == -1) {
		SEND_ERR_TO_CLIENT(attr, RESP_LS_ERR, "Bad path");	
		return;
	}


	if ((dp = opendir(path)) == NULL) {
		err_msg(errno, "opendir");
		SEND_ERR_TO_CLIENT(attr, RESP_LS_ERR, "%s", strerror(errno));
		return;
	}
	
	/* I'm not use pathconf(), I think this is ok */
	if ((entry = malloc(offsetof(struct dirent, d_name) +
			NAME_MAX + 1)) == NULL) {
		err_msg(errno, "malloc");
		goto process_ls_err_closedir;
	}
	
	
	*attr->data = 0;
	attr->resp.code = RESP_LS;
	attr->resp.len = 0;
	while ((readdir_r(dp, entry, &result) == 0) && result) {
		if (strcmp(entry->d_name, ".") == 0 ||
				strcmp(entry->d_name, "..") == 0)
			continue;

		strcpy(buf, entry->d_name);
		if (entry->d_type == DT_DIR)	/* DT_DIR not bit mask */
			strcat(buf, "/");
		strcat(buf, ":");


		n = strlen(buf);
		if (attr->resp.len + n >= BUFSZ) {
			send_response(attr);
			strcpy(attr->data, buf);	
			attr->resp.len = n;	/* Reset length of data */
		} else {
			strcat(attr->data, buf);
			attr->resp.len += n;
		}
	}
	
	closedir(dp);
	send_response(attr);

	/* Send the RESP_DATA_FINISH flag to client */
	attr->resp.code = RESP_DATA_FINISH;
	attr->resp.len = 0;
	send_response(attr);
	return;
	
process_ls_err_closedir:
	closedir(dp);	
	SEND_ERR_TO_CLIENT(attr, RESP_LS_ERR, "List the file or directorys failed");
}




/* Return 0 on success,  or security-checking() failed, return -1 */
static int getpath(struct client_attr *attr, char *path)
{
	/* It never return fails, since it req.len may be zero */
	resolve_path(attr, path);
	
	/* Multiple .. are specified */	
	if (depth_resolve_path(attr, path) == -1)
		return -1;	
		
	debug("resolving path: (%s)", path);	
	return 0;
}


/* NOTE: 
 *	FOLLING METHODS ARE DEPRECATED. 
 *	For make data tranlsate simple, If data size is more larger than BUFSZ
 * 	(1024), It will be separated BUFSZ packages sends to client.
 * 	instead by: send_response(), recv_request() 
 *//*
static void write_data_to_client(struct client_attr *attr, 
							struct data *data, unsigned long len)
{
	char *buf;
	struct data *curr;

	write_to_client(attr->fd, &attr->resp, sizeof(attr->resp));
	

	curr = data;
	buf = curr->buf;	
	while (len > 0) {
		if (len > BUFSZ) {
			write_to_client(attr->fd, buf, BUFSZ);
			curr = data->next;
			buf = curr->buf;
			len -= BUFSZ;
		} else {
			write_to_client(attr->fd, buf, len);
			break;
		} 
	}
}


static void write_to_client(const int fd, void *buf, unsigned long len)
{
	int n;
	
	while ((n = write(fd, buf, (size_t)len)) < len) {
		if (n == -1)
			err_thread_exit(fd, errno, "write");
		buf += n;
		len -= n;
	}
}

static void read_from_client(const int fd, char *buf, int len)
{
	int n;
	
	while ((n = read(fd, buf, len)) < len) {
		if (n == -1)
			err_thread_exit(fd, errno, "read");
		buf += n;
		len -= n;
	}
}*/

/* catnate the data to the structure data, return number of actully catnated,
 * or return -1, on error */ /*
static int datacat(struct client_attr *attr, char *buf)
{
	struct data *tp;
	int nwritn, srclen, buflen;
	
	if (NULL == attr->data) {
		if ((attr->data = malloc(sizeof(struct data))) == NULL)
			return -1;
		attr->curr = attr->data;
		curr->next = NULL;	
	}
	
	nwritn = 0;
	buflen = strlen(attr->curr->buf);
	srclen = strlen(buf);
	if (buflen + srclen > BUFSZ) {
		strncat(attr->curr->buf, buf, BUFSZ - buflen);
		
		nwritn += BUFSZ - buflen;
		if ((tp = malloc(sizeof(struct data))) == NULL) {
			return -1;
		}
		tp->next = NULL;
		attr->curr->next = tp;
		attr->curr = tp;
		buflen = 0;
		
		strncpy(attr->curr->buf, buf + nwritn, srclen - nwritn);
	} else {
		strcat(attr->curr->buf, buf);	
		nwritn += srclen;
	}
	
	attr->resp.len += nwritn;
	return nwritn;
}*/
