#include <stdlib.h>
#include <string.h>
#include "send_request.h"
#include "recv_response.h"
#include "fms_completion.h"

static struct server_attr *attr;

static char *dupstr(const char *s);
static char *file_list();
char **fms_completion(const char *text, int start, int end);
char *fms_command_generator(const char *text, int state);
char *fms_server_file_generator(const char *text, int state);
char *fms_local_file_generator(const char *text, int state);


/* Commands of the File Management System supported */
static char *commands[]= {
	"ls", "pwd", "cd", "download", "upload", "exit", NULL
};



/* Tell GNU Readline library how to complete. */
void initialize_fms_readline(struct server_attr *_attr)
{
	/* Use it for communicate with server */
	attr = _attr;
	/* Allow to parsing the ~/.inputrc configure file */
	rl_readline_name = "FMS";
	rl_attempted_completion_function = fms_completion;
}


/* Attempt to complete on the contents of argument text. start is the first
 * location of last string, end is the position which string ending. */
char **fms_completion(const char *text, int start, int end)
{
	char *p, cmd[BUFSZ];
	char **matches;

	matches = (char **)NULL;
	/* If text is at the start of of the line, then it is a command to
	 * complete, Otherwise it is the file name in the current directory. */
	if (start == 0) {
		matches = rl_completion_matches(text, fms_command_generator);
	} else {
		/* Get the name of command, and store completion string */
		strcpy(cmd, rl_line_buffer);
		if ((p = strchr(cmd, ' ')) != NULL)
			*p++ = 0;

		if (strcmp(cmd, "download") == 0 || strcmp(cmd, "ls") == 0)
			matches = rl_completion_matches(text,
					fms_server_file_generator);
/*		else
			matches = rl_completion_matches(text,
					fms_local_file_generator);
*/
	}
	
	return matches;
}


/* Generator function for command completion. */
char *fms_command_generator(const char *text, int state)
{
	static int command_index, len;
	char *name;

	if (!state) {
		command_index = 0;
		len = strlen(text);
	}

	while ((name = commands[command_index])) {
		command_index++;

		if (strncmp(name, text, len) == 0)
			return dupstr(name);
	}
	
	return NULL;
}


/* Generator file name in the current directory, of the path name on the
 * server */
char *fms_server_file_generator(const char *text, int state)
{
	static char *list, *p;
	char *p1, *p2;
	int len;
	
	if (!state) {
		list = file_list();
		p = list;
		len = strlen(text);
	}

	while ((p1 = strchr(p, ':')) != NULL) {
		p2 = p;
		*p1++ = 0;
		p = p1;
		if (strncmp(p2, text, len) == 0)
			return dupstr(p2);
	}

	/* All files are searched, free the list */
	free(list);
	return NULL;
}


/* Return a file list on server by dynamically allocated */
static char *file_list()
{
	char *list;
	int remain, total;

	/* Allocate first space for store file list */
	if ((list = malloc(BUFSZ)) == NULL)
		return NULL;
	remain = BUFSZ;
	total = BUFSZ;
	*list = 0;

	/* Request retriving the file list from server */
	attr->req.code = REQ_LS;
	attr->req.len = 0;
	send_request(attr);

	/* Process the file list had recived */
	recv_response(attr);
	while (attr->resp.code != RESP_DATA_FINISH) {
try_save_list:
		if (remain - attr->resp.len > 0) {
			strcat(list, attr->data);
			remain -= attr->resp.len;
		} else {
			/* Space of list is too small, reallocate it */
			total = total * 2;
			list = realloc(list, total);
			if (list == NULL)
				return NULL;
			remain = total - strlen(list);
			goto try_save_list;
		}
		recv_response(attr);
	}

	return list;
}

/* Duplicate the string s, and return it. */
static char *dupstr(const char *s)
{
	char *r;

	r = malloc(strlen(s) + 1);
	strcpy(r, s);
	return r;
}
