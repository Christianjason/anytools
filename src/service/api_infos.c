//
// Created by root on 24-8-24.
//

#include "api_infos.h"

// api/supports
/*
 * {
 *     "data":[
 *         {
 *             "network":["ip-trans", "ip-check"]
 *         }
 *     ]
 * }
 * */
static char *atk_support_string = "{\"data\":[{\"network\":[\"ip-trans\", \"ip-check\"]}\"]}";

void atk_api_supports(struct evhttp_request *req, void *arg){
    struct evbuffer *buf = evbuffer_new();
    if (!buf) {
        evhttp_send_reply(req, HTTP_INTERNAL, "Internal Server Error", NULL);
        return;
    }

    evbuffer_add_printf(buf, "%s", atk_support_string);
    evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "application/json");
    evhttp_send_reply(req, 200, "OK", buf);
    evbuffer_free(buf);
}