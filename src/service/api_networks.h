//
// Created by root on 24-8-24.
//

#ifndef ANTTOOLKIT_API_NETWORKS_H
#define ANTTOOLKIT_API_NETWORKS_H

#include <event2/http.h>
#include <event2/buffer.h>

void atk_api_network_ip_trans(struct evhttp_request *req, void *arg);

#endif //ANTTOOLKIT_API_NETWORKS_H
