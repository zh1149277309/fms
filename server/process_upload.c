#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>
#include "debug.h"
#include "err_handler.h"
#include "recv_request.h"
#include "process_upload.h"


static void create_upload_dir(struct client_attr *attr, char *dirname);

void process_upload(struct client_attr *attr)
{
	int fd;
	size_t length, n;
	char pathname[PATH_MAX + NAME_MAX + 1];


download_next:
	/* Format: | size_t file-length | size_t file-name-length | file-name */
	length = *((int *)attr->data);
	n = *((int *)(attr->data + sizeof(size_t)));

	bzero(pathname, PATH_MAX + NAME_MAX + 1);
	strcpy(pathname, attr->cwd);
	if (*(pathname + strlen(pathname) - 1) != '/')
		strcat(pathname, "/");

	strncat(pathname, (attr->data + sizeof(size_t) * 2), n);


	create_upload_dir(attr, pathname);/* create directory if necessary */
	debug("file: %s, length: %ld", pathname, length);

	/* Overwrite exist file */
	if ((fd = open(pathname, O_WRONLY | O_CREAT,
				DEFAULT_UPLOAD_FILE_MODE)) == -1) {
		err_msg(errno, "Error for open %s", pathname);
		return;
	}


	while (length > 0) {
		recv_request(attr);
		write(fd, attr->data, attr->req.len);
		length -= attr->req.len;
	}

	close(fd);

	recv_request(attr);
	if (attr->req.code != REQ_DATA_FINISH)
		goto download_next;
}



/* The created directory is appending on current work directory! */
static void create_upload_dir(struct client_attr *attr, char *dirname)
{
	char *p;
	char tmp[PATH_MAX], dir[PATH_MAX + NAME_MAX + 1];
	struct stat sb;


	*tmp = 0;
	strcpy(dir, dirname);
	while ((p = strchr(dir, '/')) != NULL) {
		*p++ = 0;

		strcat(tmp, dir);
		strcat(tmp, "/");
		/*  Relative path name, create the directory if it does not exist */
		if (stat(tmp, &sb) == -1 && errno == ENOENT) {
			while (mkdir(tmp, DEFAULT_UPLOAD_FILE_MODE) == -1);
		}

		strcpy(dir, p);
	}
}

