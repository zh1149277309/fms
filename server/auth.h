#ifndef _AUTH_H
#define _AUTH_H
#include "fmsserver.h"		/* Need for struct client_attr */

int auth(struct client_attr *attr);
int get_root_privilege(struct client_attr *attr);

#endif
