#include <stdio.h>
#include <string.h>
#include <errno.h>
/* 	#include "conf.h"	*/
#include "err_handler.h"
#include "fmsserver.h"

static FILE *fp;
static int line_cnt;

/* 	Open the configure file, return 0 on success, otherwise, -1 is returned */
int conf_init(char *conf)
{
	char *s;
	
	line_cnt = 0;	
	s = conf ? conf : CONF_FILE;
	
	if ((fp = fopen(s, "r")) == NULL)
		return -1;
	return 0;
}

/* 	Store the name and value in the configure file on success, and return 0 on
 * 	success, or -1 was returned when error occurs */
int conf_read(char *name, int nlen, char *val, int vlen)
{
	char buf[BUFSZ], *p;
	int save_errno;

	line_cnt++;
	
	/* Since fgets() not return the number of read actually */
	bzero(name, nlen);	
	bzero(val, vlen);
	
	save_errno = errno;
	
	errno = 0;
	if (fgets(buf, BUFSZ, fp) == NULL) {	
		if (errno)
			err_msg(errno, "conf_read");
			
		errno = save_errno;
		return -1;	
	}
	
	if ((p = strrchr(buf, '\n')) != NULL)	/* Strip at the end of '\n' */
		*p = 0;
	if (strlen(buf) == 0) {
		conf_read(name, nlen, val, vlen);	/* Whitespace line */
		return 0;	
	}
	
	/* Separate the NAME=VALUE pairs to arguments name and val */
	if ((p = strchr(buf, '=')) == NULL) 
		err_exit(0, "bad configure file(line %d): %s", line_cnt, buf);
	else	
		*p++ = 0;
		
	strcpy(name, buf);
	strcpy(val, p);
	return 0;
}

void conf_close(void)
{
	fclose(fp);
}
