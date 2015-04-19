#ifndef _RECV_RESPONSE_H
#define _RECV_RESPONSE_H
#include "fmsclient.h"

inline void recv_response(struct server_attr *attr);
void recv_response_header(struct server_attr *attr);
void recv_response_data(struct server_attr *attr);

#endif
