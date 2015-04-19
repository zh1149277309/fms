/* 	#define __DEBUG__ */
#define _GNU_SOURCE		/* getopt_long() */
#define _XOPEN_SOURCE	/* encrypt() */
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FMS_DEL 0x2
#define FMS_ADD 0x4
#define FMS_CHG 0x8
#define FMS_PRT 0x10
#define BUFSZ	1024

/* Use to debug */
#ifdef __DEBUG__
#define debug(fmt, ...) printf(__FILE__ ",%d: " fmt, __LINE__, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#endif


static struct option long_options[] = {
	{"delete", 	no_argument, 0, 'd'},
	{"add",		no_argument, 0, 'a'},
	{"change",	no_argument, 0, 'c'},
	{"print",	no_argument, 0, 'p'},
	{"help",	no_argument, 0, 'h'},
	{0,			0,			 0,  0 }
};


void help(char *prog)
{
	fprintf(stderr, "Usage: %s [options] user file\n", prog);
	fprintf(stderr, "Options:\n"
					"  -d, --delete     Delete the user\n"
					"  -a, --add        Add a new user\n"
					"  -c, --change     Change the passwod of user\n"
					"  -p, --print      Print encrypted password to stdout\n"
					"  -h, --help       Help\n");
	exit(EXIT_FAILURE);
}



/* Return username and password by read from the 'password' file */
int passwd_read(FILE *fp, char *name, char *pwd)
{
	char s[BUFSZ], *p;

	if ((fgets(s, BUFSZ, fp)) == NULL) {
		return -1;		/* I will not to use feof() to detecte end-of-file */
	}
	if ((p = strchr(s, ':')) == NULL)
		return -1;
	
	*p++ = 0;
	strcpy(name, s);
	strcpy(pwd, p);
	
	if ((p = strrchr(pwd, '\n')) != NULL)
		*p = 0;			/* Strip at the end of newline */
	return 0;
}

/* Read the password from stdin, and return encrypted user info */
void encode(char *encrypt_str)
{
	char *p, *p2;
	const char *const salt = "FMSIOPQWE/MNCBCKLASJZIOQWIEASD";
	
	if ((p = getpass("Enter Password:")) != NULL) {	   
		if ((p2 = crypt(p, salt)) == NULL) {
			fprintf(stderr, "Encrypt password was failed.\n");
			exit(EXIT_FAILURE);
		}
		memset(p, 0, strlen(p));
	
		strcat(encrypt_str, ":");		
		strcat(encrypt_str, p2);
		strcat(encrypt_str, "\n");
		
	} else {
		fprintf(stderr, "Get password was failed.\n");
		exit(EXIT_FAILURE);
	}
}


/* Return mattched uesr's position in the file, otherwise -1 returned */
long getpos(int fd, char *user)
{
	FILE *fp;
	char name[BUFSZ], pwd[BUFSZ];
	int is_find = 0;
	long pos;

	if ((fp = fdopen(fd, "r")) == NULL) {
		perror("Open the file occur error: ");
		exit(EXIT_FAILURE);		
	}	
	
	pos = ftell(fp);
	while (passwd_read(fp, name, pwd) != -1) {
		if (strcmp(user, name) == 0) {
			is_find = 1;
			break;
		}
		pos = ftell(fp);
	}
	/* 	If this statement was executed, the file descriptor of passwd()
	   	will be close also. */
	/*	fclose(fp);	*/
	return is_find ? pos : -1;
}

void printb(char *s)
{
	int i, j = strlen(s);
	for (i = 1; i <= j; i++) {
		printf("%x", *s++);
		if (i % 2 == 0)
			printf(" ");
	}
	printf("\n");
}


/* 	I don't sure, does this reason that non-complete-written are made my 
 *  program wrong! */
void nonblock_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	int nwritten;

	while (nmemb > 0) {
		nwritten = fwrite(ptr, size, nmemb, stream);	/* +1 for newline */
		nmemb -= nwritten;
		printf("nwritten: %d\n", nwritten);
	}
}

/* 	Set file's position to refered by fd, and save its postion */
void savepos(const int fd, long set_pos, long save_pos)
{
	save_pos = lseek(fd, 0, SEEK_CUR);	/* Save current position */
	lseek(fd, set_pos, SEEK_SET);		/* Move to the will be delete line */				
}


/* 	Recover the offset to the file */
void recoverpos(const int fd, long save_pos)
{
	lseek(fd, save_pos, SEEK_SET);		/* recover to last read */
}


/* 	Use to delete the info of the user, or change it if possible */
void _edit(const int fd, int op, char *user)
{
	char buf[BUFSZ], buf2[BUFSZ], tbuf[BUFSZ], *p;
	int nread, nread2, nwritn, is_first, len;
	long set_pos, save_pos, size;
	struct stat sb;
	
	len = 0;	
	save_pos = 0;
	*buf2 = *tbuf = 0;
	
	debug("_edit: user(%s)\n", user);
	
	set_pos = getpos(fd, user);	
	if (op & FMS_ADD && set_pos != -1) {		/* exist same user */
		fprintf(stderr, "Exist same user %s.\n", user);
		exit(EXIT_FAILURE);
	} else if ((op & (FMS_DEL | FMS_CHG)) && set_pos < 0) { 
		/* the user must be exist when delete or change */
		fprintf(stderr, "User %s does not exist.\n", user);
		exit(EXIT_FAILURE);
	}
	
	debug("_edit: fd=%d\n", fd);

	if (fstat(fd, &sb) == -1) {
		perror("fstat error: ");
		exit(EXIT_FAILURE);
	}
	
	size = sb.st_size;		/* In bytes */	

	is_first = 1;
	lseek(fd, set_pos, SEEK_SET);
	nread = read(fd, buf, BUFSZ);
	do {
		*(buf + nread) = 0;
		if (is_first) {
			if ((p = strchr(buf, '\n')) != NULL) {
				*p = 0;

				len = strlen(buf);	/* length of delete line, exclude \n */
	
				if (op & FMS_CHG) {	
					encode(user);	
					strcat(tbuf, user);
					strcat(tbuf, p + 1);
				} else if (op & FMS_DEL) {
					strcat(tbuf, p + 1);
				}
				
				nread2 = read(fd, buf2, BUFSZ);
				
				savepos(fd, set_pos, save_pos);
				nwritn = write(fd, tbuf, strlen(tbuf));
				recoverpos(fd, save_pos);
				
				memcpy(buf, buf2, BUFSZ);
				nread = nread2;
			
				is_first = 0;
			} else {		
				fprintf(stderr, "File format is incorrect.\n");			
				exit(EXIT_FAILURE);
			}			
			
		} else {
			set_pos += nwritn;
			
			nread2 = read(fd, buf2, BUFSZ);
			
			savepos(fd, set_pos, save_pos);
			nwritn = write(fd, tbuf, strlen(tbuf));
			recoverpos(fd, save_pos);
			
			memcpy(buf, buf2, BUFSZ);
			nread = nread2;
		}
	} while (nread > 0);
	
	if (op & FMS_DEL)
		ftruncate(fd, size - len - 1);		
}

/* Editor the file by according the parameter */
void edit(const int fd, char op, char *user)
{

	switch (op) {
	case FMS_DEL:	
		_edit(fd, op, user);
		break;
	case FMS_ADD:
		encode(user);		
		lseek(fd, 0, SEEK_END);
		write(fd, user, strlen(user));
		break;
	case FMS_CHG:		
	/* NOTE: 
	 *	I can write it straightly, bacause encrypted string has 13 chars 
	 *	always, But I want it used more generally. */		
	 
		_edit(fd, op, user);
		break;
	case FMS_PRT:
		encode(user);	
		printf("%s\n", user);
		break;
	}
	
}

void passwd(char *file, char *user, int op)
{
	int fd;
	int is_open, cnt, flags;
	mode_t mode;


	is_open = cnt = flags = 0;
	if (op & (FMS_DEL | FMS_ADD | FMS_CHG)) {
		is_open = 1;
		
		flags |= O_RDWR;
		if (op & FMS_ADD)
			flags |= O_CREAT;				
			
		mode = S_IRUSR | S_IWUSR | S_IRGRP;		
		if ((fd = open(file, flags, mode)) == -1) {
			perror("Open the file occur error: ");
			exit(EXIT_FAILURE);		
		}
	}
	

	/* According 'op' flags to decided which ADD or DELETE or CHANGE */
	edit(fd, op, user);	
	
	if (is_open)
		close(fd);
}



/* 	This program used to encrypt the user's password, and save to file */
int main(int argc, char **argv)
{
	char c;
	int opt_cnt, op;
	char user[BUFSZ], file[BUFSZ]; 	

	
	op = 0;
	opt_cnt = 0;
	while ((c = getopt_long(argc, argv, "dacph", long_options, NULL)) != -1) {
		switch (c) {
		case 'h':
			help(argv[0]);
			break;		/* for style */
		case 'd':
			op = FMS_DEL;
			opt_cnt++;
			break;
		case 'a':
			op = FMS_ADD;
			opt_cnt++;
			break;
		case 'c':
			op = FMS_CHG;
			opt_cnt++;
			break;
		case 'p':
			op = FMS_PRT;
			opt_cnt++;
			break;
		default:
			fprintf(stderr, "Unrecognized arguments: %c\n", c);	
			exit(EXIT_FAILURE);
		}
	}	

	
	if (opt_cnt != 1) 							  	/* options conflict */	
		help(argv[0]);	

	if ((op & (FMS_DEL | FMS_ADD | FMS_CHG)) && argc != 4) /* need 4 args */
		help(argv[0]);	

	if ((op & FMS_PRT) && argc != 3) 	/* need 3 args */
		help(argv[0]);	
	

	memset(user, 0, BUFSZ);		
	strcpy(user, argv[2]);
	if (op & (FMS_DEL | FMS_ADD | FMS_CHG))
		strcpy(file, argv[3]);	


	passwd(file, user, op);	

	return 0;
}
