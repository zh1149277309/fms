#ifndef _SEND_RESPONSE_H
#define _SEND_RESPONSE_H
#include "fmsserver.h"

inline void send_response(struct client_attr *attr);
void send_response_header(struct client_attr *attr);
void send_response_data(struct client_attr *attr);

#endif
