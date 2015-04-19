#ifndef _SEND_REQUEST_H
#define _SEND_REQUEST_H
#include "fmsclient.h"

inline void send_request(struct server_attr *attr);
void send_request_header(struct server_attr *attr);
void send_request_data(struct server_attr *attr);
void encode_args(char *src, char *args);

#endif
