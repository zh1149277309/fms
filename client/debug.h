#ifndef _DEBUG_H
#define _DEBUG_H
#include <stdio.h>

/* For used to debug program */
#ifdef __DEBUG__
#define debug(fmt, ...) \
	fprintf(stderr, "[" __FILE__ ",%d] " fmt "\n", __LINE__, ##__VA_ARGS__);
#else
#define debug(fmt, ...)
#endif

#endif
