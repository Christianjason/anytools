//
// Created by root on 24-8-24.
//

#ifndef ANTTOOLKIT_API_INFOS_H
#define ANTTOOLKIT_API_INFOS_H

#include <event2/http.h>
#include <event2/buffer.h>

void atk_api_supports(struct evhttp_request *req, void *arg);
#endif //ANTTOOLKIT_API_INFOS_H
