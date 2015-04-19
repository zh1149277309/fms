#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include "debug.h"
#include "err_handler.h"
#include "common.h"
#include "send_response.h"
#include "process_download.h"


#define DOWNLOAD_INCLUDE_DIR_NAME	0x01 /* Return the string after last dir */
#define SEND_DL_ERR_TO_CLIENT(attr, msg, ...)	\
	SEND_ERR_TO_CLIENT(attr, RESP_DOWNLOAD_ERR, msg, ##__VA_ARGS__);
#define DOWNLOAD_ACCOMPLISH(attr)			\
	attr->resp.code = RESP_DOWNLOAD_ACCOM;	\
	attr->resp.len = 0;						\
	send_response(attr);


static char *brevity_name(char *pathname, int dir_name_length, int flags);
static int set_dir_name_length(char *pathname, int *dir_name_length);
static int transmit(struct client_attr *attr, char *pathname, 
					int dir_name_length, int flags);
static void download_recursive(struct client_attr *attr, char *path, 
					int dir_name_length);	

/* 	Response data format:
 *		first:	send 'length' + 'pathname';
 *		then:	send 'data's... 
 *	NOTE:
 *		single thread for download;
 *		no support wildcard, like '*';
 *		no support download the whole directory;
 *		
 *		If possible, I will implement thems; */
int process_download(struct client_attr *attr)
{
	int dir_name_length;								
	char pathname[PATH_MAX + NAME_MAX]; 
	struct stat sb;

	/*	Get fully path name */
	if (get_download_file(attr, pathname) == NULL) {
		SEND_DL_ERR_TO_CLIENT(attr, "Unknow file name");
		return -1;
	}	
	
	
	if (stat(pathname, &sb) == -1) {
		SEND_DL_ERR_TO_CLIENT(attr, "stat: %s", strerror(errno));
		return -1;
	} 
	
	if ((S_IFMT & sb.st_mode) == S_IFREG) {
		transmit(attr, pathname, 0, 0);
		DOWNLOAD_ACCOMPLISH(attr);
		return 0;
	} else if ((S_IFMT & sb.st_mode) == S_IFDIR) {
		/*	Set where is the index of last directory name start */
		if (set_dir_name_length(pathname, &dir_name_length) == -1)
			return -1;
		
		download_recursive(attr, pathname, dir_name_length);
		
		/*  When all files was download success, send RESP_DOWNLOAD_FINISH */
		DOWNLOAD_ACCOMPLISH(attr);
		return 0;
	} 
	
	return -1;
}


static void download_recursive(struct client_attr *attr, char *path, 
		int dir_name_length)
{
	DIR *dirp;
	struct dirent entry, *result;
	char tmp[PATH_MAX];
	
	debug("opendir: %s", path);
	if ((dirp = opendir(path)) == NULL) {
		return;
	}
	
	
	while ((readdir_r(dirp, &entry, &result) == 0) && result) {
		if (strcmp(".", entry.d_name) == 0 || strcmp("..", entry.d_name) == 0)
			continue;
		
		strcpy(tmp, path);
		if (*(tmp + strlen(tmp) - 1) != '/')
			strcat(tmp, "/");
		strcat(tmp, entry.d_name);	
		
		if (entry.d_type == DT_REG) {
		/*  Is a regular file, Just transmit it to client */
			transmit(attr, tmp, dir_name_length, DOWNLOAD_INCLUDE_DIR_NAME);
		} else if (entry.d_type == DT_DIR) {
		/*	Is a directory, recursive to download all things on it */
			download_recursive(attr, tmp, dir_name_length);
		}
	}
	
	
	closedir(dirp);
}



/*  Download the file which pointed by "pathname" */
static int transmit(struct client_attr *attr, char *pathname, 
		int dir_name_length, int flags)
{
	int fd;
	size_t length, n;	
	char *p;
	
	
	if ((fd = open(pathname, O_RDONLY)) == -1) {
		SEND_DL_ERR_TO_CLIENT(attr, "open file: %s", strerror(errno));
		return -1;
	}
	
	debug("Transmit: %s", pathname);
	/* 	Write file length to client */
	attr->resp.len = 0;
	length = lseek(fd, 0, SEEK_END);
	attr->resp.code = RESP_DOWNLOAD;
	memcpy(attr->data, &length, sizeof(size_t));			/* file length */
	attr->resp.len += sizeof(size_t);

	if ((p = brevity_name(pathname, dir_name_length, flags)) == NULL) {
		SEND_DL_ERR_TO_CLIENT(attr, "Unknown brevity name");
		return -1;
	}
	
	n = strlen(p);
	memcpy(attr->data + attr->resp.len, &n, sizeof(size_t));/* file name length */
	attr->resp.len += sizeof(size_t);
	
	memcpy(attr->data + attr->resp.len, p, n);				/* file name */
	attr->resp.len += n;
	send_response(attr);
	
	/*  Write data to client */
	lseek(fd, 0, SEEK_SET);
	while ((attr->resp.len = read(fd, attr->data, BUFSZ - 1)) > 0) {
		send_response(attr);
	}
	
	if (attr->resp.len == -1) {
		err_msg(errno, "Download occur error");
		return -1;	
	}
	close(fd);
	return 0;
}



static int set_dir_name_length(char *pathname, int *dir_name_length)
{
	size_t s;
	char *p;
	
	
	s = strlen(pathname);
	if (*(pathname + s - 1) == '/')
		*(pathname + s - 1) = 0;
		
	if ((p = strrchr(pathname, '/')) == NULL)
		return -1;
	
	*p = 0;
	*dir_name_length = strlen(pathname) + 1;	/* +1 for'/' */
	*p = '/';
	return 0;
}


/*	Return the pointer which point to the suitable path name for client to 
 *	store it on success; Otherwise, return NULL;	Example:
 *	
 *	NOTE:	(This may helpful to implement download a whole directory) 
 *		if requested download string is a single file, whatever it is absolute
 *		path name or relative path name, it returns file name only, exclusive 
 *		path;	For example:
 *			$download foobar.c	[or]	$downloaod /path/to/foobar.c 
 *				RETURN	"foobar.c"
 *
 *		if requested string is a path, For example:
 *			$download ../dir/ 	[or]	$download /path/to/dir/
 *				RETURN dir/foobar.c  
 *	The function return string does not include last directory on default! */
static char *brevity_name(char *pathname, int dir_name_length, int flags)
{
	if (flags == DOWNLOAD_INCLUDE_DIR_NAME) {
		return (pathname + dir_name_length);
	} else {
		return (strrchr(pathname, '/') + 1);
	}
}
