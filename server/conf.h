#ifndef _CONF_H
#define _CONF_H

int conf_init(char *conf);
int conf_read(char *name, int nlen, char *val, int vlen);
void conf_close();

#endif
