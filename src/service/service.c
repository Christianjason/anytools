//
// Created by root on 24-8-24.
//

#include "service.h"
#include "cJSON.h"
#include "api_infos.h"
#include "api_networks.h"
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int evhttp_url_register(struct evhttp *http)
{
    if(NULL == http) {
        return -1;
    }

    evhttp_set_cb(http, "/api/supports", atk_api_supports, NULL);
    evhttp_set_cb(http, "/networks/ip_trans", atk_api_network_ip_trans, NULL);

    return 0;
}

int service_init(int argc, char **argv)
{
    struct event_base *base;
    struct evhttp *http;
    struct evhttp_bound_socket *handle;

    base = event_base_new();
    if (!base) {
        puts("Couldn't create an event_base: exiting.");
        return 1;
    }

    http = evhttp_new(base);
    if (!http) {
        puts("Couldn't create evhttp. Exiting.");
        return 1;
    }

    evhttp_url_register(http);

    handle = evhttp_bind_socket_with_handle(http, "0.0.0.0", 8080);
    if (!handle) {
        puts("Couldn't bind to port 8080. Exiting.");
        return 1;
    }

    printf("Server started on port 8080...\n");
    event_base_dispatch(base);

    evhttp_free(http);
    event_base_free(base);
    return 0;
}
