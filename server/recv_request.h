#ifndef _RECV_REQUEST_H
#define _RECV_REQUEST_H
#include "fmsserver.h"

inline void recv_request(struct client_attr *attr);
void recv_request_header(struct client_attr *attr);
void recv_request_data(struct client_attr *attr);
void decode_args(char *src, char *args);

#endif

