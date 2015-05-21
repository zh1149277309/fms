#ifndef _FMS_COMPLETION_H
#define _FMS_COMPLETION_H
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "fmsclient.h"

void initialize_fms_readline(struct server_attr *_attr);
#endif
