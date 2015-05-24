#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include "debug.h"
#include "common.h"
#include "err_handler.h"
#include "recv_response.h"
#include "send_request.h"
#include "fmsclient.h"

#define UPLOAD_INCLUDE_DIR_NAME	0x01
#define UPLOAD_FINISH(attr)			\
	attr->req.code = REQ_DATA_FINISH;	\
	attr->req.len = 0;			\
	send_request(attr);

/* Hidden the implementations */
static void process_ls(struct server_attr *attr);
static void process_cd(struct server_attr *attr);
static void process_pwd(struct server_attr *attr);
static void process_download(struct server_attr *attr);
static void create_download_dir(char *dir);
static char *brevity_name(char *pathname, int dir_name_length, int flags);
static int set_dir_name_length(char *pathname, int *dir_name_length);
static int transmit(struct server_attr *attr, char *pathname,
		int dir_name_length, int flags);
static void upload_recursive(struct server_attr *attr, char *path,
		int dir_name_length);
static void process_upload(struct server_attr *attr);



int process_response(struct server_attr *attr)
{
	switch (attr->resp.code) {
	case RESP_LS:
		process_ls(attr);
		break;
	case RESP_CD:
		process_cd(attr);
		break;
	case RESP_PWD:
		process_pwd(attr);
		break;
	case RESP_MKDIR:		/* Success */
		break;
	case RESP_RM:
		break;
	case RESP_DOWNLOAD:
		process_download(attr);
		break;
	case RESP_UPLOAD:
		process_upload(attr);
		break;
	case RESP_LS_ERR:		/* Print error message just */
	case RESP_CD_ERR:
	case RESP_MKDIR_ERR:
	case RESP_RM_ERR:
	case RESP_DOWNLOAD_ERR:
	case RESP_UPLOAD_ERR:
		printf("%s\n", attr->data);
		break;
	case RESP_EXIT:
		return RESP_EXIT;	/* Receive exit confirm */
		break;
	default:
		err_exit(0, "UNKNOWN RESPONSE: (%#X)", attr->resp.code);
	}
	return 0;
}


static void process_ls(struct server_attr *attr)
{
	char *p, *p2;
	do {
		p = attr->data;
		while ((p2 = strchr(p, ':')) != NULL) {
			*p2++ = 0;
			printf("%s\n", p);
			p = p2;
		}
		recv_response(attr);
	} while(attr->resp.code != RESP_DATA_FINISH);
}




static void process_cd(struct server_attr *attr)
{
	strcpy(attr->cwd, attr->data);
}




/* Print the response from the server only */
static void process_pwd(struct server_attr *attr)
{
	printf("%s\n", attr->data);
}




static void process_download(struct server_attr *attr)
{
	int fd;
	size_t length, n;
	char str[BUFSZ];
	char filename[NAME_MAX + 1];


download_next:
	/* Format: | size_t file-length | size_t file-name-length | file-name */
	length = *((int *)attr->data);

	n = *((int *)(attr->data + sizeof(size_t)));
	strncpy(filename, (attr->data + sizeof(size_t) * 2), n);
	*(filename + n) = 0;

	create_download_dir(filename);	/* create directory if necessary */
	


	/* Overwrite exist file */
	fd = open(filename, O_WRONLY | O_CREAT, DEFAULT_DL_FILE_MODE);
	if (fd == -1) {
		err_msg(errno, "Error for open %s", filename);
		return;
	}

	SET_PROGRESS_DOWNLOAD(str, filename, length);
	while (n <= length) {
		recv_response(attr);
		writen(fd, attr->data, attr->resp.len);

		n += attr->resp.len;
		PRINT_PROGRESS(str, length, n);
	}

	close(fd);

	/* Confirm does recived the RESP_DATA_FINISH flag */
	recv_response(attr);
	if (attr->resp.code != RESP_DATA_FINISH)
		goto download_next;
}



/* The created directory is appending on current work directory! */
static void create_download_dir(char *pathname)
{
	char *p;
	char tmp[PATH_MAX], dir[PATH_MAX + NAME_MAX + 1];
	struct stat sb;



	*tmp = 0;
	/* Not implement download directory, this helpless */
	/*strcat(tmp, DEFAULT_DL_DIR);*/
	strcpy(dir, pathname);

	while ((p = strchr(dir, '/')) != NULL) {
		*p++ = 0;

		strcat(tmp, dir);
		strcat(tmp, "/");
		/*  Relative path name, create the directory if it does not
		 *  exist */
		if (stat(tmp, &sb) == -1 && errno == ENOENT) {
			while (mkdir(tmp, DEFAULT_DL_DIR_MODE) == -1);
		}

		strcpy(dir, p);
	}
}



/* Send files to server */
static void process_upload(struct server_attr *attr)
{
	int dir_name_length;
	char pathname[PATH_MAX + NAME_MAX];
	struct stat sb;


	strcpy(pathname, attr->data);
	/*if (get_upload_file(attr, pathname) == NULL)
		return;
	*/
	if (stat(pathname, &sb) == -1) {
		err_msg(errno, "stat");
		return;
	}


	if ((S_IFMT & sb.st_mode) == S_IFREG) {
		transmit(attr, pathname, 0, 0);
		UPLOAD_FINISH(attr);
		return;
	} else if ((S_IFMT & sb.st_mode) == S_IFDIR) {
		/* Set where is the index of last directory name start */
		if (set_dir_name_length(pathname, &dir_name_length) == -1)
			return;

		upload_recursive(attr, pathname, dir_name_length);

		/* When all files was download success, send
		 * RESP_DOWNLOAD_FINISH */
		UPLOAD_FINISH(attr);
		return;
	}

	return;
}




static void upload_recursive(struct server_attr *attr, char *path,
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
		if (strcmp(".", entry.d_name) == 0 ||
				strcmp("..", entry.d_name) == 0)
			continue;

		strcpy(tmp, path);
		if (*(tmp + strlen(tmp) - 1) != '/')
			strcat(tmp, "/");
		strcat(tmp, entry.d_name);

		if (entry.d_type == DT_REG) {
		/* Is a regular file, Just transmit it to client */
			transmit(attr, tmp, dir_name_length,
					UPLOAD_INCLUDE_DIR_NAME);
		} else if (entry.d_type == DT_DIR) {
		/* Is a directory, recursive to download all things on it */
			upload_recursive(attr, tmp, dir_name_length);
		}
	}

	closedir(dirp);
}



/* Download the file which pointed by "pathname" */
static int transmit(struct server_attr *attr, char *pathname,
		int dir_name_length, int flags)
{
	int fd;
	size_t length, n;
	char *p;
	char str[BUFSZ];


	if ((fd = open(pathname, O_RDONLY)) == -1) {
		err_msg(errno, "open");
		return -1;
	}

	debug("upload: %s", pathname);
	debug("Transmit: %s", pathname);

	/* File's length */
	attr->req.len = 0;
	length = lseek(fd, 0, SEEK_END);
	attr->req.code = RESP_UPLOAD;
	memcpy(attr->data, &length, sizeof(size_t));
	attr->req.len += sizeof(size_t);

	if (!flags) {
		/* The pathname include filename only. */
		n = strlen(pathname);
		p = pathname;
	} else if (flags == UPLOAD_INCLUDE_DIR_NAME) {
		/* Setting the UPLOAD_INCLUDE_DIR_NAME flag */
		p = brevity_name(pathname, dir_name_length, flags);
		n = strlen(p);
	}
	printf("%s, %d\n", pathname, dir_name_length);

	/* Filename's length */
	memcpy(attr->data + attr->req.len, &n, sizeof(size_t));
	attr->req.len += sizeof(size_t);

	/* Filename */
	memcpy(attr->data + attr->req.len, p, n);
	attr->req.len += n;
	send_request(attr);


	SET_PROGRESS_UPLOAD(str, p, length);

	/* Send data to server*/
	n = 0;
	lseek(fd, 0, SEEK_SET);
	while ((attr->req.len = read(fd, attr->data, BUFSZ - 1)) > 0) {
		send_request(attr);
		n += attr->req.len;
		PRINT_PROGRESS(str, length, n);
	}

	if (attr->resp.len == -1) {
		err_msg(errno, "Upload occur error");
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



static char *brevity_name(char *pathname, int dir_name_length, int flags)
{
	if (flags == UPLOAD_INCLUDE_DIR_NAME) {
		return (pathname + dir_name_length);
	} else {
		return (strrchr(pathname, '/') + 1);
	}
}


